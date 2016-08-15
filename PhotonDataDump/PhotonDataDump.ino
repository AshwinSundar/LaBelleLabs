// This #include statement was automatically added by the Particle IDE.
#include "SdFat/SdFat.h"

//// //// //// //// ////
// Please read - the following section MUST be completed BEFORE attempting to commit
// changes to GitHub.
//// //// //// //// ////
// Title of File: PhotonDataDump.ino
// Name of Editor: Ashwin Sundar
// Date of GitHub commit: August 15, 2016
// What specific changes were made to this code, compared to the currently up-to-date code
// on GitHub?: I want to implement the changes informed by the testing last week. First, I
// will write bulk data to the card. Second, I will partition the data into files no larger
// than 1MB each. Finally, in the event of a write error, I will have the Photon flash the 
// main LED yellow and red. Could not get these functions up and running today. Will try 
// again tomorrow. 
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
// Photon     <->   microSD board
// N/A         -    CD
// D3          -    D0
// GND         -    GND
// D4          -    SCK
// 3V3         -    VCC
// D2          -    DI
// D1          -    CS
//// //// //// //// ////
//// //// //// //// ////
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

Timer WiFiTimer(1000, checkCloud);
// Timer SDStatus(5000, checkSDStatus);
Timer WriteTimer(250, writeBulkData);
File testFile; 
int cardDetect = D6; 
bool flag; // flag checks whether certain events were successful or not

// mock data is 250 chars long. Writing 4 times a second is 1000 chars per second. 
String mockData = "gibberish gibberish gibberish gibberish gibberish gibberish gibberish gibberish gibberish gibberish gibberish gibberish gibberish gibberish gibberish gibberish gibberish gibberish gibberish gibberish gibberish gibberish gibberish gibberish gibberish ";

void setup() {
    Particle.connect(); // must manually call Particle.connect() if system_mode is 
    // semi_automatic
    Serial.begin(115200); // debugging purposes
    // SDStatus.start();
    // pinMode(cardDetect, INPUT);
    pinMode(D7, OUTPUT);
    
    // Test: Open a file. 
    flag = testFile.open("testFile.txt", O_RDWR | O_CREAT | O_AT_END);
    if (!flag) {
        Serial.println("SETUP(): testFile.txt O_RDWR | 0_CREAT | O_AT_END failed");
        digitalWrite(D7, HIGH);
    }
    
    if (flag) {
        Serial.println("SETUP(): testFile.txt opened successfully.");
        digitalWrite(D7, LOW);
    }

    // Test: Close a file.
    flag = testFile.close();
    if (!flag) {
        Serial.println("SETUP(): testFile.txt failed to close.");
        digitalWrite(D7, HIGH);
    }
    
    if (flag) {
        Serial.println("SETUP(): testFile.txt closed successfully.");
        digitalWrite(D7, LOW);
    }
    
    WriteTimer.start();
}

void loop() {

}

void checkCloud() {
    Serial.print(millis()/1000); // prints time
    Serial.print(" ");
    Serial.print(Particle.connected()); // if Photon is connected to Cloud, returns 1. 
    Serial.println(";");
   
}

void writeBulkData() {
    
    // Test: Open a file. 
    
    flag = testFile.open("testFile.txt", O_RDWR | O_AT_END);
    Serial.println(flag);
    if (flag == 0) {
        Serial.println("WRITEBULKDATA(): testFile.txt O_RDWR | 0_CREAT | O_AT_END failed.");
        digitalWrite(D7, HIGH);
    }
    
    if (flag == 1) {
        Serial.println("WRITEBULKDATA(): testFile.txt opened successfully.");
        digitalWrite(D7, LOW);
    }

    testFile.print(mockData);
    
    // Test: Close a file.
    flag = testFile.close();
    if (flag == 0) {
        Serial.println("WRITEBULKDATA(): testFile.txt failed to close.");
        digitalWrite(D7, HIGH);
    }
    
    if (flag == 1) {
        Serial.println("WRITEBULKDATA(): testFile.txt closed successfully.");
        digitalWrite(D7, LOW);
    }
}

// doesn't really work that well. 
// void checkSDStatus() {
//     Serial.println(digitalRead(cardDetect));
//     if (digitalRead(cardDetect) == 0) {
//         Serial.println("SD disconnected. Please plug in an SD card.");
//         digitalWrite(D7, HIGH);
//     }
    
//     if (digitalRead(cardDetect) == 1) {
//         Serial.println("SD connected.");
//         digitalWrite(D7, LOW);
//     }



