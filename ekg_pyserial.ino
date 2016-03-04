#include "Arduino.h"

int analogPin = 0;
float analogEkg, voltageEkg;

void setup()
{
  
  Serial.begin(115200);
  
}

void loop()
{
  
  analogEkg = analogRead(analogPin);
  voltageEkg = analogEkg*5.0/1024.0;
  
  
  
  Serial.print("EKGData:");
  Serial.print(voltageEkg);
  Serial.println(":");
  delay(10); // sampling rate = 5ms ~ 200Hz


 
}



