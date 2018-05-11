#include <Servo.h>
#include <string.h>
#include "communications.h"

Servo right_servo;
Servo left_servo;
Servo left_spatula_servo;
Servo right_spatula_servo;

const int THIS_SLAVE_ID = 4; // Header ID - Change for each slave

const int rightServoPin = 5;
const int leftServoPin = 4;
const int leftSpatulaServoPin = 6;
const int rightSpatulaServoPin = 7;

int input_ID;
int rMotVal = 90;
int lMotVal = 90;
int spatulaVal = 85;

/* QTI Stuff */
int rear_qti_val = 0;
int front_qti_val = 0;
int rear_qti_thresh = 400;
int front_qti_thresh = 200;
bool kill = false;
const int REAR_QTI_PIN = 52;
const int FRONT_QTI_PIN = 51;


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
struct ir_vals{
  bool IR_sense = false;
  bool freq_sense = false;
};

struct ir_vals front_ir_vals;
struct ir_vals left_ir_vals;
struct ir_vals right_ir_vals;

const int FRONT_IR_PIN = 2;
//const int RIGHT_IR_PIN = 2;
//const int LEFT_IR_PIN = 3;
const int num_samples = 5;
const int IR_SENSE_DELAY = 250;
int count_front = 0;
int count_right = 0;
int count_left = 0;
unsigned long t_change_front, t_old_front, t_delay_front;
unsigned long t_change_right, t_old_right, t_delay_right;
unsigned long t_change_left, t_old_left, t_delay_left;
float f_front[num_samples], f_median_front;
float f_right[num_samples], f_median_right;
float f_left[num_samples], f_median_left;
volatile byte front_IR_state_change = false;
volatile byte right_IR_state_change = false;
volatile byte left_IR_state_change = false;

/* Depreciated 
boolean front_IR_sense = true;
boolean front_freq_sense = true;
boolean right_IR_sense = true;
boolean right_freq_sense = true;
boolean left_IR_sense = true;
boolean left_freq_sense = true; /*

/* Operational Modes */
bool autonomous = true; //not using yet
bool manual = false;
bool carry = false;
bool init_switch_hit = false; // checks if switch was just hit
bool navigating = false;
unsigned long no_detect_time = millis();
int no_detect_timeout = 250;
unsigned long random_action_time = millis();
int random_action_timeout = random(1000);
int random_action = 0;
/* Switches */
const int SWITCH_PIN = 43;
bool switch_val = false;
unsigned long switch_send_timeout = 500;
unsigned long last_switch_send = millis();

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

  pinMode(REAR_QTI_PIN, INPUT);
  pinMode(FRONT_QTI_PIN, INPUT);
  
  attachInterrupt(digitalPinToInterrupt(FRONT_IR_PIN), irStateChangeFront, CHANGE);
  //attachInterrupt(digitalPinToInterrupt(RIGHT_IR_PIN), irStateChangeRight, CHANGE);
  //attachInterrupt(digitalPinToInterrupt(LEFT_IR_PIN), irStateChangeLeft, CHANGE);

  switch(THIS_SLAVE_ID){
    case 2:
      rear_qti_thresh = 400;
      front_qti_thresh = 200;
      break;
    case 3:
      rear_qti_thresh = 600;
      front_qti_thresh = 200;
      break;
    case 4:
      rear_qti_thresh = 400;
      front_qti_thresh = 200;
      break;
  }
}

