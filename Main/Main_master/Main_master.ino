#include "communications.h"
int IDs[3] = {2, 3, 4};
int ID_LEDs[3] = {51, 52, 50};
int slave_index = 0;
int SLAVE_ID = IDs[slave_index];

const int upPin = 8;
const int downPin = 9;
const int rightPin = 6;
const int leftPin = 7;
const int cyclePin = 10;
const int spatulaPin = A0;

unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 250;    // the debounce time; increase if the output flickers

int right_motor_val = 90;
int left_motor_val = 90;
int spatula_val = 90;

char send_array[100];

void setup() {
  Serial.begin(57600);
  Serial1.begin(57600);

  for(int i=0; i<3; i++){
    pinMode(ID_LEDs[i], OUTPUT);
  } 
  digitalWrite(ID_LEDs[slave_index], HIGH);
}

void loop() {

  // Get command values
  if (digitalRead(upPin) == HIGH) {
    Serial.println("Up");
    right_motor_val = 180;
    left_motor_val = 0; 
  }
  else if (digitalRead(downPin) == HIGH) {
    Serial.println("Down");
    right_motor_val = 0;
    left_motor_val = 180; 
  }
  else if (digitalRead(rightPin) == HIGH) {
    Serial.println("Left");
    right_motor_val = 180;
    left_motor_val = 180; 
  }
  else if (digitalRead(leftPin) == HIGH) {
    Serial.println("Right");
    right_motor_val = 0;
    left_motor_val = 0; 
  }
  else{
    right_motor_val = 90;
    left_motor_val = 90;
  }
  spatula_val = (int)map(analogRead(spatulaPin), 0, 1023, 85, 95);  

  /* Check for cycle */
  if(digitalRead(cyclePin) == HIGH && (millis() - lastDebounceTime) >= debounceDelay){
    lastDebounceTime = millis();
    
    slave_index++;
    if(slave_index == 3){
      slave_index = 0;
    }
    SLAVE_ID = IDs[slave_index];
    for(int i=0; i<3; i++){
      digitalWrite(ID_LEDs[i], LOW);
    }
    digitalWrite(ID_LEDs[slave_index], HIGH);

    /* Send stop command to all robots after cycle change */
    for(int i=0; i<3; i++){
        strcpy(send_array, "");
        add_int_to_string(send_array, IDs[i], "M", false);
        add_int_to_string(send_array, 90, "L", false);
        add_int_to_string(send_array, 90, "R", true);
        Serial1.print(send_array);
    }
  }

  strcpy(send_array, "");
  add_int_to_string(send_array, SLAVE_ID, "M", false);
  add_int_to_string(send_array, left_motor_val, "L", false);
  add_int_to_string(send_array, right_motor_val, "R", false);
  add_int_to_string(send_array, spatula_val, "S", true);
  Serial.println(send_array);
  Serial1.print(send_array);

  delay(50);
}
