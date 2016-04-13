//// //// //// //// //// 
// Please read - the following section MUST be completed BEFORE attempting to commit
// changes to GitHub.
//// //// //// //// ////
// Title of File: BodyTemperature.ino
// Name of Editor: Ashwin Sundar
// Date of GitHub commit: April 12 2016
// What specific changes were made to this code, compared to the currently up-to-date code
// on GitHub?: Changed file name for clarity. 
//// //// //// //// ////
// Best coding practices
// 1) When you create a new variable or function, make it obvious what the variable or
// function does. The name of the variable or function should reflect its purpose. 
// 2) Comment copiously. Created a new variable or function? Explain why. Explain what
// your functions do. Feel free to link to sites that you referenced for information. 
// 3) Please compile your code and make sure it is functional BEFORE committing changes to GitHub. 
//// //// //// //// ////
//Arduino 1.0+ Only
//////////////////////////////////////////////////////////////////
#include <Wire.h>
int tmp102Address = 0x48;
int testVariable1 = 0; //created this variable while following the tutorial
int testVariable2 = 0; //created this variable while following the tutorial

void setup(){
  Wire.begin();
}

void loop(){
  // Placeholder for bluetooth communications. I will dump the result of getTemperature
  // into a bluetooth packet and transmit it to anyone listening. 
  
}


// Uses Wire library to request temperature from tmp102 module. Returns temperature
// in Celsius. Precision unknown at this time. 
float getTemperature(){
  Wire.requestFrom(tmp102Address,2); 
  
  byte MSB = Wire.read();
  byte LSB = Wire.read();

  //it's a 12bit int, using two's compliment for negative
  int TemperatureSum = ((MSB << 8) | LSB) >> 4; 

  float celsius = TemperatureSum*0.0625;
  return celsius;
}
