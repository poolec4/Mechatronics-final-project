#define SENSE_PIN 20

void setup() {
  Serial.begin(9600);
  pinMode(SENSE_PIN, INPUT);
}

void loop() {
  if(digitalRead(SENSE_PIN) == LOW)
  {
    Serial.println("LED ON");
  }
  else
  {
    Serial.println("LED OFF");
  }
}
