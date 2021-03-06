/*  Xbee_slave
   Communication for XBee Slave Node (Experimental)
*/

#include <string.h>
#include "communications.h"
#include <Servo.h>

/* Definitions */
const int SLAVE_ID = 2; // Channel ID
const int THIS_SLAVE_ID = 2; // Header ID - Change for each slave
// LED Pins
const int GREEN_LED_PIN = 28;
const int RED_LED_PIN = 24;
const int YELLOW_LED_PIN = 26;
// Servo Pins
const int leftServoPin = 4;
const int rightServoPin = 5;
const int spatulaPin = 6;
// QTI Pin
const int qtiPin = 52;
// IDs
const int n = 3;
int IDs[n] = {2, 3, 4};

/* QTI Stuff */
int qtiVal = 0;
int qtiThresh = 400;
bool kill = false;

/* Servo Stuff*/
Servo right_servo;
Servo left_servo;
Servo spatula_servo;

int rMotVal;
int lMotVal;
int temprMotVal;
int templMotVal;
int numDropped = 0;
int spatulaVal;

bool carrying = false;

/* IR Stuff */
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

int IR_state = 1;  // 0 = nothing, 1 = ir but wrong freq, 2 = right freq

/* XBee stuff */
String inpt;
char inpt_char[100];
int inpt_ID;
int inpt_IR[3];
unsigned long t_sent;
const unsigned long SEND_DELAY = 500;

char send_array[100];

int SWITCH_PIN = 44;

struct ir_vals{
  bool IR_sense = false;
  bool freq_sense = false;
};

struct ir_vals front_ir_vals;
struct ir_vals left_ir_vals;
struct ir_vals right_ir_vals;


void setup() {
  t_change = millis();
  t_old = millis();
  attachInterrupt(digitalPinToInterrupt(FRONT_IR_PIN), irStateChangeFront, CHANGE);
  attachInterrupt(digitalPinToInterrupt(RIGHT_IR_PIN), irStateChangeLeft, CHANGE);
  attachInterrupt(digitalPinToInterrupt(LEFT_IR_PIN), irStateChangeRight, CHANGE);
  pinMode(GREEN_LED_PIN, OUTPUT);
  pinMode(RED_LED_PIN, OUTPUT);
  pinMode(YELLOW_LED_PIN, OUTPUT);

  right_servo.attach(rightServoPin);
  left_servo.attach(leftServoPin);
  spatula_servo.attach(spatulaPin);
  Serial.begin(9600);
  Serial.setTimeout(10);
  Serial1.begin(9600);
  Serial1.setTimeout(15);
  right_servo.write(90);
  left_servo.write(90);
  Serial1.flush();
  Serial1.print("+++");
  read_char_from_serial1();

  Serial1.print("ATID 1000\r");
  read_char_from_serial1();
  Serial1.print("ATDL 1\r");
  read_char_from_serial1();
  Serial1.print("ATMY " + String(SLAVE_ID) + "\r");
  read_char_from_serial1();

  Serial1.print("ATID\r");
  read_char_from_serial1();
  Serial1.print("ATDH\r");
  read_char_from_serial1();
  Serial1.print("ATDL\r");
  read_char_from_serial1();
  Serial1.print("ATMY\r");
  read_char_from_serial1();  
  Serial1.flush();
  Serial.println("Ready");
}

void loop() {
  front_ir_vals = IRSense(front_ir_vals, front_IR_state_change);
  //left_ir_vals = IRSense(left_ir_vals, left_IR_state_change); 
  //right_ir_vals = IRSense(right_ir_vals, right_IR_state_change);
  Serial.println("Front IR Values");
  Serial.print("IRSense: ");
  Serial.println(front_ir_vals.IR_sense);
  Serial.print("Freq_sense: ");
  Serial.println(front_ir_vals.freq_sense);
  IRLEDs(front_ir_vals.IR_sense, front_ir_vals.freq_sense);
  if(carrying == false){
    qtiVal = QTIRead(qtiPin);
    amIDead();
  }
  foundBox();
  if (Serial1.available() && kill == false) {     
    getInptID();
    if (inpt_ID == THIS_SLAVE_ID){
      getMotorVals();
      writeMotorVals();
      isCarrying();
      moveSpatula();
      printMotorVals();
    }
  }
  // IRTransmit();  
  // IRRec();
  Serial1.flush();
  Serial.flush();
  //delay(100);
}

void getInptID(){ // Gets the ID from the message header
  inpt = Serial1.readStringUntil('\n');
  inpt.toCharArray(inpt_char, inpt.length()+1);
  inpt_ID = parse_string_to_int(inpt_char, "M"); 
}

