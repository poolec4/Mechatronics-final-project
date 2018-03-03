double f= 10;
double T = 1.0/f;
unsigned long t_old;
boolean b = false;

int outPin = 10;

void setup() {
  Serial.begin(9600);
  t_old = millis();
  pinMode(outPin, OUTPUT);
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
      tone(outPin, 38000);
    }
    else{
      noTone(outPin);
    }
    t_old = millis();
  }
}
