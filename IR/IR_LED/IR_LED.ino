double f= 10;
double T = 1.0/f;
unsigned long t_old;
boolean b = false;

const int IR_LED_PIN = 7;

void setup() {
  Serial.begin(9600);
  t_old = millis();
  pinMode(IR_LED_PIN, OUTPUT);
  Serial.print("Frequency: ");
  Serial.println(f);
  Serial.print("Period: ");
  Serial.println(T);
  
}

void loop() {
  if (Serial.available() > 0){
    f = Serial.parseInt();
    T = 1.0/f;
    Serial.print("Frequency: ");
    Serial.println(f);
    Serial.print("Period: ");
    Serial.println(T);
  }
  if (abs(millis()-t_old) >= T*1000.0/2.0){
    b = !(b);
    if (b == true){
      tone(IR_LED_PIN, 38000);
    }
    else{
      noTone(IR_LED_PIN);
    }
    t_old = millis();
  }
}
