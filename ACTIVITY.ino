
const int led = 13;
int step_count = 0; 

void setup() {
 
// Initial commit to github of this activity.ino file. 
// Ashwin
// March 2 2016

  Serial.begin(9600);
  Serial.setTimeout(25);  //Set limit time to send data
  //defined in setup

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
  
  step_count = step_count + 1;
  Bean.setLed(255,0,0);
  Bean.sleep(25);
  
}

double height = 6*12+2; // inputs INCHES
double weight = 200; // inputs LBS

double stride = height*0.414*0.0254; // inches to meters conversion
double distance = (stride*step_count)/(1609.34); // miles traveled
double calories = distance*(0.57*weight); // calories burnt BASED ON LIVESTRONG
double literature_cal = ((stride*step_count)/1000)*(weight*0.45359237)*0.7788; //Calories burnt based on literature review


Serial.print("Step count: ");
Serial.println(step_count);
Serial.print("Miles performed: ");
Serial.println(distance);
Serial.print("Calories burnt: ");
//Serial.println(calories);
Serial.println(literature_cal);
 
Bean.sleep(25);
    
}

