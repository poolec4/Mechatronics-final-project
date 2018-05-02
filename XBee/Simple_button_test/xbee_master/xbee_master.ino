void setup() {
  Serial.begin(57600);
  Serial1.begin(57600);
}

void loop() {
  if(digitalRead(11) == HIGH){
    Serial1.print("1");
    Serial.println('1');
  }
  else{
    //Serial1.println("0");
  }
}