void loop() {
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
    
    if(input_ID == THIS_SLAVE_ID && (manual == true || carry == true)){
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
    // Check master's mode
    if(is_tag_available(inpt_char, "C") && is_tag_available(inpt_char, "A")){
      int carry_val = parse_string_to_int(inpt_char, "C");
      int auto_val = parse_string_to_int(inpt_char, "A");
      carry = bool(carry_val);
      autonomous = bool(auto_val);
      if(!autonomous && !carry){
        manual = true;     
      }
      else{
        manual = false;
      }
    }
    if(carry){
      digitalWrite(YELLOW_LED_PIN, HIGH);
    }
    else{
      digitalWrite(YELLOW_LED_PIN, LOW);
    }
    Serial.println(inpt);
  }
  //Check switch state
  if(digitalRead(SWITCH_PIN) == HIGH){
    if(init_switch_hit == false){
      init_switch_hit = true;
      autonomous = false;
      manual = true;
      rMotVal = 91;
      lMotVal = 89;
    }
    if(millis()-last_switch_send >= switch_send_timeout){
      last_switch_send = millis();
      strcpy(send_array, "");
      add_int_to_string(send_array, THIS_SLAVE_ID, "M", false);
      add_int_to_string(send_array, init_switch_hit, "B", true);
      Serial.println(send_array);
      Serial.println("Read Switch");
      Serial1.print(send_array);
    }
  }
  /*else if (digitalRead(SWITCH_PIN) == LOW){ //assumes switch contact lost in carry mode
    if(init_switch_hit == true){ 
      init_switch_hit = false;
      carry = false;
      manual = true;
      rMotVal = 91;
      lMotVal = 89;
    }
    if(millis()-last_switch_send >= switch_send_timeout){
      last_switch_send = millis();
      strcpy(send_array, "");
      add_int_to_string(send_array, THIS_SLAVE_ID, "M", false);
      add_int_to_string(send_array, init_switch_hit, "B", true);
      Serial.println(send_array);
      Serial.println("Switch Lost");
      Serial1.print(send_array);
    }
  }*/
  front_ir_vals = IRSenseFront(front_ir_vals, front_IR_state_change);
  //right_ir_vals = IRSenseRight(right_ir_vals, right_IR_state_change);
  //left_ir_vals = IRSenseLeft(left_ir_vals, left_IR_state_change);
  //Serial.println("State Change Values");
  // Serial.println(front_IR_state_change);
  //Serial.println(right_IR_state_change);
  //Serial.println(left_IR_state_change);
  IRLEDs(front_ir_vals.IR_sense, front_ir_vals.freq_sense);

  rear_qti_val = QTIRead(REAR_QTI_PIN);
  front_qti_val = QTIRead(FRONT_QTI_PIN);
//  Serial.println("QTI");
//  Serial.println(rear_qti_val);
//  Serial.println(front_qti_val);
  amIDead();
  
  if(kill && !carry){
    rMotVal = 90;
    lMotVal = 90;
  }

  Serial.println(rMotVal);
  Serial.println(lMotVal);
  right_servo.write(rMotVal);
  left_servo.write(lMotVal);
  left_spatula_servo.write(spatulaVal);
  right_spatula_servo.write(-spatulaVal+180);
}

void amIDead(){
  if (rear_qti_val >= rear_qti_thresh){
    kill = true;
    digitalWrite(RED_LED_PIN, HIGH);
    right_servo.write(90);
    left_servo.write(90);
    Serial.println("I am dead"); 
  }
}

/* Interrupts */
void irStateChangeFront() {
  if(!front_ir_vals.IR_sense){
    random_action_time = millis();
    right_servo.write(90);
    left_servo.write(90);
  }
  t_change_front = millis();
  front_IR_state_change = true;
}

void irStateChangeRight() {
  if(!right_ir_vals.IR_sense){
    random_action_time = millis();
    right_servo.write(90);
    left_servo.write(90);
  }
  Serial.println("Right Interrupt");
  t_change_right = millis();
  right_IR_state_change = true;
}

void irStateChangeLeft() {
  if(!left_ir_vals.IR_sense){
    random_action_time = millis();
    right_servo.write(90);
    left_servo.write(90);
  }
  Serial.println("Left Interrupt");
  t_change_left = millis();
  left_IR_state_change = true;
}

/* IR Sense */
struct ir_vals  IRSenseFront(struct ir_vals bools, bool IR_state_change){
  if(IR_state_change == true){
    // calculateFreqFront();
    t_delay_front = t_change_front-t_old_front;
    f_median_front = (float)1.0/(t_delay_front/1000.0*2.0);
    t_old_front = t_change_front;
    front_IR_state_change = false;
    Serial.println(f_median_front);
  }
  if(f_median_front > 8.0 && f_median_front < 12.0){
    bools.IR_sense = true;
    bools.freq_sense = true;
    //Serial.println("True Signal Detected in Front");
  }
  else if(f_median_front == 0.0){
    bools.IR_sense = false;
    bools.freq_sense = false;
   // Serial.println("Signal Not Detected in Front");
  }
  else if(f_median_front < 8.0 || f_median_front > 12.0){
    bools.IR_sense = true;
    bools.freq_sense = false;
    //Serial.println("False Signal Detected in Front");
  }

