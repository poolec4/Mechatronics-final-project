/*  Xbee_param_set
   Setting XBee parameters using Arduino
*/

void setup() {

  Serial.begin(57600);
  Serial1.begin(57600);

  Serial1.print("+++");

  for (int i = 0; i <= 2; i++)
  {
    while (!Serial1.available());
    Serial.print((char)Serial1.read());
  }
  Serial.print('\n');
}

void loop() {

}
