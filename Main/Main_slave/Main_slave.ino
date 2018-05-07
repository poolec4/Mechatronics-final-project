#include <Servo.h>
#include <string.h>
#include "communications.h"

Servo right_servo;
Servo left_servo;
Servo left_spatula_servo;
Servo right_spatula_servo;

const int THIS_SLAVE_ID = 2; // Header ID - Change for each slave

const int rightServoPin = 5;
const int leftServoPin = 4;
const int leftSpatulaServoPin = 6;
const int rightSpatulaServoPin = 7;

int input_ID;
int rMotVal = 90;
int lMotVal = 90;
int spatulaVal = 90;

/* QTI Stuff */
int qtiVal = 0;
int qtiThresh = 400;
bool kill = false;
const int qtiPin = 52;
const int frontQtiPin = 53;

/* LED Stuff */
const int GREEN_LED_PIN = 28;
const int RED_LED_PIN = 24;
const int YELLOW_LED_PIN = 26;

/* XBee Stuff */
int serial_count = 0;
String inpt;
char inpt_char[100];
bool read_serial = true;
char send_array[100];

/* IR Stuff */
const int num_samples = 10;

struct ir_vals{
  volatile byte state_change = false;
  bool IR_sense = false;
  bool freq_sense = false;
  int count = 0;
  float f[num_samples] = {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1};
  float f_median;
  unsigned long t_old = millis();
  unsigned long t_new;
};

struct ir_vals front_ir_vals;
struct ir_vals left_ir_vals;
struct ir_vals right_ir_vals;

const int FRONT_IR_PIN = 2;
const int RIGHT_IR_PIN = 20;
const int LEFT_IR_PIN = 21;
const int IR_SENSE_DELAY = 250;
int count = 0;
/*unsigned long t_change_front;
unsigned long t_change_right;
unsigned long t_change_left;*/
/*volatile byte front_IR_state_change = false;
volatile byte right_IR_state_change = false;
volatile byte left_IR_state_change = false;*/
/*boolean front_IR_sense = false;
boolean front_freq_sense = false;
boolean right_IR_sense = false;
boolean right_freq_sense = false;
boolean left_IR_sense = false;
boolean left_freq_sense = false;*/

unsigned long front_t_old = millis();
unsigned long front_t_new;

/* Operational Modes */
bool autonomous = true; //not using yet
bool manual = false;
bool carry = false;
bool temp_check = false; // In place for autonomous mode
bool navigating = false;
unsigned long no_detect_time = millis();
int no_detect_timeout = 500;
unsigned long random_action_time = millis();
int random_action_timeout = random(1000);
int random_action = 0;
unsigned long temp_time;
/* Switches */
const int SWITCH_PIN = 44;
bool switch_val = false;

void setup() {
  Serial.begin(57600);
  Serial1.begin(57600);
  Serial1.setTimeout(5);

  right_servo.attach(rightServoPin);
  left_servo.attach(leftServoPin);
  left_spatula_servo.attach(leftSpatulaServoPin);
  right_spatula_servo.attach(rightSpatulaServoPin);
  
  pinMode(GREEN_LED_PIN, OUTPUT);
  pinMode(RED_LED_PIN, OUTPUT);
  pinMode(YELLOW_LED_PIN, OUTPUT);

  attachInterrupt(digitalPinToInterrupt(FRONT_IR_PIN), irStateChangeFront, CHANGE);
  //attachInterrupt(digitalPinToInterrupt(RIGHT_IR_PIN), irStateChangeRight, CHANGE);
  //attachInterrupt(digitalPinToInterrupt(LEFT_IR_PIN), irStateChangeLeft, CHANGE);

  digitalWrite(RED_LED_PIN, HIGH);
  delay(500);
  digitalWrite(RED_LED_PIN, LOW);
  digitalWrite(YELLOW_LED_PIN, HIGH);
  delay(500);
  digitalWrite(YELLOW_LED_PIN, LOW);
  digitalWrite(GREEN_LED_PIN, HIGH);
  delay(500);
  digitalWrite(GREEN_LED_PIN, LOW);
}

void loop() {
  /* Drive Autonomously */
  if (autonomous == true){
    butWhosDriving();
  }
  /* Receive values from XBee */
  if (Serial1.available() && autonomous == false) {    
    strcpy(inpt_char, "");
    inpt = Serial1.readStringUntil('\n');
    //Serial1.read(); // get rid of new line
    inpt.toCharArray(inpt_char, inpt.length()+1);
    
    if(is_tag_available(inpt_char, "M")){
      input_ID = parse_string_to_int(inpt_char, "M"); 
    }
    
    /* Stuff to set only if it matches the slave ID */
    
    if(input_ID == THIS_SLAVE_ID && manual == true){
      if(is_tag_available(inpt_char, "R")){
        rMotVal = parse_string_to_int(inpt_char, "R");
      }
      if(is_tag_available(inpt_char, "L")){
        lMotVal = parse_string_to_int(inpt_char, "L");
      }
    }

    /* Stuff to set regardless of slave ID */
    // Get Spatula Value
    if(is_tag_available(inpt_char, "S")){
      spatulaVal = parse_string_to_int(inpt_char, "S");
    }
    // Check is master is in carry mode
    if(is_tag_available(inpt_char, "C")){
      int carry_val = parse_string_to_int(inpt_char, "C");
      carry = bool(carry_val);
      manual = !carry;
      }
    Serial.println(inpt);
  }
  //Check if switch is pressed
  if(digitalRead(SWITCH_PIN) == HIGH){
    if(temp_check == false){
      temp_check = true;
      autonomous = false;
      manual = true;
      strcpy(send_array, "");
      add_int_to_string(send_array, THIS_SLAVE_ID, "M", false);
      add_int_to_string(send_array, manual, "B", true);
      Serial.println(send_array);
      Serial.println("Read Switch");
      Serial1.print(send_array);
    }
  }
  front_ir_vals.t_new = temp_time;
  front_ir_vals = IRSense(front_ir_vals);
  //right_ir_vals = IRSense(right_ir_vals);
  //left_ir_vals = IRSense(left_ir_vals);
  IRLEDs(front_ir_vals.IR_sense, front_ir_vals.freq_sense);

  qtiVal = QTIRead(qtiPin);
  amIDead();
  
  if(kill){
    rMotVal = 90;
    lMotVal = 90;
  }
  
  right_servo.write(rMotVal);
  left_servo.write(lMotVal);
  left_spatula_servo.write(spatulaVal);
  right_spatula_servo.write(-spatulaVal+180);
}

