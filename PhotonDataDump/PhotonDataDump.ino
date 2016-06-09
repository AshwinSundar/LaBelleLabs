//// //// //// //// ////
// Please read - the following section MUST be completed BEFORE attempting to commit
// changes to GitHub.
//// //// //// //// ////
// Title of File: PhotonDataDump.ino
// Name of Editor: Ashwin Sundar
// Date of GitHub commit: June 9, 2016
// What specific changes were made to this code, compared to the currently up-to-date code
// on GitHub?: Wrote EKG collection function. Moved temperature collection into its own
// function. Experimented with sampling rates. 
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
int lastTempCheck = 0;
int lastEKGCheck = 0; 
String EKGData; // collection of EKG points, to be published in bulk

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
    VOut = analogRead(A3);  // determine the reference voltage by querying VOut on A3
 
    checkTemp();
    checkEKG();
}

void checkTemp()
{
    if ((millis() - lastTempCheck) > 1000) // reads temperature once per second. 
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
        Particle.publish("newTemp", String(floor(temperature*100 + 0.5)/100).remove(5,6)); 

        // resets the "timer"
        lastTempCheck = floor(millis()/1000)*1000; // rounds down temp check time to 1 second
    }
    
}

void checkEKG()
{
    if ((millis() - lastEKGCheck) >= 10)
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
