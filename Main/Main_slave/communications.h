#ifndef communications_h
#define communications_h

void add_int_to_string(char *string, int value, char const *tag, bool last_value_in_string)
{
    char temp[100];

    sprintf(temp, "%s=%d", tag, value);
    strcat(string, temp);
    if (last_value_in_string == false)
    {
      strcat(string, "&");
    }
    else
    {
      strcat(string, "\n");
    }
}

bool is_tag_available(char *string, char const *tag)
{
  char* string_copy = (char*)malloc(strlen(string) + 1);
  char* char_result;
  char* token;
  bool available = false;

  strcpy(string_copy, string);
  token = strtok(string_copy, "&");

  while(token)
  {
    char* equals_sign = strchr(token, '=');

    if(equals_sign)
    {
      *equals_sign = 0;

      if(0 == strcmp(token, tag))
      {
	available = true;
      }
    }
    token = strtok(0, "&");
  }
  free(string_copy);

  return available;
}

int parse_string_to_int(char *string, char const *tag)
{
  char* string_copy = (char*)malloc(strlen(string) + 1);
  char* char_result;
  char* token;
  double result = 0.0;

  strcpy(string_copy, string);

  token = strtok(string_copy, "&");

  while(token)
  {
    char* equals_sign = strchr(token, '=');

    if(equals_sign)
    {
      *equals_sign = 0;

      if(0 == strcmp(token, tag))
      {
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

double parse_string_to_double(char *string, char const *tag)
{
  char* string_copy = (char*)malloc(strlen(string) + 1);
  char* char_result;
  char* token;
  double result = 0.0;

  strcpy(string_copy, string);

  token = strtok(string_copy, "&");

  while(token)
  {
    char* equals_sign = strchr(token, '=');

    if(equals_sign)
    {
      *equals_sign = 0;

      if(0 == strcmp(token, tag))
      {
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

void read_char_from_serial1(){
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

void read_hex_from_serial1(){
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

float QTIRead(int QTI_PIN) {
  float duration = 0;
  pinMode(QTI_PIN, OUTPUT); // Sets pin as OUTPUT
  digitalWrite(QTI_PIN, HIGH); // Pin HIGH
  delayMicroseconds(500); // Waits for 0.5 millisecond
  pinMode(QTI_PIN, INPUT); // Sets pin as INPUT
  digitalWrite(QTI_PIN, LOW); // Pin LOW

  while (digitalRead(QTI_PIN)) { // Waits for the pin to go LOW
    duration = duration + 1;
  }
  
  return duration; // Returns the duration of the pulse
}

#endif
