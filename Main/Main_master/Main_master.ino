/*
   Xbee_Master
   Communication for XBee Master Node (Experimental)
*/
#include <string.h>
#include "communications.h"

const int YELLOW_LED_PIN = 10;

const int n = 3;
int IDs[n] = {2, 3, 4};
String inpt;
char inpt_char[100];
int inpt_ID;
int inpt_IR[3];
String out_ID;
char out_ID_chr[100];

void setup() {
  pinMode(YELLOW_LED_PIN, OUTPUT);

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
    inpt_IR[inpt_ID - IDs[0]] = parse_string_to_int(inpt_char, "I");
    Serial.println(inpt);

    if (inpt_IR[0] == 2 || inpt_IR[1] == 2 || inpt_IR[2] == 2) {
//      String disp = "   Slave " + String(inpt_ID) + " found correct signal";
//      Serial.println(disp);
      digitalWrite(YELLOW_LED_PIN, HIGH);
    }
    else if (inpt_IR[0] != 2 && inpt_IR[1] != 2 && inpt_IR[2] != 2) {
//      String disp = "   Slave " + String(inpt_ID) + " found nothing";
//      Serial.println(disp);
      digitalWrite(YELLOW_LED_PIN, LOW);
    }




    Serial1.println(inpt);

    Serial.flush();
    Serial1.flush();

  }
}

