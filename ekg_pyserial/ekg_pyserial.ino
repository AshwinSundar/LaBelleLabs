//// //// //// //// //// 
// Please read - the following section MUST be completed BEFORE attempting to commit
// changes to GitHub.
//// //// //// //// ////
// Title of File: ekg_pyserial.ino
// Name of Editor: Ashwin Sundar
// Date of GitHub commit: April 12, 2016
// What specific changes were made to this code, compared to the currently up-to-date code
// on GitHub?: Made minor organizational changes. Moved callibration function into "Auxillary 
// functions" section. 
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
#include "math.h"


//sizeof(array) returns the amounts of bytes in the array. Dividing the total amount of bytes in the array
//by the byte size of a single element in the array gives the total amount of elements in the array.
//http://en.cppreference.com/w/cpp/language/sizeof -> c++ reference for 

#define array_size(array) (sizeof((array))/sizeof((array[0])))

//defining the size of the array, this is arbitrary & can be changed.
//Weaknesses of predefined array:
//1.Limited storage of EKG due to size of the array -> User could still have more inputs
//2.Wasted memory if we do not use entire array -> User may use too few array elements and waste memory (RAM)
#define array_length 250

//Using the timer function instead of delay function to set sampling rate
//Arduino timer function returns an unsigned long variable
//Created time variable to store milliseconds from the millis function
//https://www.arduino.cc/en/Reference/Millis
unsigned long time;

//EKG Analog Pin
int analogPin = 0;
//Instantiating an array
float voltageEKG[array_length];
//Creating an index variable for the array
int index = 0;
//Passing the address of variable to a pointer, so that it can be passed to a function and dereferenced
//to change the value of index
int *index_address = &index;

void setup()
{
  
}

void loop()
{
  //Creating a pointer that will accept the pointer from the calibration function
  float * voltageEKGCalibrated;
  voltageEKGCalibrated = calibration();
  time = millis();
  
  // Refer to page 80 of medical instrumentation text for information on
  // calculating statistical distributions of data and noise
  
  
  //Every 10 milliseconds this statement is true and thus executes whats inside the if statement
  if((time % 10)==0){
  
  //int i calls the index_array functions and passes the pointer index_address to get the index of array element
  //that needs to be updated in the circular array
  int i = index_array(index_address); 
  
  //voltageEKGCalibrated replaces its ith - 1 element with a new analog voltage reading
  //Initially the index is at 0 and when incremented equals to 1
  //Thus to update the 0 element (the first element in the array) i is substracted by one
  *(voltageEKGCalibrated+i-1) = analogRead(analogPin)*5.0/1024.0;
  //voltageEKGCalibrated[i-1] = analogRead(analogPin)*5.0/1024.0 would also execute the same way
  // Use Professor Spano's peak detect algorithm here. 
  
  }
  
  //Passing the pointer to the auxillary functions:
  //Float average gets the average of the array
  float average = computeAverageOfArrayElements(voltageEKGCalibrated);
  //Float standard_deviation gets the standard deviation of the array
  float standard_deviation = standardDeviationOfArrayElements(voltageEKGCalibrated);
  //Float range gets the range of the array
  float range = rangeOfArray(voltageEKGCalibrated);
  //Float numerical_derivative gets the slope between two consective points where the time interval between them is 10 milliseconds
  //10 milliseconds = 0.01 seconds thus that is why the third input in numericalDifferentiation is 0.01
  float numerical_derivative = numericalDifferentiation(*(voltageEKGCalibrated+index-1),*(voltageEKGCalibrated+index),0.01);
  
  
  // Establish an SD communications protocol here. Store the EKG waveform on the SD card. 

  // sampling rate = 100Hz to satisfy Nyquist criteria for EKG waveform detection. 
}

//Creating an index function that returns an integer
//Index_array accepts an pointer to integer, to dereference the pointer and change its value.
int index_array (int *x){
  //Derefencing the pointer and incrementing the interger its pointing to by 1
  //In this case it is incrementing the global variable index by 1
  *x = *x+1;
  
  //If the value of index is greater than the length of the array it starts it back to the initial point
  if(*x > array_length){
    *x = 1;
  }
  
  //Passing the derefence value to an index variable that is returned when called upon
  //Essentially creating a circular array on a predefined array
  int index = *x;
  return index;
  
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
// of the word "instantaneous". Function has not been validated yet. 
float numericalDifferentiation(float x1, float x2, float h) {
  return (x2 - x1)/h;  
}

// computeAverageOfArrayElements returns the average of an array of any size. Function has not been validated yet. 
float computeAverageOfArrayElements(float x[]){
  // i is an incrementation variable.
  float sumOfArrayElements = 0;
  int lengthOfArray = array_size(x); 
  
  for(int i = 0; i < lengthOfArray; i++)
  {
    sumOfArrayElements += x[i];
  }
  
  return sumOfArrayElements/lengthOfArray;
}

// Made provision for standard deviation function. Computed average inside of function to improve run time.
// math.h library doesn't occupy any space unless you try to access it. I know in python (or Fortran?) you can 
// pull only the function you want to use. I'm not sure how to do that in C++. This might be a good question for 
// Prof. Spano. Function has not been validated yet. 
// Using https://en.wikipedia.org/wiki/Standard_deviation as reference for standard deviation computation. 
float standardDeviationOfArrayElements(float x[]){
  float arrayAverage = computeAverageOfArrayElements(x);
  float elementDeviationSum = 0; 
  int lengthOfArray = array_size(x);
  for (int i = 0; i < lengthOfArray; i++)
  {
    elementDeviationSum += pow((x[i] - arrayAverage), 2);
  }
  
  return pow(elementDeviationSum/lengthOfArray, 0.5);
}

// rangeOfArray computes the difference between the maximum value of the array and the minimum value of the array.
// The function thus gives the range of array. Function has not yet been validated
float rangeOfArray(float x[]){
  int lengthOfArray = array_size(x);
  float minimumValue = x[0];
  float maximumValue = x[0];
  for (int i = 0; i < lengthOfArray; i++)
  {
    if(minimumValue > x[i+1]){
      minimumValue = x[i+1];
    }
    if(maximumValue < x[i+1]){
      maximumValue = x[i+1];
    }
  }
  return (maximumValue-minimumValue);
}

//Initial EKG Calibration functions where it occupies all the elements in the voltageEKG array
//To return an float array the calibration function was created that declares its return type
//as returning a float pointer
float* calibration(){
  
  int i = 0;
  
  while(i < array_length){
    time = millis();
    //Any multiple of 10 returns 0 when modulus with ten
    //Thus after every 10 milliseconds the if statement is true and thus executes every 10 milliseconds
    //http://www.cprogramming.com/tutorial/modulus.html
    if ((time % 10)==0){
      //Storing analog voltage into array
      voltageEKG[i] = analogRead(analogPin)*5.0/1024.0;
      i++;
    }
  }
  
  //returning the array pointer
  return voltageEKG; 
  
}

