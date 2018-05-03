#include <Servo.h>
#include <string.h>
#include "communications.h"

Servo right_servo;
Servo left_servo;

const int THIS_SLAVE_ID = 3; // Header ID - Change for each slave

const int rightServoPin = 5;
const int leftServoPin = 4;

int input_ID;
int rMotVal = 90;
int lMotVal = 90;
int spatulaVal = 90;

int serial_count = 0;
String inpt;
char inpt_char[100];
bool read_serial = true;

void setup() {
  Serial.begin(57600);
  Serial1.begin(57600);
  Serial1.setTimeout(5);

  right_servo.attach(rightServoPin);
  left_servo.attach(leftServoPin);

  pinMode(24, OUTPUT);
}

void loop() {
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
    
    if(is_tag_available(inpt_char, "S")){
      spatulaVal = parse_string_to_int(inpt_char, "S");
    }
    // but we want the spatula state the same for all, don't care about ID

    
    Serial.println(inpt);
  }
  
  right_servo.write(rMotVal);
  left_servo.write(lMotVal);
}