void amIDead(){
  if (qtiVal >= qtiThresh){
    kill = true;
    digitalWrite(RED_LED_PIN, HIGH);
    right_servo.write(90);
    left_servo.write(90);
    Serial.println("I am dead"); 
  }
}

void irStateChangeFront() {
  temp_time = millis();
  // front_ir_vals.t_new = millis();
  front_ir_vals.state_change = true;
  Serial.println(front_ir_vals.state_change);
}

void irStateChangeRight() {
  temp_time = millis();
  right_ir_vals.state_change = true;
}

void irStateChangeLeft() {
  temp_time = millis();
  left_ir_vals.state_change = true;
}

struct ir_vals IRSense(struct ir_vals vals){
  if(vals.state_change == true){
    vals = calculateFreq(vals);
    Serial.print("Median Frequency: ");
    Serial.println(vals.f_median);
  }
  if(vals.f_median > 8.0 && vals.f_median < 12.0){
    vals.IR_sense = true;
    vals.freq_sense = true;
  }
  else if(vals.f_median < 8.0 || vals.f_median > 12.0){
    vals.IR_sense = true;
    vals.freq_sense = false;
  }

  if(millis()-vals.t_new > IR_SENSE_DELAY){ // timeout if signal hasn't been seen for a while
    vals.IR_sense = false;
    vals.freq_sense = false;
  }
  return vals;
}

void IRLEDs(bool IR_sense, bool freq_sense){
  if (IR_sense == true && freq_sense == true){
    digitalWrite(GREEN_LED_PIN, HIGH);
    digitalWrite(YELLOW_LED_PIN, LOW);
  }
  else{
    digitalWrite(GREEN_LED_PIN, LOW);
    digitalWrite(YELLOW_LED_PIN, HIGH);
  }
}

struct ir_vals sortFreq(struct ir_vals vals) {
  for (int i = 1; i < num_samples; i++){
       float key = vals.f[i];
       int j = i-1;
       while (j >= 0 && vals.f[j] > key){
           vals.f[j+1] = vals.f[j];
           j = j-1;
       }
       vals.f[j+1] = key;
   }
  return vals;
}

struct ir_vals calculateFreq(struct ir_vals vals) {
  unsigned long t_delay = vals.t_new - vals.t_old;
  Serial.println(t_delay);
  vals.f[vals.count] = (float)1.0/(t_delay/1000.0*2.0);
  vals.count++;
  unsigned long temp = vals.t_new;
  Serial.println(temp);
  Serial.println(temp_time);
  vals.t_old = temp;

  if(vals.count >= num_samples){  
    vals = sortFreq(vals);
    Serial.println("Frequencies");
    for(int i=0; i<num_samples; i++){
      Serial.print(vals.f[i]);
      Serial.print(' ');
    }
    if(num_samples%2 == 0){  
      vals.f_median = (vals.f[(int)floor((float)num_samples/2.0)]+vals.f[(int)ceil((float)num_samples/2.0)])/2.0;
    }
    else{
      vals.f_median = vals.f[(int)floor((float)num_samples/2.0)];  
    }
    vals.count = 0; 
  }
  return vals;
}

void butWhosDriving(){
  if (front_ir_vals.IR_sense == true && front_ir_vals.freq_sense == true){
    Serial.println("Found IR Signal in Front");
    rMotVal = 180;
    lMotVal = 0;
    no_detect_time = millis();
    navigating = true;
  }
  else if (right_ir_vals.IR_sense == true && right_ir_vals.freq_sense == true){
    Serial.println("Found IR Signal on Right");
    rMotVal = 180;
    lMotVal = 180;
    no_detect_time = millis();
    navigating = true;
  }
  else if (left_ir_vals.IR_sense == true && left_ir_vals.freq_sense == true){
    Serial.println("Found IR Signal on Left");
    rMotVal = 0;
    lMotVal = 0;
    no_detect_time = millis();
    navigating = true;
  }
  else{
    if(navigating == false || millis()-no_detect_time >= no_detect_timeout){
      navigating = false;
      randomSearch();
    }
  }
}

void randomSearch(){
  if(millis()-random_action_time >= random_action_timeout){
    Serial.println("Selecting Random Action");
    random_action_time = millis();
    random_action = random(3);
    Serial.println(random_action);
    switch(random_action){
    case 0: //forward
      Serial.println("Driving Forward");
      rMotVal = 100;
      lMotVal = 80;
      random_action_timeout = random(1000, 2000);
      break;
    case 1: //right
      Serial.println("Turning Right");
      rMotVal = 92;
      lMotVal = 92;
      random_action_timeout = random(750, 1000);
      break;
    case 2: //left
      Serial.println("Turning Left");
      rMotVal = 88;
      lMotVal = 88;
      random_action_timeout = random(750, 1000);
    }    
  }
}

