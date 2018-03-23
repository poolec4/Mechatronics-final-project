#include <Servo.h>

void setup() {
  // put your setup code here, to run once:
  Servo L_servo;  // create servo object to control a servo
  Servo R_servo;  // create servo object to control a servo
  
  L_servo.attach(2);  // attaches the servo on pin 9 to the servo object
  R_servo.attach(3);  // attaches the servo on pin 9 to the servo object

  L_servo.write(92);              
  R_servo.write(92);              

}

void loop() {
  // put your main code here, to run repeatedly:

}
