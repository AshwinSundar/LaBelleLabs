//// //// //// //// //// 
// Please read - the following section MUST be completed BEFORE attempting to commit
// changes to GitHub.
//// //// //// //// ////
// Title of File: ekg_pyserial.ino
// Name of Editor: Ashwin Sundar
// Date of GitHub commit: March 14, 2016
// What specific changes were made to this code, compared to the currently up-to-date code
// on GitHub?: Cleaned up the code. Removed unneccessary variables. Rewrote any variables 
// that had unclear names
//// //// //// //// ////
// Best coding practices
// 1) When you create a new variable or function, make it obvious what the variable or
// function does. The name of the variable or function should reflect its purpose. 
// 2) Comment copiously. Created a new variable or function? Explain why. Explain what
// your functions do. Feel free to link to sites that you referenced for information. 
// 3) Please compile your code, run it on a device, and make sure it is functional BEFORE
// committing changes to GitHub. 
//// //// //// //// ////

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



