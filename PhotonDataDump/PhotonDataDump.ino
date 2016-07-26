//// //// //// //// ////
// Please read - the following section MUST be completed BEFORE attempting to commit
// changes to GitHub.
//// //// //// //// ////
// Title of File: PhotonDataDump.ino
// Name of Editor: Ashwin Sundar
// Date of GitHub commit: July 6, 2016
// What specific changes were made to this code, compared to the currently up-to-date code
// on GitHub?: Added SD card functionality. First, I confirmed I was able to write temperature
// data to a file. Then, I implemented the functionality with the checkAccel and checkEKG 
// functions. 
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
//// //// //// //// ////
//// //// //// //// ////
// Particle Photon connections to microSD breakout board
// Photon     <-> ADXL335 Breakout Board
// N/A         -  CD
// D3          -  D0
// GND         -  GND
// D4          -  SCK
// 3V3         -  VCC
// D2          -  DI
// D1          -  CS
//// //// //// //// ////
//// //// //// //// ////

#include <math.h>
#include <Particle.h>
// This #include statement was automatically added by the Particle IDE.
#include "SdFat/SdFat.h"

//// //// //// //// ////
//// microSD config ////
// Pick an SPI configuration.
// See SPI configuration section below (comments are for photon).
#define SPI_CONFIGURATION 1
//// //// //// //// ////
// Setup SPI configuration.
#if SPI_CONFIGURATION == 0
// Primary SPI with DMA
// SCK => A3, MISO => A4, MOSI => A5, SS => A2 (default)
SdFat sd;
const uint8_t chipSelect = SS;
#elif SPI_CONFIGURATION == 1
// Secondary SPI with DMA
// SCK => D4, MISO => D3, MOSI => D2, SS => D1
SdFat sd(1);
const uint8_t chipSelect = D1;
#elif SPI_CONFIGURATION == 2
// Primary SPI with Arduino SPI library style byte I/O.
// SCK => A3, MISO => A4, MOSI => A5, SS => A2 (default)
SdFatLibSpi sd;
const uint8_t chipSelect = SS;
#elif SPI_CONFIGURATION == 3
// Software SPI.  Use any digital pins.
// MISO => D5, MOSI => D6, SCK => D7, SS => D0
SdFatSoftSpi<D5, D6, D7> sd;
const uint8_t chipSelect = D0;
#endif  // SPI_CONFIGURATION
//// //// //// //// ////
//// //// //// //// ////

// note: ints in C++ cannot have numbers in their name.
double thermistorAnalog; // contains raw analog data from thermistor circuit VOut
double thermistorResistance; // converts analog reading to resistance
double temperature; // converts thermistor resistance to temperature, according to Amphenol datasheet 
String accelData; // ADXL335 acceleration magnitude data points
int EKG = 0; // analog reading of EKG
String accel;
int xaccel = 0; // analog reading of x acceleration
int yaccel = 0; // analog reading of y acceleration
int zaccel = 0; // analog reading of z acceleration
int lastTempCheck = 0;
int lastEKGCheck = 0; 
int lastAccelCheck = 0;
String EKGData; // collection of EKG points, to be published in bulk

File globalTemp; // for microSD interface
File globalEKG;
File globalAccel;

Timer EKGTimer(10, checkEKG); // creates timer that checks EKG every .01 seconds
Timer TempTimer(5000, checkTemp); // creates timer that checks temperature every 5 seconds
Timer AccelTimer(200, checkAccel); // creates timer that checks acceleration every 0.2 seconds
    
void setup()
{
    Particle.syncTime(); // syncs time to correct time zone
    EKGTimer.start(); // by default, timers are stopped. start them. 
    TempTimer.start();
    AccelTimer.start();
    // Initialize SdFat or print a detailed error message and halt
    // Use half speed like the native library.
    // Change to SPI_FULL_SPEED for better performance.
    if (!sd.begin(chipSelect, SPI_HALF_SPEED)) 
    {
        sd.initErrorHalt();
    }
    
    // prepare files for writing. O_RDWR enables read-write, O_CREAT makes a new file
    // if there isn't already a file with the name in quotes, O_AT_END adds new data
    // to the end of the file instead of overwriting the contents. 
    if (!globalTemp.open("globalTemp.txt", O_RDWR | O_CREAT | O_AT_END)) 
    {
        sd.errorHalt("SETUP(): opening globalTemp.txt for write failed"); // debug info
    }
    else 
    { 
        globalTemp.close();
    }
    
    if (!globalEKG.open("globalEKG.txt", O_RDWR | O_CREAT | O_AT_END))
    {
        sd.errorHalt("SETUP(): opening globalEKG.txt for write failed"); // debug info
    }
    else 
    { 
        globalEKG.close();
    }
    
    if (!globalAccel.open("globalAccel.txt", O_RDWR | O_CREAT | O_AT_END))
    {
        sd.errorHalt("SETUP(): opening globalAccel.txt for write failed"); // debug info
    }
    else 
    { 
        globalAccel.close();
    }
}

