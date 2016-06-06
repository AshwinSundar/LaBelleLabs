//// //// //// //// ////
// Please read - the following section MUST be completed BEFORE attempting to commit
// changes to GitHub.
//// //// //// //// ////
// Title of File: PhotonDataDump.ino
// Name of Editor: Ashwin Sundar
// Date of GitHub commit: June 6, 2016
// What specific changes were made to this code, compared to the currently up-to-date code
// on GitHub?: Determined how to use Particle.publish() to achieve high-speed transmission.
// Basically, I'm saving data in bulk, and transmitting it once I hit the buffer limit of
// 255 bytes. Particle.publish() limits me to 3 such transmissions per second. If I attempt
// 4 for 2 consecutive seconds, the Photon will impose a cooldown of 4 sceonds before I 
// can publish more data. I've experimented with the temperature data, and it works. So
// I can just apply the code to the EKG data. I need to condense the EKG data as much as 
// possible before transmission if I want to achieve high throughput. Otherwise the data
// stream will start to lag. We could experiment with lagging data streams as well. 
//// //// //// //// ////
// Best coding practices
// 1) When you create a new variable or function, make it obvious what the variable or
// function does. The name of the variable or function should reflect its purpose.
// 2) Comment copiously. Created a new variable or function? Explain why. Explain what
// your functions do. Feel free to link to sites that you referenced for information.
// 3) Please compile your code and make sure it is functional BEFORE committing changes to GitHub.
//// //// //// //// ////
//// //// //// //// ////
// Particle Photon connections to ADXL335
// Photon     <-> ADXL335 Breakout Board
// 3v3         -  Vin
// 3Vo         -  A3
// GND         -  GND
// ZOut        -  A0
// YOut        -  A1
// XOut        -  A2
// Test        -  A3

#include <math.h>
#include <Particle.h>

// int's in C++ cannot have numbers in their name. Hence the bizarre nomenclature. 
int XAccel; // x acceleration from the ADXL335
int YAccel; // y acceleration from the ADXL335
int ZAccel; // z acceleration from the ADXL335
int VOut; // voltage output of the ADXL335
//
double thermistorAnalog; // contains raw analog data from thermistor circuit VOut
double thermistorResistance; // converts analog reading to resistance
double temperature; // converts thermistor resistance to temperature, according to Amphenol datasheet 
int EKG; // analog reading of EKG
String tempData = "0"; // stores temperature data as a large block

void setup()
{

 // Let's register some Particle variables to the Particle cloud.
 // This means when we "ask" the Particle cloud for the string in quotes, we will get
 // the value after the comma. Note: The published variable name in quotes AND the
 // actual variable name MUST be 12 characters or less. Otherwise, the cloud will
 // ignore the variable and you won't be able to see it when you make a request.

 Particle.variable("XAccel", XAccel);
 Particle.variable("YAccel", YAccel);
 Particle.variable("ZAccel", ZAccel);
 Particle.variable("VOut", VOut);
 Particle.variable("EKG", EKG);
 Particle.variable("temp", temperature); // temp in Celsius
}

void loop()
{
 // read the raw ADXL335 acceleration using analogRead().
 XAccel = analogRead(A2);
 YAccel = analogRead(A1);
 ZAccel = analogRead(A0);
 
 // determine the reference voltage by querying VOut on A3
 VOut = analogRead(A3);
 
 // read the raw thermistor analog data
 thermistorAnalog = analogRead(A5);
 thermistorResistance = (10000*thermistorAnalog/4096)/(1-thermistorAnalog/4096);
 // calculated temperature by plotting a line of best fit using linear algebra and MATLAB. Error was found to be 3.7998*10^-4 Kelvins using Igor Pro. 
 temperature = pow((1.1106*.001 + 2.3724*.0001*log(thermistorResistance) + 7.4738*.00000001*pow(log(thermistorResistance), 3)), -1) - 273.15;
 
 tempData += ",";
 tempData += String(floor(temperature*100 + 0.5)/100).remove(5,6); // rounds the temperature to 2 decimals, truncates last 6 elements (trailing zeroes)
 delay(333/40);
 
 if (tempData.length() > 240) // Once the chunk reaches 40 data points, send it. Max 255 bytes (or 255*3 = 765 bytes per second)  
 {
    Particle.publish("Temps", String(tempData));
    tempData = "";
 }
 
 // read the EKG, connected to A4 of the Photon. 
 EKG = analogRead(A4);
}

