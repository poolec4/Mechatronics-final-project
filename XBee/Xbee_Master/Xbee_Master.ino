/*  
 * Xbee_Master
 * Communication for XBee Master Node (Experimental)
*/
#include <string.h>
#include "communications.h"

const int n = 3;
int IDs[n] = {1, 2, 3};
String inpt;
char inpt_char[100];
int inpt_ID;
int inpt_IR;
String out_ID;

void setup() {

  Serial.begin(9600);
  Serial1.begin(9600);

  Serial1.print("+++");
  read_char_from_serial();

  Serial1.print("ATID 1000\r"); //Network
  read_char_from_serial();
  Serial1.print("ATDL 1\r"); //Other ID
  read_char_from_serial();
  Serial1.print("ATMY 0\r"); //My ID
  read_char_from_serial();

  Serial1.print("ATID\r");
  read_char_from_serial();
  Serial1.print("ATDH\r");
  read_char_from_serial();
  Serial1.print("ATDL\r");
  read_char_from_serial();
  Serial1.print("ATMY\r");
  read_char_from_serial();
  
}

void loop() {
  if (Serial.available()) {      // Arduino Serial,
    Serial1.write(Serial.read());   // Send to XBee
  }

  if (Serial1.available()) {     // XBee
    inpt = Serial.readStringUntil('\n'); 
    inpt.toCharArray(inpt_char, inpt.length()+1);
    inpt_ID = parse_string_to_int(inpt_char, "M");
    inpt_IR = parse_string_to_int(inpt_char, "I");
    for(int i=0; i<n; i++){
      out_ID = "ATDL " + String(i) + "\r";
      Serial1.println(out_ID);
      read_char_from_serial(); //replace with Serial.flush()
      Serial1.print(inpt);
    }
  }
}

