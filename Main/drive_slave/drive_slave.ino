#include <string.h>
#include "communications.h"
#include <Servo.h>

const int leftServoPin = 2;
const int rightServoPin = 3;

const int my_ID = 4; // Change for each slave
const int SLAVE_ID = 2;

int inpt_ID;
int rMotVal;
int lMotVal;

String inpt;
char inpt_char[100];

const int qtiPin 13;
int qtiVal;

Servo right_servo;
Servo left_servo;

void setup() {
  right_servo.attach(rightServoPin);
  left_servo.attach(leftServoPin);
  Serial.begin(9600);
  Serial.setTimeout(10);
  Serial1.begin(9600);
  Serial1.setTimeout(15);
  right_servo.write(92);
  left_servo.write(92);

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
}

void loop() {
  QTI_read(qtiPin);
  // Get Signal from XBee
  if (Serial1.available()) {     
    getInptID();
    if (inpt_ID == my_ID){
      getMotorVals();
      writeMotorVals();
      printMotorVals();
    }
  }
}

void getInptID(){ // Gets the ID from the message header
  inpt = Serial1.readStringUntil('\n');
  inpt.toCharArray(inpt_char, inpt.length()+1);
  inpt_ID = parse_string_to_int(inpt_char, "M"); 
}

void getMotorVals(){ // Gets motor values from message
   rMotVal = parse_string_to_int(inpt_char, "R");
   lMotVal = parse_string_to_int(inpt_char, "L");
   validMes();
}

void writeMotorVals(){
  right_servo.write(180-rMotVal);
  left_servo.write(lMotVal);
}

void printMotorVals(){
  Serial.println(inpt);
  Serial.print("Right Motor Value: ");
  Serial.println(180-rMotVal);
  Serial.print("Left Motor Value: ");
  Serial.println(lMotVal);
}

void validMes(){
  if(rMotVal < 20 || lMotVal < 20 || rMotVal > 180 || lMotVal > 180){
    rMotVal = 88;
    lMotVal = 92;
  }
}

