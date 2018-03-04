/*  Xbee_slave
   Communication for XBee Slave Node (Experimental)
*/

#include <string.h>
#include "communications.h"

/* Definitions */
const int SLAVE_ID = 2;
const int THIS_SLAVE_ID = 2;

const int GREEN_LED_PIN = 8;
const int RED_LED_PIN = 9;
const int YELLOW_LED_PIN = 10;

/* IR Stuff */
const int IR_SENSE_PIN = 2;
boolean IR_sense = true;
boolean freq_sense = true;
const int num_samples = 3;
int count = 0;
unsigned long t_change, t_old, t_delay;
float f[num_samples], f_median;
volatile byte IR_state_change = false;

int IR_state = 1;  // 0 = nothing, 1 = ir but wrong freq, 2 = right freq

/* XBee stuff */
String inpt;
char inpt_char[100];
int inpt_ID;
int inpt_IR;

char send_array[100];

void setup() {
  t_change = millis();
  t_old = millis();
  attachInterrupt(digitalPinToInterrupt(2), irStateChange, CHANGE);

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
}

void loop() {
  //* IR SENSE *//

  if(IR_state_change == true)
  {
    calculateFreq();
  }

  if(f_median > 8.0 && f_median < 12.0){
    IR_sense = true;
    freq_sense = true;
  }
  else if(f_median < 8.0 || f_median > 12.0){
    IR_sense = true;
    freq_sense = false;
  }

  if(millis()-t_change > 500){ // timeout if signal hasn't been seen for a while
    IR_sense = false;
    freq_sense = false;
  }
  
  //* TRANSMITTING *//
  
  strcpy(send_array, "");

  if (IR_sense == false && freq_sense == false) { // no signal
    IR_state = 0;
    digitalWrite(GREEN_LED_PIN, LOW);
    digitalWrite(RED_LED_PIN, LOW);
  }
  if (IR_sense == true && freq_sense == false) { // decoy signal
    IR_state = 1;
    digitalWrite(GREEN_LED_PIN, LOW);
    digitalWrite(RED_LED_PIN, HIGH);
  }
  if (IR_sense && freq_sense) { // true signal
    IR_state = 2;
    digitalWrite(GREEN_LED_PIN, HIGH);
    digitalWrite(RED_LED_PIN, LOW);
  }

  add_int_to_string(send_array, THIS_SLAVE_ID, "M", false);
  add_int_to_string(send_array, IR_state, "I", true);

  Serial1.print(send_array);
  
  //* RECEIVING *//
  

  if (Serial1.available()) {     
    inpt = Serial1.readStringUntil('\n');
    inpt.toCharArray(inpt_char, inpt.length()+1);
    inpt_ID = parse_string_to_int(inpt_char, "M");
    inpt_IR = parse_string_to_int(inpt_char, "I");

    if(inpt_ID != THIS_SLAVE_ID){
      if (inpt_IR == 0){ // someone else found nothing
        digitalWrite(YELLOW_LED_PIN, LOW);
      }
      
      if (inpt_IR == 1){ // someone else found the dummy
        digitalWrite(YELLOW_LED_PIN, LOW);
      }
      
      if (inpt_IR == 2){ // someone else found the real signal
        digitalWrite(YELLOW_LED_PIN, HIGH);
      }
    }
  }

  delay(1000);
  Serial1.flush();
  Serial.flush();
}


void irStateChange()
{
  t_change = millis();
  IR_state_change = true;
}

void sortFreq() {
  for (int i = 1; i < num_samples; i++){
       float key = f[i];
       int j = i-1;
       while (j >= 0 && f[j] > key){
           f[j+1] = f[j];
           j = j-1;
       }
       f[j+1] = key;
   }
}

void calculateFreq() {
  t_delay = t_change-t_old;
  f[count] = (float)1.0/(t_delay/1000.0*2.0);
  count++;
  t_old = t_change;
  IR_state_change = false;

  if(count >= num_samples){  
    sortFreq();
    
    if(num_samples%2 == 0){  
      f_median = (f[(int)floor((float)num_samples/2.0)]+f[(int)ceil((float)num_samples/2.0)])/2.0;
    }
    else{
      f_median = f[(int)floor((float)num_samples/2.0)];  
    }
    count = 0; 
  }
}