void loop()
{
    
}

void checkTemp()
{
    if (!globalTemp.open("globalTemp.txt", O_RDWR | O_CREAT | O_AT_END)) 
    {
        sd.errorHalt("CHECKTEMP(): opening globalTemp.txt for write failed"); // debug info
    }
    thermistorAnalog = analogRead(A5); // read the raw thermistor analog data

    // convert resistance to ohms using voltage divider equation, where R1 = 10k ohms
    // shielded equation from dividing by zero by putting it in an if statement
    if (thermistorAnalog < 4095)
    {
        thermistorResistance = (9820*thermistorAnalog/4095)/(1-thermistorAnalog/4095);
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
    Serial.print("Writing to globalTemp.txt..."); // serial debug
    globalTemp.print(Time.year()); // prints formatted time
    globalTemp.print(", ");
    globalTemp.print(Time.month());
    globalTemp.print(", ");
    globalTemp.print(Time.day());
    globalTemp.print(", ");
    globalTemp.print(Time.hourFormat12());
    globalTemp.print(", ");
    globalTemp.print(Time.minute());
    globalTemp.print(", ");
    globalTemp.print(Time.second());
    globalTemp.print(", ");
    globalTemp.print(String(floor(temperature*100 + 0.5)/100).remove(5)); // writes temp
    globalTemp.println("; ");
    Particle.publish("newTemp", String(floor(temperature*100 + 0.5)/100).remove(5)); 
    
    globalTemp.close();
}

void checkEKG()
{
    if (!globalEKG.open("globalEKG.txt", O_RDWR | O_CREAT | O_AT_END)) 
    {
        sd.errorHalt("CHECKEKG(): opening globalEKG.txt for write failed"); // debug info
    }
    
    EKG = analogRead(A4); // read the EKG, connected to A4 of the Photon. 
    EKGData += String(EKG); 
    EKGData += ",";
    
    if (EKGData.length() > 250)
    {
        Particle.publish("EKG", EKGData);
        EKGData = ""; // erases the EKG data buffer, starts over
    }
    
    // Serial.print("Writing to globalEKG.txt..."); // serial debug
    globalEKG.print(Time.year()); // prints formatted time
    globalEKG.print(", ");
    globalEKG.print(Time.month());
    globalEKG.print(", ");
    globalEKG.print(Time.day());
    globalEKG.print(", ");
    globalEKG.print(Time.hourFormat12());
    globalEKG.print(", ");
    globalEKG.print(Time.minute());
    globalEKG.print(", ");
    globalEKG.print(Time.second());
    globalEKG.print(", ");
    globalEKG.print(String(EKG)); // writes EKG
    globalEKG.println("; ");
    
    globalEKG.close();
}

void checkAccel()
{
    if (!globalAccel.open("globalAccel.txt", O_RDWR | O_CREAT | O_AT_END)) 
    {
        sd.errorHalt("CHECKACCEL(): opening globalAccel.txt for write failed"); // debug info
    }
    
    // X, Y, and Z are directly wired to A2, A1, and A0 respectively. Rounds data, truncates trailing zeros and decimal. 
    xaccel = analogRead(A0);
    yaccel = analogRead(A1);
    zaccel = analogRead(A2);
    accel = String(floor(sqrt(pow(xaccel,2) + pow(yaccel,2) + pow(zaccel,2)) + 0.5)).remove(4); 
    accelData += accel; 
    accelData += ",";
    
    if (accelData.length() > 250)
    {
        Particle.publish("Acceleration", accelData);
        accelData = ""; // erases the acceleration data buffer, starts over
    }
    
    // Serial.print("Writing to globalAccel.txt..."); // serial debug
    globalAccel.print(Time.year()); // prints formatted time
    globalAccel.print(", ");
    globalAccel.print(Time.month());
    globalAccel.print(", ");
    globalAccel.print(Time.day());
    globalAccel.print(", ");
    globalAccel.print(Time.hourFormat12());
    globalAccel.print(", ");
    globalAccel.print(Time.minute());
    globalAccel.print(", ");
    globalAccel.print(Time.second());
    globalAccel.print(", ");
    globalAccel.print("xaccel: ");
    globalAccel.print(String(xaccel)); // writes x acceleration data
    globalAccel.print(", ");
    globalAccel.print("yaccel: ");
    globalAccel.print(String(yaccel)); // writes y acceleration data
    globalAccel.print(", ");
    globalAccel.print("zaccel: ");
    globalAccel.print(String(zaccel)); // writes z acceleration data
    globalAccel.println("; ");
    
    globalAccel.close();
}
