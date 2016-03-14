// extra info
// INSTRUCTIONS FOR EDITOR - THIS SECTION MUST BE COMPLETED
// Fill out this header section BEFORE attempting to commit changes to GitHub
// Title of File: ACTIVITY.ino
// Name of Editor: Ashwin Sundar
// Date of GitHub commit: March 14, 2016
// What specific changes were made to this code, compared to the code currently up-to-date
// on GitHub?: Cleaned up the code. Removed unneccessary variables. Rewrote any variables 
// that had unclear names
// 
// Best coding practices
// 1) When you create a new variable or function, make it obvious what the variable or
// function does.
// 2) Comment copiously. Created a new variable or function? Explain why. Explain what
// function do. Feel free to link to sites that you referenced for an algorithm,
// for example. 
// 3) Please compile your code, run it on a device, and make sure it is functional BEFORE
// pushing up to GitHub. 

// Steps are integer values. I initialize step count at 0 when the sketch loads
// for the first time. I currently do not have a way to save the step count in the
// event that the device shuts off. 

int stepCount = 0; 

void setup() {
 
}

void loop() {

    Bean.setLed(0,0,255);
    AccelerationReading accel = {0, 0, 0};
    accel = Bean.getAcceleration();
    
    //Serial.print("x1 value: ");
  //  Serial.println(accel.xAxis);
    //Serial.print("y1 value: ");
  //  Serial.println(accel.yAxis);
    //Serial.print("z1 value: ");
  //  Serial.println(accel.zAxis);
    
    int x1 = accel.xAxis;
    int y1 = accel.yAxis;
    int z1= accel.zAxis;

    Bean.sleep(25);

    accel = Bean.getAcceleration();
  
    int x2 = accel.xAxis;

 //   Serial.print("x2 : ");
 //   Serial.println(x2);

    int y2 = accel.yAxis;
    int z2 = accel.zAxis;

    int deltaX = (x2-x1);
    int deltaY = (y2-y1);
    int deltaZ = (z2-z1);

  if (deltaX <= -20 || deltaY <= -20 || deltaZ <= -20) { //deltaY is the most important
  
  stepCount = stepCount + 1;
  Bean.setLed(255,0,0);
  Bean.sleep(25);
  
}

double height = 6*12+2; // inputs INCHES
double weight = 200; // inputs LBS

double stride = height*0.414*0.0254; // inches to meters conversion
double distance = (stride*stepCount)/(1609.34); // miles traveled
double calories = distance*(0.57*weight); // calories burnt BASED ON LIVESTRONG
double literature_cal = ((stride*stepCount)/1000)*(weight*0.45359237)*0.7788; //Calories burnt based on literature review


Serial.print("Step count: ");
Serial.println(stepCount);
Serial.print("Miles performed: ");
Serial.println(distance);
Serial.print("Calories burnt: ");
//Serial.println(calories);
Serial.println(literature_cal);
 
Bean.sleep(25);
    
}
