#ifndef frequency_h
#define frequency_h

void sortFreq() {
  for (int i = 1; i < num_samples; i++){
       float key = f[i];
       int j = i-1;
       while (j >= 0 && f[j] > key){
           f[j+1] = f[j];
           j = j-1;
       }
       f[j+1] = key;
   }
}

void calculateFreq() {
  t_delay = t_change-t_old;
  f[count] = (float)1.0/(t_delay/1000.0*2.0);
  count++;
  t_old = t_change;
  IR_state_change = false;

  if(count >= num_samples){  
    sortFreq();
    
    if(num_samples%2 == 0){  
      f_median = (f[(int)floor((float)num_samples/2.0)]+f[(int)ceil((float)num_samples/2.0)])/2.0;
    }
    else{
      f_median = f[(int)floor((float)num_samples/2.0)];  
    }
    count = 0; 
  }
}

#endif