#include <Servo.h>

Servo right_servo;
Servo left_servo;

// Read Pins (Digital)
int upPin = 7;
int downPin=6;
int rightPin=5;
int leftPin=4;
// Write Pins (Analog)
int rightServoPin = 3;
int leftServoPin = 2;
// Boolean Values
bool up = false;
bool down = false;
bool left = false;
bool right = false;
// Servo Values
float rightServo;
float leftServo;
//Serial Read
int inpt;

void setup() {
  // Pin Setup
  pinMode(upPin, INPUT);
  pinMode(downPin, INPUT);
  pinMode(rightPin, INPUT);
  pinMode(leftPin, INPUT);
  // Servo setup
  right_servo.attach(rightServoPin);
  left_servo.attach(leftServoPin);
  Serial.begin(9600);
  Serial.setTimeout(10);
  right_servo.write(92);
  left_servo.write(92);
}

void loop() {
  // Serial Input
  /* if (Serial.available()){
    getSerialValue();
    writeSerialValue();
  }*/
  // Button Input
  getButtonValues();
  writeButtonValues();
}

void getButtonValues(){
  if (digitalRead(upPin) == HIGH){
    Serial.println("Up - changed");
    up = true;
  }
  else{
    up = false;
  }
  if (digitalRead(downPin) == HIGH){
    Serial.println("Down");
    down = true;
  }
  else{
    down = false;
  }
  if (digitalRead(leftPin) == HIGH){
    Serial.println("Left");
    left = true;
  }
  else{
    left = false;
  }
  if (digitalRead(rightPin) == HIGH){
    Serial.println("Right");
    right = true;
  }
  else{
    right = false;
  }
}

void writeButtonValues(){
  rightServo = 88;
  leftServo = 92;
  if (up == HIGH){
    Serial.println("Up");
    rightServo += 45;
    leftServo += 45;
  }
  else if (down == HIGH){
    Serial.println("Down");
    rightServo -= 45;
    leftServo -= 45;
  }
  if (left == HIGH){
    Serial.println("Left");
    rightServo += 45;
    leftServo -= 45;
  }
  else if (right == HIGH){
    Serial.println("Right");
    leftServo += 45;
    rightServo -= 45;
  }
  right_servo.write(180-rightServo);
  left_servo.write(leftServo);
  Serial.print("Right Motor Value: ");
  Serial.println(180-rightServo);
  Serial.print("Left Motor Value: ");
  Serial.println(leftServo);
}

void getSerialValue(){
  inpt = Serial.parseInt();
}

void writeSerialValue(){
  rightServo = 88;
  leftServo = 92;
  if (inpt == 1){
    rightServo += 45;
    leftServo += 45;
  }
  else if (inpt == 2){
    rightServo -= 45;
    leftServo -= 45;
  }
  else if (inpt == 3){
    rightServo += 45;
    leftServo -= 45;
  }
  else if (inpt == 4){
    leftServo += 45;
    rightServo -= 45;
  }
  right_servo.write(180-rightServo);
  left_servo.write(leftServo);
  Serial.print("Right Motor Value: ");
  Serial.println(180-rightServo);
  Serial.print("Left Motor Value: ");
  Serial.println(leftServo);
}


