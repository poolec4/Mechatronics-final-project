#include <string.h>
#include "communications.h"
#include <Servo.h>

const int leftServoPin = 4;
const int rightServoPin = 5;

const int my_ID = 4; // Change for each slave
const int SLAVE_ID = 2;

int inpt_ID;
int rMotVal;
int lMotVal;
int temprMotVal;
int templMotVal;
int numDropped = 0;

String inpt;
char inpt_char[100];

const int qtiPin = 52;

int qtiVal = 0;
int qtiThresh = 400;
bool kill = false;

const int LEDPin = 24;

Servo right_servo;
Servo left_servo;

void setup() {
  right_servo.attach(rightServoPin);
  left_servo.attach(leftServoPin);
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
  pinMode(LEDPin, OUTPUT);
  digitalWrite(LEDPin, LOW);
}

void loop() {
  qtiVal = QTI_read(qtiPin);
  Serial.println(qtiVal);
  if (qtiVal >= qtiThresh){
    kill = true;
    digitalWrite(LEDPin, HIGH);
    right_servo.write(90);
    left_servo.write(90);
    Serial.println("Dead");
  }
  // Get Signal from XBee
  if (Serial1.available() && kill == false) {     
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
   temprMotVal = parse_string_to_int(inpt_char, "R");
   templMotVal = parse_string_to_int(inpt_char, "L");
   validMes();
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

