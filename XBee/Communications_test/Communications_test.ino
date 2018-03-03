#include <string.h>
#include "communications.h"

char send_array[100];
String receive_string;
char receive_array[100];

void setup() {
  Serial.begin(9600);

  int A=1, B=2.0, C=3;

  /* on server side */
  
  add_int_to_string(send_array, 1, "A", false);
  add_int_to_string(send_array, 2, "B", false);
  add_int_to_string(send_array, 3, "C", true);

  Serial.println(send_array);
  /* message send_array sent over xbee here */

  /* on client side */

  int A_r = parse_string_to_int(send_array, "A");
  int B_r = parse_string_to_int(send_array, "B");
  int C_r = parse_string_to_int(send_array, "C");

  Serial.println(A_r);
  Serial.println(B_r);
  Serial.println(C_r);
}

void loop() {

}
