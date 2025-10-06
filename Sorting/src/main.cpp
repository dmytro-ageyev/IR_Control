#include <Arduino.h>

// put function declarations here:
void FillArray(int arr[], int size);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);  
}

void loop() {
  // put your main code here, to run repeatedly:
}

// put function definitions here:
void FillArray(int arr[], int size) {
  if(Serial.available() > 0) {
    for(int i = 0; i < size; i++)
      arr[i] = random(0, 100); // Fill with random numbers between 0 and 99
    Serial.println("Array filled with random numbers.");
    Serial.print("Array contents: ");
    for(int i = 0; i < size; i++) {
      Serial.print(arr[i]);
      if(i < size - 1) Serial.print(", ");
    }
    Serial.println();
  }
}