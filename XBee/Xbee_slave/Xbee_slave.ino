/*  Xbee_slave
   Communication for XBee Slave Node (Experimental)
*/

#include <string.h>
#include "communications.h"

const int SLAVE_ID = 2;
const int THIS_SLAVE_ID = 2;

const int GREEN_LED_PIN = 8;
const int RED_LED_PIN = 9;

boolean IR_sense = true;
boolean freq_sense = true;

int IR_state = 1;  // 0 = nothing, 1 = ir but wrong freq, 2 = right freq

char send_array[100];

void setup() {

  pinMode(8, INPUT);

  Serial.begin(57600);
  Serial.setTimeout(10);
  Serial1.begin(57600);

  Serial1.print("+++");
  read_char_from_serial();

  Serial1.print("ATID 1000\r");
  read_char_from_serial();
  Serial1.print("ATDL 1\r");
  read_char_from_serial();
  Serial1.print("ATMY " + String(SLAVE_ID) + "\r");
  read_char_from_serial();

  Serial1.print("ATID\r");
  read_char_from_serial();
  Serial1.print("ATDH\r");
  read_char_from_serial();
  Serial1.print("ATDL\r");
  read_char_from_serial();
  Serial1.print("ATMY\r");
  read_char_from_serial();

  delay(500);
}

void loop() {

  strcpy(send_array, "");

  if (IR_sense == false && freq_sense == false) {
    IR_state = 0;
  }
  if (IR_sense == true && freq_sense == false) {
    IR_state = 1;
  }
  if (IR_sense && freq_sense) {
    IR_state = 2;
  }

  add_int_to_string(send_array, THIS_SLAVE_ID, "M", false);
  add_int_to_string(send_array, IR_state, "I", true);

  Serial1.print(send_array);

  if (Serial1.available()) {     // If anything comes in Serial1 (pins 0 & 1)
    Serial.println(Serial1.readStringUntil('\n'));   // read it and send it out Serial (USB)
  }
  delay(1000);
  Serial1.flush();
  Serial.flush();
  //  }
}