void getMotorVals(){ // Gets motor values from message
   if(is_tag_available(inpt_char, "R") == true && is_tag_available(inpt_char, "L") == true){
    temprMotVal = parse_string_to_int(inpt_char, "R");
    templMotVal = parse_string_to_int(inpt_char, "L");
    validMes();
   }
}

void isCarrying(){
  if(is_tag_available(inpt_char, "C") == true){
    carrying = parse_string_to_int(inpt_char, "C");
    Serial.println("Carrying Mode On");
  }
}

void moveSpatula(){
  if(is_tag_available(inpt_char, "S") == true){
    spatulaVal = parse_string_to_int(inpt_char, "S");
    switch(spatulaVal){
      case 1:
        spatula_servo.write(95);
        Serial.println("Moving Spatula Up");
        break;
      case 2:
        spatula_servo.write(85);
        Serial.println("Moving Spatula Down");
        break;
      // default:
      // spatula_servo.write(90);
      // break;
    }
  }
}

void writeMotorVals(){
  right_servo.write(180-rMotVal);
  left_servo.write(lMotVal);
}

void printMotorVals(){
  Serial.println(inpt);
  Serial.print("Right Motor Value: ");
  Serial.println(rMotVal);
  Serial.print("Left Motor Value: ");
  Serial.println(lMotVal);
}

void validMes(){
  if(temprMotVal < 20 || templMotVal < 20 || temprMotVal > 180 || templMotVal > 180){
    numDropped = numDropped + 1;
  }
  else{
    numDropped = 0;
    rMotVal = temprMotVal;
    lMotVal = templMotVal;
  }
  if (numDropped >= 5){
    rMotVal = 90;
    lMotVal = 90; 
  }
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

struct ir_vals  IRSense(struct ir_vals bools, bool IR_state_change){
  if(IR_state_change == true){
    calculateFreq();
    Serial.println(f_median);
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
    Serial.println("TIMEOUT");
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

/*void IRTransmit(){
  if (IR_sense == false && freq_sense == false) { // no signal
    IR_state = 0;
    digitalWrite(GREEN_LED_PIN, LOW);
    digitalWrite(RED_LED_PIN, LOW);
  }
  if (IR_sense == true && freq_sense == false) { // decoy signal
    IR_state = 1;
    digitalWrite(GREEN_LED_PIN, LOW);
    digitalWrite(RED_LED_PIN, HIGH);
  }
  if (IR_sense == true && freq_sense == true) { // true signal
    IR_state = 2;
    digitalWrite(GREEN_LED_PIN, HIGH);
    digitalWrite(RED_LED_PIN, LOW);
  }

  if(millis()-t_sent > SEND_DELAY){
    strcpy(send_array, "");
  
    add_int_to_string(send_array, THIS_SLAVE_ID, "M", false);
    add_int_to_string(send_array, IR_state, "I", true);
  
    Serial1.print(send_array);
    t_sent = millis();
  }
}*/

void IRRec(){
  if (Serial1.available()) {     
    inpt = Serial1.readStringUntil('\n');
    inpt.toCharArray(inpt_char, inpt.length()+1);
    inpt_ID = parse_string_to_int(inpt_char, "M");
    
    inpt_IR[inpt_ID - IDs[0]] = parse_string_to_int(inpt_char, "I");

    if(inpt_ID != THIS_SLAVE_ID){
      if (inpt_IR[0] == 0 && inpt_IR[1] == 0 && inpt_IR[2] == 0){ // someone else found nothing
        digitalWrite(YELLOW_LED_PIN, LOW);
      }
      
      if (inpt_IR[0] == 1 && inpt_IR[1] == 1 && inpt_IR[2] == 1){ // someone else found the dummy
        digitalWrite(YELLOW_LED_PIN, LOW);
      }
      
      if (inpt_IR[0] == 2 || inpt_IR[1] == 2 || inpt_IR[2] == 2){ // someone else found the real signal
        digitalWrite(YELLOW_LED_PIN, HIGH);
      }
    }
  }
}

void irStateChangeFront() {
  t_change = millis();
  //Serial.print("t_change: ");
  //Serial.println(t_change);
  front_IR_state_change = true;
}

void irStateChangeLeft(){
  t_change = millis();
  left_IR_state_change = true;
}

void irStateChangeRight(){
  t_change = millis();
  right_IR_state_change = true; 
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

void foundBox(){
  if (digitalRead(SWITCH_PIN) == HIGH){
    strcpy(send_array, "");
    add_int_to_string(send_array, THIS_SLAVE_ID, "M", false);
    add_int_to_string(send_array, 1, "B", true);
    Serial1.println(send_array); 
    Serial.println("Found Box");
  }
}

