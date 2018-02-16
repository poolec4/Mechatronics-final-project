/*  Xbee_param_set
   Setting XBee parameters using Arduino
*/
void read_char_from_serial()
{
  while(1)
  {
    char read_char;
    
    while (!Serial1.available());
    read_char = (char)Serial1.read();
    Serial.print(read_char);
    if(read_char == '\r')
      break;
  }
  Serial.print('\n');
}

void read_hex_from_serial()
{
  String string_buff = "0x";
  byte read_byte;
  while(1)
  {
    while (!Serial1.available());
    read_byte = (byte)Serial1.read();
    if(read_byte == '\r')
      break;
    string_buff += String(read_byte,HEX);
  }
  Serial.print(string_buff);
  Serial.print('\n');
}

void setup() {

  Serial.begin(9600);
  Serial1.begin(9600);

  Serial1.print("+++");
  read_char_from_serial();

  Serial1.print("ATID 1000\r");
  read_char_from_serial();
  Serial1.print("ATDL 42\r");
  read_char_from_serial();
  Serial1.print("ATMY 1\r");
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
  if (Serial.available()) {      // If anything comes in Serial (USB),
    Serial1.write(Serial.read());   // read it and send it out Serial1 (pins 0 & 1)
  }

  if (Serial1.available()) {     // If anything comes in Serial1 (pins 0 & 1)
    Serial.write(Serial1.read());   // read it and send it out Serial (USB)
  }
}


