//// //// //// //// ////
// Please read - the following section MUST be completed BEFORE attempting to commit
// changes to GitHub.
//// //// //// //// ////
// Title of File: PhotonDataDump.ino
// Name of Editor: Ashwin Sundar
// Date of GitHub commit: August 10, 2016
// What specific changes were made to this code, compared to the currently up-to-date code
// on GitHub?: Fully rewrote the code from scratch, unit testing along the way. I'm doing
// this because I can't figure out what's wrong with the July 6 code that is creating. Unit
// testing the writeToSD() function tonight.
// strange hardware faults and SD errors. 
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
// Photon     <->   ADXL335 Breakout Board
// 3v3         -    Vin
// 3Vo         -    A3
// GND         -    GND
// ZOut        -    A0
// YOut        -    A1
// XOut        -    A2
// Test/ST     -    N/A
//// //// //// //// ////
//// //// //// //// ////
// Particle Photon connections to microSD breakout board
// Photon     <->   ADXL335 Breakout Board
// N/A         -    CD
// D3          -    D0
// GND         -    GND
// D4          -    SCK
// 3V3         -    VCC
// D2          -    DI
// D1          -    CS
//// //// //// //// ////
//// //// //// //// ////

// This #include statement was automatically added by the Particle IDE.
#include "SdFat/SdFat.h"
#include "math.h"

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
SYSTEM_MODE(SEMI_AUTOMATIC); // allows Photon code to execute without being connected
// to a WiFi network. You must manually call Particle.connect(). In AUTOMATIC mode, 
// Particle.connect() is automatically called before any code is executed, and the 
// device waits to be connected to WiFi before executing any of your code. 

double start;
double stop;
File testFile; 
void setup()
{
    Particle.connect();
    Serial.begin(115200);
    // RGB.control(true);
    // RGB.color(255, 255, 0);
    // Initialize SdFat or print a detailed error message and halt
    // Use half speed like the native library.
    // Change to SPI_FULL_SPEED for better performance.
    start = micros();
    if (!sd.begin(chipSelect, SPI_FULL_SPEED)) 
    {
        sd.initErrorHalt();
    }
    
    // prepare files for writing. O_RDWR enables read-write, O_CREAT makes a new file
    // if there isn't already a file with the name in quotes, O_AT_END adds new data
    // to the end of the file instead of overwriting the contents. 
    if (testFile.open("testFile.txt", O_RDWR | O_CREAT | O_AT_END)) 
    {
        Serial.print("testFile.txt opened successfully.");
    }
    
    else 
    { 
        Serial.print("setup(): opening testFile.txt for 0_RDWR | 0_CREAT | 0_AT_END.");
    }

    testFile.close(); 
    
    stop = micros(); 
    
    Serial.print("Time to execute setup(): " );
    Serial.print((stop - start)/1000);
    Serial.print(" milliseconds; ");
}

void loop()
{
    writeToFile();
}

void writeToFile() {
    start = micros();
    if (testFile.open("testFile.txt", O_RDWR | O_AT_END)) 
    {
        Serial.print("Opening testFile.txt succeeded. "); // debug info
        testFile.print("gibberish");
        stop = micros();
        Serial.print("Time to execute open and print: " );
        Serial.print((stop - start)/1000); 
        Serial.println(" milliseconds;");
    }
    
    else
    { 
        Serial.print("writeToFile(): opening testFile.txt for 0_RDWR | 0_AT_END failed. ");
        
    }
    
    testFile.close();
}


