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
// 3) For this particular piece of code, since it is written for the LightBlue Bean, the code
// WILL NOT compile in the Arduino IDE. You'll get a bunch of error messages about a lot of 
// functions not being "declared in this scope". That's because those functions only exist in
// the world of LightBlue Bean. You need to compile this code in BeanLoader, make sure it runs
// over there, then copypasta it into here, hit Save, and commit the change in GitHub Desktop. 
// Don't forget to provide a detailed description when you commit a file. 
//// //// //// //// ////

// Steps are integer values. I initialize step count at 0 when the sketch loads
// for the first time. I currently do not have a way to save the step count in the
// event that the device shuts off. 
int stepCount = 0; 

void setup() {
 
}

void loop() {
    AccelerationReading accel = Bean.getAcceleration();
    
    int x1 = accel.xAxis;
    int y1 = accel.yAxis;
    int z1= accel.zAxis;

// The 25 ms period between acceleration readings is arbitrary from what I understand. 
// Please provide reasoning as to why 25 ms has been chosen. 
Bean.sleep(25);

    accel = Bean.getAcceleration();
  
    int x2 = accel.xAxis;
    int y2 = accel.yAxis;
    int z2 = accel.zAxis;

    int deltaX = (x2-x1);
    int deltaY = (y2-y1);
    int deltaZ = (z2-z1);

// This algorithm needs improvement. Acceleration is merely a proxy for step count. We need
// to analyze the data using Professor Spano's EKG algorithm here. The EKG algorithm establishes
// a baseline and a threshold for any set of periodic data. 
  if (deltaX <= -20 || deltaY <= -20 || deltaZ <= -20) { 
  
  stepCount = stepCount + 1;

// This next line is known as a refractory period. We have arbitrarily picked 25 ms. This 
// will not do. Please determine an appropriate refractory period for step count by
// consulting the gait literature provided by Victoria (email Ashwin, he has the literature)
  Bean.sleep(25);
  
}

// These variables must be dynamically set by the experimenter. They cannot be hard-coded
// like this. 
double height = 6*12+2; // inputs INCHES
double weight = 200; // inputs LBS

double stride = height*0.414*0.0254; // inches to meters conversion
double distance = (stride*stepCount)/(1609.34); // miles traveled
double calories = distance*(0.57*weight); // calories burnt BASED ON LIVESTRONG
double literature_cal = ((stride*stepCount)/1000)*(weight*0.45359237)*0.7788; //Calories burnt based on literature review

// Once again, a placeholder. Bluetooth communications protocols need to go here. 
 
Bean.sleep(25);
    
}
