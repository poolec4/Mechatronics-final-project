#include "communications.h"

// Read Pins (Digital)
int upPin = 9;
int downPin = 8;
int rightPin = 6;
int leftPin = 7;

// Button State Boolean Values
bool up = false;
bool down = false;
bool left = false;
bool right = false;

// Servo Values
int rightServo;
int leftServo;

// XBee values
const int n = 3;
int IDs[n] = {2, 3, 4};
char out_ID_chr[100];
int SLAVE_ID = 4;

char send_array[100];

void setup() {
  Serial.begin(9600);
  Serial1.begin(9600);
  Serial1.setTimeout(10);

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
}

void loop() {
  getButtonValues();
  calcMotorValues();
  sendMotorValues();
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
  rightServo = 88;
  leftServo = 92;
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

//  Serial.println("Right Motor Value: ");
//  Serial.print(rightServo);
//  Serial.println("Left Motor Value: ");
//  Serial.print(leftServo);
}

void sendMotorValues() {
  strcpy(send_array, "");
  add_int_to_string(send_array, SLAVE_ID, "M", false);
  add_int_to_string(send_array, leftServo, "L", false);
  add_int_to_string(send_array, rightServo, "R", true);
  Serial.println(send_array);
  Serial1.println(send_array);
}

