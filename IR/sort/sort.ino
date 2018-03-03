int n = 5;
float arr[5] = {1, 5, 4, 2, 3};

void sort() {
  for (int i = 1; i < n; i++){
       float key = arr[i];
       int j = i-1;
       while (j >= 0 && arr[j] > key)
       {
           arr[j+1] = arr[j];
           j = j-1;
       }
       arr[j+1] = key;
   }
}
