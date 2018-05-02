void setup() {
  Serial.begin(57600);
  Serial1.begin(57600);

  pinMode(24, OUTPUT);
}

void loop() {
  if (Serial1.available() > 0) {
    Serial.println(Serial1.read());

    if (Serial1.read() == 49) {
      digitalWrite(24, HIGH);
    }
  }
  else {
    digitalWrite(24, LOW);
  }
}
