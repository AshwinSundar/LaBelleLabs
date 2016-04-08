//// //// //// //// //// 
// Please read - the following section MUST be completed BEFORE attempting to commit
// changes to GitHub.
//// //// //// //// ////
// Title of File: ekg_pyserial.ino
// Name of Editor: Ashwin Sundar
// Date of GitHub commit: April 7 2016
// What specific changes were made to this code, compared to the currently up-to-date code
// on GitHub?: Modified the code to work with the Light Blue Bean, so the test subject
// can walk around with the device and we can collect data wirelessly at our laptops. 
//// //// //// //// ////
// Best coding practices
// 1) When you create a new variable or function, make it obvious what the variable or
// function does. The name of the variable or function should reflect its purpose. 
// 2) Comment copiously. Created a new variable or function? Explain why. Explain what
// your functions do. Feel free to link to sites that you referenced for information. 
// 3) Please compile your code and make sure it is functional BEFORE committing changes to GitHub. 
//// //// //// //// ////
//// //// //// //// //// 
// Main Arduino Code - setup and loop
//// //// //// //// ////

#include <Wire.h>
#include <ADXL345.h>


ADXL345 adxl; // variable adxl is an instance of the ADXL345 library

void setup(){
  Serial.begin(9600);
  adxl.powerOn();
}

void loop(){
 
  int x,y,z;  
  adxl.readAccel(&x, &y, &z); //read the accelerometer values and store them in variables  x,y,z

  // Output x,y,z values - Commented out
  Serial.print(x);
  Serial.print(",");
  Serial.print(y);
  Serial.print(",");
  Serial.println(z);
  delay(200);
  
 
}
