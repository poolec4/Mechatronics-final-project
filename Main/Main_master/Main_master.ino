/*
   Xbee_Master
   Communication for XBee Master Node (Experimental)
*/
#include <string.h>
#include "communications.h"

const int GREEN_LED_PIN = 8;
const int RED_LED_PIN = 9;
const int YELLOW_LED_PIN = 10;

const int n = 3;
int IDs[n] = {2, 3, 4};
String inpt;
char inpt_char[100];
int inpt_ID;
int inpt_IR;
String out_ID;
char out_ID_chr[100];

void setup() {

  Serial.begin(57600);
  Serial.setTimeout(10);
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

  if (Serial1.available() > 0) {     // XBee
    inpt = Serial1.readStringUntil('\n');
    inpt.toCharArray(inpt_char, inpt.length() + 1);
    inpt_ID = parse_string_to_int(inpt_char, "M");
    inpt_IR = parse_string_to_int(inpt_char, "I");
    Serial.print(inpt);

    if (inpt_IR == 0) {
      String disp = "   Slave " + String(inpt_ID) + " found nothing";
      Serial.println(disp);
      digitalWrite(YELLOW_LED_PIN, LOW);
    }

    if (inpt_IR == 1) {
      String disp = "   Slave " + String(inpt_ID) + " found dummy signal";
      Serial.println(disp);
      digitalWrite(YELLOW_LED_PIN, LOW);
    }

    if (inpt_IR == 2) {
      String disp = "   Slave " + String(inpt_ID) + " found correct signal";
      Serial.println(disp);
      digitalWrite(YELLOW_LED_PIN, HIGH);
    }

    Serial1.println(inpt);

    Serial.flush();
    Serial1.flush();

  }
}

