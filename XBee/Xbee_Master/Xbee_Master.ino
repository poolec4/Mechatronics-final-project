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
char out_ID_chr[100];

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
//  if (Serial.available()) {      // Arduino Serial,
//    Serial1.write(Serial.read());   // Send to XBee
//  }
  if (Serial1.available() > 0) {     // XBee
    inpt = Serial1.readStringUntil('\n');
    inpt.toCharArray(inpt_char, inpt.length()+1);
    inpt_ID = parse_string_to_int(inpt_char, "M");
    inpt_IR = parse_string_to_int(inpt_char, "I");
    if (inpt_IR == 1){
      String disp = "Slave " + String(inpt_ID) + " found dummy signal";
      Serial.println(disp);
    }
    if (inpt_IR == 2){
      String disp = "Slave " + String(inpt_ID) + " found correct signal";
      Serial.println(disp);
    }
    for(int i=0; i<n; i++){
      out_ID = "ATDL " + String(IDs[i]) + "\r";
      out_ID.toCharArray(out_ID_chr, out_ID.length()+1);
      Serial1.println(out_ID_chr);
      Serial1.flush();
      //read_char_from_serial(); //replace with Serial.flush()
      Serial1.println(inpt);
      Serial1.flush();
    }
  }
}

void read_char_from_serial(){
  while(1){
    char read_char;
    while (!Serial1.available());
    read_char = (char)Serial1.read();
    Serial.print(read_char);
    if(read_char == '\r')
      break;
  }
  Serial.print('\n');
}

void read_hex_from_serial(){
  String string_buff = "0x";
  byte read_byte;
  while(1){
    while (!Serial1.available());
    read_byte = (byte)Serial1.read();
    if(read_byte == '\r')
      break;
    string_buff += String(read_byte,HEX);
  }
  Serial.print(string_buff);
  Serial.print('\n');
}
