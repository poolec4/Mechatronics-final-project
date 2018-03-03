const int num_samples = 3;
int count = 0;
unsigned long t_change, t_old, t_delay;
float f[num_samples], f_median;
volatile byte state_change = false;

void setup()
{
  Serial.begin(9600);
  t_change = millis();
  t_old = millis();
  attachInterrupt(digitalPinToInterrupt(2), irStateChange, CHANGE);
}

void loop()
{
  if(state_change == true)
  {
    t_delay = t_change-t_old;
    f[count] = (float)1.0/(t_delay/1000.0*2.0);
    count++;
    t_old = t_change;
    state_change = false;

    if(count >= num_samples)
    {  
      sortFreq();
      
      if(num_samples%2 == 0)
      {  
        f_median = (f[(int)floor((float)num_samples/2.0)]+f[(int)ceil((float)num_samples/2.0)])/2.0;
      }
      else
      {
        f_median = f[(int)floor((float)num_samples/2.0)];  
      }
      
      count = 0; 
      Serial.print("Frequency = ");
      Serial.println(f_median);
    }
  }
}


void irStateChange()
{
  t_change = millis();
  state_change = true;
}

void sortFreq() {
  for (int i = 1; i < num_samples; i++){
       float key = f[i];
       int j = i-1;
       while (j >= 0 && f[j] > key)
       {
           f[j+1] = f[j];
           j = j-1;
       }
       f[j+1] = key;
   }
}

