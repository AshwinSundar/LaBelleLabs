//// //// //// //// //// 
// Please read - the following section MUST be completed BEFORE attempting to commit
// changes to GitHub.
//// //// //// //// ////
// Title of File: ACTIVITY.ino
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
//Arduino 1.0+ Only
//////////////////////////////////////////////////////////////////
//©2011 bildr
//Released under the MIT License - Please reuse change and share
//Simple code for the TMP102, simply prints temperature via serial
//////////////////////////////////////////////////////////////////

#include <Wire.h>
int tmp102Address = 0x48;
int testVariable1 = 0;
int testVariable2 = 0;

void setup(){
  Serial.begin(9600);
  Wire.begin();
  Serial.println("reset");
}

void loop(){

  float celsius = getTemperature();
  Serial.print("Celsius: ");
  Serial.println(celsius);


  float fahrenheit = (1.8 * celsius) + 32;  
  Serial.print("Fahrenheit: ");
  Serial.println(fahrenheit);

  delay(1000); //just here to slow down the output. You can remove this
}

float getTemperature(){
  Wire.requestFrom(tmp102Address,2); 

  byte MSB = Wire.read();
  byte LSB = Wire.read();

  //it's a 12bit int, using two's compliment for negative
  int TemperatureSum = ((MSB << 8) | LSB) >> 4; 

  float celsius = TemperatureSum*0.0625;
  return celsius;
}
