//// //// //// //// ////
// Please read - the following section MUST be completed BEFORE attempting to commit
// changes to GitHub.
//// //// //// //// ////
// Title of File: PhotonDataDump.ino
// Name of Editor: Ashwin Sundar
// Date of GitHub commit: June 13, 2016
// What specific changes were made to this code, compared to the currently up-to-date code
// on GitHub?: Wrote acceleration collection function.  
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

// note: ints in C++ cannot have numbers in their name.
double thermistorAnalog; // contains raw analog data from thermistor circuit VOut
double thermistorResistance; // converts analog reading to resistance
double temperature; // converts thermistor resistance to temperature, according to Amphenol datasheet 
String accelData; // ADXL335 acceleration magnitude data points
int EKG; // analog reading of EKG
int lastTempCheck = 0;
int lastEKGCheck = 0; 
int lastAccelCheck = 0;
String EKGData; // collection of EKG points, to be published in bulk

void setup()
{

}

void loop()
{
    checkTemp();
    checkEKG();
    checkAccel();
}

void checkTemp()
{
    if ((millis() - lastTempCheck) > 1000) // 1 Hz sampling rate for temperature
    {
        thermistorAnalog = analogRead(A5); // read the raw thermistor analog data
 
        // convert resistance to ohms using voltage divider equation, where R1 = 10k ohms
        // shielded equation from dividing by zero by putting it in an if statement
        if (thermistorAnalog < 4095)
        {
            thermistorResistance = (10000*thermistorAnalog/4095)/(1-thermistorAnalog/4095);
        }
    
        else if (thermistorAnalog >= 4095) // if the analog reads 4095, then there is a short or the thermistor has failed.
        {
            thermistorResistance = 0; 
        }
        // calculated temperature by plotting a line of best fit using linear algebra and MATLAB. Error was found to be 3.7998*10^-4 Kelvins 
        // using Igor Pro. Compressed some of the math operations to reduce floating point error. 
        temperature = pow((.0011106 + 0.00023724*log(thermistorResistance) + 0.000000074738*pow(log(thermistorResistance), 3)), -1) - 273.15;
        // rounds the temperature to 2 decimals, truncates last 6 elements (trailing zeroes))
        // Particle.publish only accepts chars or strings
        Particle.publish("newTemp", String(floor(temperature*100 + 0.5)/100).remove(5)); 

        // resets the "timer"
        lastTempCheck = floor(millis()/1000)*1000; // rounds down temp check time to 1 second
    }
    
}

void checkEKG()
{
    if ((millis() - lastEKGCheck) >= 10) // 100 Hz sampling rate for EKG
    {
        EKG = analogRead(A4); // read the EKG, connected to A4 of the Photon. 
        EKGData += String(EKG); 
        EKGData += ",";
        
        if (EKGData.length() > 240)
        {
            Particle.publish("EKG", EKGData);
            EKGData = ""; // erases the EKG data buffer, starts over
        }
        
        lastEKGCheck = floor(millis()/10)*10; // rounds down EKG check time to 10 milliseconds. 
    }
    
}

void checkAccel()
{
    if ((millis() - lastAccelCheck >=  200)) // 5 Hz sampling rate for acceleration
    {
        // X, Y, and Z are directly wired to A2, A1, and A0 respectively. Rounds data, truncates trailing zeros and decimal. 
        accelData += String(floor(sqrt(pow(analogRead(A0),2) + pow(analogRead(A1),2) + pow(analogRead(A2),2)) + 0.5)).remove(4); 
        accelData += ",";
        if (accelData.length() > 240)
        {
            Particle.publish("Acceleration", accelData);
            accelData = ""; // erases the acceleration data buffer, starts over
        }
        
        lastAccelCheck = floor(millis()/200)*200; // rounds down accel check to 200 milliseconds. 
    }
}
