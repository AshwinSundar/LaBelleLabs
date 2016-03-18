//// //// //// //// //// 
// Please read - the following section MUST be completed BEFORE attempting to commit
// changes to GitHub.
//// //// //// //// ////
// Title of File: ekg_pyserial.ino
// Name of Editor: Ashwin Sundar
// Date of GitHub commit: March 16, 2016
// What specific changes were made to this code, compared to the currently up-to-date code
// on GitHub?: Reviewed code with Muai. Wrote computeAverageOfArrayElements function. Made 
// provision for standard deviation function. 
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
#include "Arduino.h"

int analogPin = 0;
float voltageEKG;

void setup()
{
  
}

void loop()
{
  // Refer to page 80 of medical instrumentation text for information on
  // calculating statistical distributions of data and noise
  voltageEKG = analogRead(analogPin)*5.0/1024.0;
  // Use Professor Spano's peak detect algorithm here. 
  
  // Establish an SD communications protocol here. Store the EKG waveform on the SD card. 

  delay(10); // sampling rate = 100Hz to satisfy Nyquist criteria for EKG waveform detection. 
}
//// //// //// //// ////
//// //// //// //// ////
// Auxilliary functions
//
// Numerical differentiation describes an algorithm that estimates the derivative of a 
// mathematical function using values of the function separated by h time. numericalDifferentiation
// accepts two floats - a starting point f(x1), a finish point f(x2) and a time distance to the finish 
// point f(x+h). f is not a mathematical function. Rather, it's just the incoming analog data. When 
// this function is called, the slope between x1 and x2 is calculated. For our purposes, this is the 
// instantaneous rate of change of our data, although mathematicians might get upset at my liberal use
// of the word "instantaneous". 
float numericalDifferentiation(float x1, float x2, float h) {
  return (x2 - x1)/h;  
}

// computeAverageOfArrayElements returns the average of an array of any size. 
float computeAverageOfArrayElements(float x[]){
  // i is an incrementation variable.
  float sumOfArrayElements = 0;
  int lengthOfArray = sizeof(x); 
  
  for(int i = 0; i < sizeof(x); i++)
  {
    sumOfArrayElements += x[i];
  }
  
  return sumOfArrayElements/lengthOfArray;
}

// Made provision for standard deviation function. 
float standardDeviationOfArrayElements(float x[]){
}


