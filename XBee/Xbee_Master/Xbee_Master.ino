/*  
 * Xbee_Master
 * Communication for XBee Master Node (Experimental)
*/
#include <string.h>

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
  Serial1.print("ATDL 42\r"); //Other ID
  read_char_from_serial();
  Serial1.print("ATMY 1\r"); //My ID
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

double parse_string_to_double(char *string, char const *tag){
  
  char* string_copy = (char*)malloc(strlen(string) + 1);
  char* char_result;
  char* token;
  double result = 0.0;

  strcpy(string_copy, string);
  token = strtok(string_copy, "&");

  while(token){
    char* equals_sign = strchr(token, '=');
    if(equals_sign){
      *equals_sign = 0;
      if( 0 == strcmp(token, tag)){
        equals_sign++;
        char_result = (char*)malloc(strlen(equals_sign) + 1);
        strcpy(char_result, equals_sign);
        result = atof(char_result);
        free(char_result);
      }
    }
    token = strtok(0, "&");
  }
  free(string_copy);

  return result;
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
