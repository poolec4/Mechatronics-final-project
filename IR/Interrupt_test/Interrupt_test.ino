unsigned long time_stamp;
unsigned int count0 = 0;
unsigned int count1 = 0;
unsigned int count2 = 0;


unsigned int output0 = 0;
unsigned int output1 = 0;
unsigned int output2 = 0;


void setup()
{
  Serial.begin(9600);
  time_stamp = millis();
  attachInterrupt(0, sensor0, RISING); // sensor 1 on mega pin no 2 Due pin 0
  attachInterrupt(1, sensor1, RISING); // sensor 2 on mega pin no 3 Due Pin 1
  attachInterrupt(2, sensor2, RISING); // sensor 3 on mega pin no 21 Due Pin 2
}


void loop()
{
  if(millis() - time_stamp > 50)
  {
    detachInterrupt(0); // disable interrups
    detachInterrupt(1);
    detachInterrupt(2);
    output0 = count0 * 20;
    output1 = count1 * 20;
    output2 = count2 * 20;
    Serial.print("Sensor0 Reading = ");
    Serial.println(output0);
    Serial.print("Sensor1 Reading = ");
    Serial.println(output1);
    Serial.print("Sensor2 Reading = ");
    Serial.println(output2);
    delay(40);
    count0 = 0;
    count1 = 0;
    count2 = 0;
    time_stamp = millis();
    attachInterrupt(0, sensor0, RISING); // we have done with this mesasurenment,enable interrups for next cycle
    attachInterrupt(1, sensor1, RISING);
    attachInterrupt(2, sensor2, RISING);
  }
}


void sensor0()
{
  count0++;
}


void sensor1()
{
  count1++;
}


void sensor2()
{
  count2++;
}


