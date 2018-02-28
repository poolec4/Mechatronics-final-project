double f2 = 10;

double T2 = 1.0/f2;

unsigned long t_old1;
unsigned long t_old;

boolean bool2 = false;

int outPin = 7;

void setup() {
  Serial.begin(9600);
  t_old = micros();
  pinMode(outPin, OUTPUT);
  Serial.println(f2);
  Serial.println(T2);
  
}

void loop() {
  if (abs(micros()-t_old) >= T2*1000000.0/2.0){
    bool2 = !(bool2);
    if (bool2 == true){
      tone(10, 38000);
    }
    else{
      noTone(10);
    }
    t_old = micros();
  }
}