  if(millis()-t_change_front > IR_SENSE_DELAY){ // timeout if signal hasn't been seen for a while
    bools.IR_sense = false;
    bools.freq_sense = false;
    //Serial.println("TIMEOUT");
  }
  return bools;
}

struct ir_vals  IRSenseRight(struct ir_vals bools, bool IR_state_change){
  if(IR_state_change == true){
    //Serial.println("Calculating Frequency");
    calculateFreqRight();
    //Serial.println(f_median);
  }
  if(f_median_right > 8.0 && f_median_right < 12.0){
    bools.IR_sense = true;
    bools.freq_sense = true;
    //Serial.println("True Signal Detected on Right");
  }
  else if(f_median_right == 0.0){
    bools.IR_sense = false;
    bools.freq_sense = false;
    //Serial.println("Signal Not Detected on Right");
  }
  else if(f_median_right < 8.0 || f_median_right > 12.0){
    bools.IR_sense = true;
    bools.freq_sense = false;
    //Serial.println("False Signal Detected on Right");
    //Serial.println(f_median_right);
  }

  if(millis()-t_change_right > IR_SENSE_DELAY){ // timeout if signal hasn't been seen for a while
    bools.IR_sense = false;
    bools.freq_sense = false;
  }
  return bools;
}
struct ir_vals  IRSenseLeft(struct ir_vals bools, bool IR_state_change){
  if(IR_state_change == true){
    calculateFreqLeft();
    //Serial.println(f_median);
  }
  if(f_median_left > 8.0 && f_median_left < 12.0){
    bools.IR_sense = true;
    bools.freq_sense = true;
    //Serial.println("True Signal Detected on Left");
  }
  else if(f_median_left == 0.0){
    bools.IR_sense = false;
    bools.freq_sense = false;
    //Serial.println("Signal Not Detected on Left");
  }
  else if(f_median_left < 8.0 || f_median_left > 12.0){
    bools.IR_sense = true;
    bools.freq_sense = false;
    //Serial.println("False Signal Detected on Left");
    //Serial.println(f_median_left);
  }

  if(millis()-t_change_left > IR_SENSE_DELAY){ // timeout if signal hasn't been seen for a while
    bools.IR_sense = false;
    bools.freq_sense = false;
    //Serial.println("TIMEOUT");
  }
  return bools;
}

void IRLEDs(bool IR_sense, bool freq_sense){ //Only call for front IR sensor
  if (IR_sense == true && freq_sense == true){
    digitalWrite(GREEN_LED_PIN, HIGH);
  }
  else{
    digitalWrite(GREEN_LED_PIN, LOW);
  }
}
/* Sort Frequencies */
void sortFreqFront() {
  for (int i = 1; i < num_samples; i++){
       float key = f_front[i];
       int j = i-1;
       while (j >= 0 && f_front[j] > key){
           f_front[j+1] = f_front[j];
           j = j-1;
       }
       f_front[j+1] = key;
   }
}

void sortFreqRight() {
  for (int i = 1; i < num_samples; i++){
       float key = f_right[i];
       int j = i-1;
       while (j >= 0 && f_right[j] > key){
           f_right[j+1] = f_right[j];
           j = j-1;
       }
       f_right[j+1] = key;
   }
}

void sortFreqLeft() {
  for (int i = 1; i < num_samples; i++){
       float key = f_left[i];
       int j = i-1;
       while (j >= 0 && f_left[j] > key){
           f_left[j+1] = f_left[j];
           j = j-1;
       }
       f_left[j+1] = key;
   }
}

/* Calculate Freq */
void calculateFreqFront() {
  t_delay_front = t_change_front-t_old_front;
  f_front[count_front] = (float)1.0/(t_delay_front/1000.0*2.0);
  count_front++;
  t_old_front = t_change_front;
  front_IR_state_change = false;

  if(count_front >= num_samples){  
    sortFreqFront();
    Serial.println("Array");
    for(int i=0; i<num_samples; i++){
      Serial.println(f_front[i]);
    }
    Serial.println("-----------");
    if(num_samples%2 == 0){  
      f_median_front = (f_front[(int)floor((float)num_samples/2.0)]+f_front[(int)ceil((float)num_samples/2.0)])/2.0;
    }
    else{
      f_median_front = f_front[(int)floor((float)num_samples/2.0)];  
    }
    count_front = 0; 
    for (int i=0; i < num_samples; i++){
      f_front[i] = 0;
    }
  }
}

