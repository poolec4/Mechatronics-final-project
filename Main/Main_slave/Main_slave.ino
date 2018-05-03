#include <Servo.h>
#include <string.h>
#include "communications.h"

Servo right_servo;
Servo left_servo;
Servo spatula_servo;

const int THIS_SLAVE_ID = 3; // Header ID - Change for each slave

const int rightServoPin = 5;
const int leftServoPin = 4;
const int spatulaServoPin = 6;

int input_ID;
int rMotVal = 90;
int lMotVal = 90;
int spatulaVal = 90;

/* QTI Stuff */
int qtiVal = 0;
int qtiThresh = 400;
bool kill = false;
const int qtiPin = 52;

/* LED Stuff */
const int GREEN_LED_PIN = 28;
const int RED_LED_PIN = 24;
const int YELLOW_LED_PIN = 26;

/* XBee Stuff */
int serial_count = 0;
String inpt;
char inpt_char[100];
bool read_serial = true;

/* IR Stuff */
struct ir_vals{
  bool IR_sense = false;
  bool freq_sense = false;
};

struct ir_vals front_ir_vals;
struct ir_vals left_ir_vals;
struct ir_vals right_ir_vals;

const int FRONT_IR_PIN = 2;
const int RIGHT_IR_PIN = 20;
const int LEFT_IR_PIN = 21;
const int num_samples = 10;
const int IR_SENSE_DELAY = 250;
int count = 0;
unsigned long t_change, t_old, t_delay;
float f[num_samples], f_median;
volatile byte front_IR_state_change = false;
volatile byte right_IR_state_change = false;
volatile byte left_IR_state_change = false;
boolean front_IR_sense = true;
boolean front_freq_sense = true;
boolean right_IR_sense = true;
boolean right_freq_sense = true;
boolean left_IR_sense = true;
boolean left_freq_sense = true;

void setup() {
  Serial.begin(57600);
  Serial1.begin(57600);
  Serial1.setTimeout(5);

  right_servo.attach(rightServoPin);
  left_servo.attach(leftServoPin);
  spatula_servo.attach(spatulaServoPin);
  
  pinMode(GREEN_LED_PIN, OUTPUT);
  pinMode(RED_LED_PIN, OUTPUT);
  pinMode(YELLOW_LED_PIN, OUTPUT);

  attachInterrupt(digitalPinToInterrupt(FRONT_IR_PIN), irStateChangeFront, CHANGE);
}

void loop() {
  
  /* Receive values from XBee */
  if (Serial1.available()) {    
    strcpy(inpt_char, "");
    inpt = Serial1.readStringUntil('\n');
    //Serial1.read(); // get rid of new line
    inpt.toCharArray(inpt_char, inpt.length()+1);
    
    if(is_tag_available(inpt_char, "M")){
      input_ID = parse_string_to_int(inpt_char, "M"); 
    }
    
    // Stuff to set only if it matches the slave ID
    
    if(input_ID == THIS_SLAVE_ID){
      if(is_tag_available(inpt_char, "R")){
        rMotVal = parse_string_to_int(inpt_char, "R");
      }
      if(is_tag_available(inpt_char, "L")){
        lMotVal = parse_string_to_int(inpt_char, "L");
      }
    }

    // Stuff to set regardless of slave ID
    
    if(is_tag_available(inpt_char, "S")){
      spatulaVal = parse_string_to_int(inpt_char, "S");
    }
    
    Serial.println(inpt);
  }

  front_ir_vals = IRSense(front_ir_vals, front_IR_state_change);
  IRLEDs(front_ir_vals.IR_sense, front_ir_vals.freq_sense);

  qtiVal = QTIRead(qtiPin);
  amIDead();
  
  if(kill){
    rMotVal = 90;
    lMotVal = 90;
    spatulaVal = 90;
  }
  
  right_servo.write(rMotVal);
  left_servo.write(lMotVal);
  spatula_servo.write(spatulaVal);
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
  t_change = millis();
  front_IR_state_change = true;
}

struct ir_vals  IRSense(struct ir_vals bools, bool IR_state_change){
  if(IR_state_change == true){
    calculateFreq();
    //Serial.println(f_median);
  }
  if(f_median > 8.0 && f_median < 12.0){
    bools.IR_sense = true;
    bools.freq_sense = true;
    Serial.println("True Signal Detected");
  }
  else if(f_median < 8.0 || f_median > 12.0){
    bools.IR_sense = true;
    bools.freq_sense = false;
  }

  if(millis()-t_change > IR_SENSE_DELAY){ // timeout if signal hasn't been seen for a while
    bools.IR_sense = false;
    bools.freq_sense = false;
    //Serial.println("TIMEOUT");
  }
  return bools;
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

void sortFreq() {
  for (int i = 1; i < num_samples; i++){
       float key = f[i];
       int j = i-1;
       while (j >= 0 && f[j] > key){
           f[j+1] = f[j];
           j = j-1;
       }
       f[j+1] = key;
   }
}

void calculateFreq() {
  t_delay = t_change-t_old;
  f[count] = (float)1.0/(t_delay/1000.0*2.0);
  count++;
  t_old = t_change;
  front_IR_state_change = false;

  if(count >= num_samples){  
    sortFreq();
    
    if(num_samples%2 == 0){  
      f_median = (f[(int)floor((float)num_samples/2.0)]+f[(int)ceil((float)num_samples/2.0)])/2.0;
    }
    else{
      f_median = f[(int)floor((float)num_samples/2.0)];  
    }
    count = 0; 
  }
}
