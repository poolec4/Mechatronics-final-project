/*
 * Integraged Master Code for Milestone 3 (Experimental)
*/
#include <string.h>
#include "communications.h"

// Button Read Pins (Digital)
int upPin = 9;
int downPin = 8;
int rightPin = 6;
int leftPin = 7;

// Cycle Slave Stuff
const int cyclePin = 10;
int slaveIndex = 0;
bool buttonState;
bool lastButtonState = LOW;
unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 50;    // the debounce time; increase if the output flickers
const int n = 3;
int IDs[n] = {2, 3, 4};
int SLAVE_ID = IDs[slaveIndex];

//LED Pins
const int redPin = 51;
const int yellowPin = 50;
const int greenPin = 52;
const int detectPin = 53;

// Button State Boolean Values
bool up = false;
bool down = false;
bool left = false;
bool right = false;

// Servo Values
int rightServo;
int leftServo;

//XBee Communication
String inpt;
char inpt_char[100];
int inpt_ID;
int inpt_IR[3];
String out_ID;
char out_ID_chr[100];
char send_array[100];

void setup() {
  Serial.begin(9600);
  Serial1.begin(57600);
  Serial1.setTimeout(10);
  Serial1.flush();
  Serial1.print("+++");
  read_char_from_serial1();

  Serial1.print("ATID 1000\r"); //Network
  read_char_from_serial1();
  Serial1.print("ATDL 2\r"); //Other ID
  read_char_from_serial1();
  Serial1.print("ATMY 1\r"); //My ID
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
  
  pinMode(redPin, OUTPUT);
  pinMode(yellowPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(detectPin, OUTPUT);
  robotLEDs();
}

void loop() {
  getButtonValues();
  calcMotorValues();
  sendMotorValues();
  cycleSlave();
}

void getButtonValues() {
  if (digitalRead(upPin) == HIGH) {
    Serial.println("Up");
    up = true;
  }
  else {
    up = false;
  }
  if (digitalRead(downPin) == HIGH) {
    Serial.println("Down");
    down = true;
  }
  else {
    down = false;
  }
  if (digitalRead(leftPin) == HIGH) {
    Serial.println("Left");
    left = true;
  }
  else {
    left = false;
  }
  if (digitalRead(rightPin) == HIGH) {
    Serial.println("Right");
    right = true;
  }
  else {
    right = false;
  }
}

void calcMotorValues() {
  rightServo = 90;
  leftServo = 90;
  if (up == HIGH) {
    rightServo += 60;
    leftServo += 60;
  }
  else if (down == HIGH) {
    rightServo -= 40;
    leftServo -= 40;
  }
  if (left == HIGH) {
    rightServo += 20;
    leftServo -= 20;
  }
  else if (right == HIGH) {
    leftServo += 20;
    rightServo -= 20;
  }
}

void sendMotorValues() {
  strcpy(send_array, "");
  add_int_to_string(send_array, SLAVE_ID, "M", false);
  add_int_to_string(send_array, leftServo, "L", false);
  add_int_to_string(send_array, rightServo, "R", true);
  Serial.println(send_array);
  Serial1.println(send_array);
}

void cycleSlave(){
  buttonState = digitalRead(cyclePin);
  if (buttonState != lastButtonState && (millis()-lastDebounceTime) > debounceDelay){ // Update Slave
    lastDebounceTime = millis();
    if (buttonState == HIGH){
      if (slaveIndex < 2){
       slaveIndex++;
      }
      else{
        slaveIndex = 0;
      }
      SLAVE_ID = IDs[slaveIndex];
      robotLEDs();
      Serial.print("Slave ID: ");
      Serial.println(SLAVE_ID);
    }
  }
  lastButtonState = buttonState;
}

void robotLEDs(){
  // Write all LEDs Low
  digitalWrite(redPin, LOW);
  digitalWrite(yellowPin, LOW);
  digitalWrite(greenPin, LOW);
  switch(SLAVE_ID){
    case 2:
      digitalWrite(greenPin, HIGH);
      Serial.println("Green");
      break;
    case 3:
      digitalWrite(yellowPin, HIGH);
      Serial.println("Yellow");
      break;
    case 4:
      Serial.println("Red");
      digitalWrite(redPin, HIGH);
      break;
  }
}

void getIRData(){
  if (Serial1.available() > 0) {     // XBee
    inpt = Serial1.readStringUntil('\n');
    inpt.toCharArray(inpt_char, inpt.length() + 1);
    inpt_ID = parse_string_to_int(inpt_char, "M");
    inpt_IR[inpt_ID - IDs[0]] = parse_string_to_int(inpt_char, "I");
    Serial.println(inpt);
    if (inpt_IR[0] == 2 || inpt_IR[1] == 2 || inpt_IR[2] == 2) {
//      String disp = "   Slave " + String(inpt_ID) + " found correct signal";
//      Serial.println(disp);
      digitalWrite(detectPin, HIGH);
    }
    else { 
//      String disp = "   Slave " + String(inpt_ID) + " found nothing";
//      Serial.println(disp);
      digitalWrite(detectPin, LOW);
    }
    Serial1.println(inpt);
    Serial.flush();
    Serial1.flush();
  }
}