void calculateFreqRight() {
  t_delay_right = t_change_right-t_old_right;
  f_right[count_right] = (float)1.0/(t_delay_right/1000.0*2.0);
  count_right++;
  t_old_right = t_change_right;
  right_IR_state_change = false;

  if(count_right >= num_samples){  
    sortFreqRight();
    if(num_samples%2 == 0){  
      f_median_right = (f_right[(int)floor((float)num_samples/2.0)]+f_right[(int)ceil((float)num_samples/2.0)])/2.0;
    }
    else{
      f_median_right = f_right[(int)floor((float)num_samples/2.0)];  
    }
    count_right = 0; 
  }
}

void calculateFreqLeft() {
  t_delay_left = t_change_left-t_old_left;
  f_left[count_left] = (float)1.0/(t_delay_left/1000.0*2.0);
  count_left++;
  t_old_left = t_change_left;
  left_IR_state_change = false;

  if(count_left >= num_samples){  
    sortFreqLeft();
    
    if(num_samples%2 == 0){  
      f_median_left = (f_left[(int)floor((float)num_samples/2.0)]+f_left[(int)ceil((float)num_samples/2.0)])/2.0;
    }
    else{
      f_median_left = f_left[(int)floor((float)num_samples/2.0)];  
    }
    count_left = 0; 
  }
}

void butWhosDriving(){
  //Serial.println(rear_qti_val);
  //Serial.println(front_qti_val);
  if (front_qti_val > front_qti_thresh && kill==false){ //Turn around if edge detected
    //Serial.println("Hit Boundary");
    // Drive backwards
    rMotVal = 0;
    lMotVal = 180;
    right_servo.write(rMotVal);
    left_servo.write(lMotVal);
    random_action_time = millis();
    random_action_timeout = 1500;
    //delay(1500);
    // Turn around
    //right_servo.write(100);
    //left_servo.write(100);
    //delay(1000);    
  }
  else if (front_ir_vals.IR_sense == true && front_ir_vals.freq_sense == true){
    //Serial.println("Found IR Signal in Front");
    rMotVal = 180;
    lMotVal = 0;
    no_detect_time = millis();
    random_action_time = millis();
    random_action_timeout = 500;
    random_action = 0;
    navigating = true;
  }
  else if (right_ir_vals.IR_sense == true && right_ir_vals.freq_sense == true){
    //Serial.println("Found IR Signal on Right");
    rMotVal = 180;
    lMotVal = 180;
    no_detect_time = millis();
    navigating = true;
  }
  else if (left_ir_vals.IR_sense == true && left_ir_vals.freq_sense == true){
    //Serial.println("Found IR Signal on Left");
    rMotVal = 0;
    lMotVal = 0;
    no_detect_time = millis();
    navigating = true;
  }
  else{
    if(navigating == false || millis()-no_detect_time >= no_detect_timeout){
      // Serial.println("No detect timeout");
      navigating = false;
      randomSearch();
    }
  }
}

void randomSearch(){
  if(millis()-random_action_time >= random_action_timeout){
    //Serial.println("Selecting Random Action");
    random_action_time = millis();
    //random_action = random(3);
    random_action = !random_action;
    //Serial.println(random_action);
    switch(random_action){
    case 0: //forward
      //Serial.println("Driving Forward");
      rMotVal = 120;
      lMotVal = 60;
      random_action_timeout = random(1000, 2000);
      break;
    case 1: //turn
      int turn_direction = random(2);
      if(turn_direction == 0){
        //Serial.println("Turning Right");
        rMotVal = 93;
        lMotVal = 93;
        random_action_timeout = random(2000, 3000);
      }
      else{
        //Serial.println("Turning Left");
        rMotVal = 87;
        lMotVal = 87;
        random_action_timeout = random(2000, 3000);
      }
      break;
    /* case 2: //left
     Serial.println("Turning Left");
     rMotVal = 88;
     lMotVal = 88;
     random_action_timeout = random(1000, 2000); */
    }    
  }
}

