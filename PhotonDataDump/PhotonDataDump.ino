// This #include statement was automatically added by the Particle IDE.
#include "SdFat/SdFat.h"

//// //// //// //// ////
// Please read - the following section MUST be completed BEFORE attempting to commit
//// //// //// //// ////
// Title of File: PhotonDataDump.ino
// Name of Editor: Ashwin Sundar
// Date of GitHub commit: August 23, 2016
// What specific changes were made to this code, compared to the currently up-to-date code
// on GitHub?: I asked Bill to reimport SdFat, and he said the Particle IDE should rescan
// GitHub automatically soon and it will update. While I'm waiting for that to happen,
// I'm going to run a pair of SD performance tests. 
// Test 1 (current): I will write "bulk" data every 100ms and observe the time to write over a course of 3 hours. 
// Test 2: Add file partioning. 
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

FatFile testFile; 
String mockData = "gibberish gibberish gibberish gibberish gibberish gibberish gibberish gibberish gibberish gibberish ";
String fileName;
int cardDetect = D6; // determines if a card is inserted or not. Doesn't work reliably. 
bool flag; // flag checks whether certain events were successful or not
double start;
double stop;
int currentFileSize; 
int fileTracker = 1; 

void checkCloud() {
    Serial.print(millis()/1000); // prints time
    Serial.print(" ");
    Serial.print(Particle.connected()); // if Photon is connected to Cloud, returns 1. 
    Serial.println(";");
   
}

void writeBulkData() {
    
    // Test: Open a file. 
    flag = testFile.open(fileName, O_RDWR | O_CREAT | O_AT_END);
    Serial.println(flag);
    if (flag == 0) {
        Serial.println("WRITEBULKDATA(): " + fileName + " O_RDWR | 0_CREAT | O_AT_END failed.");
        digitalWrite(D7, HIGH);
    }
    
    if (flag == 1) {
        Serial.println("WRITEBULKDATA(): " + fileName + " opened successfully.");
        digitalWrite(D7, LOW);
    }
    
    // SD Performance Test
    start = micros(); 
    testFile.write(mockData);
    stop = micros(); 
    Serial.print("Write Time: ");
    Serial.print((stop-start)/1000);
    Serial.println(" ms");
    
    // Test: Close a file.
    flag = testFile.close();
    if (flag == 0) {
        Serial.println("WRITEBULKDATA(): " + fileName + " failed to close.");
        digitalWrite(D7, HIGH);
    }
    
    if (flag == 1) {
        Serial.println("WRITEBULKDATA(): " + fileName + " closed successfully.");
        digitalWrite(D7, LOW);
    }
    
    // sd.errorPrint(); // these are currently private functions. waiting for Particle to rescan GitHub
    // sd.cardErrorCode();
}

bool checkFileSize() {
    Serial.print("File Size: ");
    currentFileSize = (testFile.fileSize());
    Serial.println(currentFileSize);
    
    if (testFile.fileSize() > 1000000) // 1 MB
    {
        fileTracker++; // move to the next file
        fileName = "testFile" + String(fileTracker) + ".txt";
        return TRUE;
    }
    
    else {
        return FALSE; 
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

Timer WiFiTimer(1000, checkCloud);
Timer WriteTimer(100, writeBulkData);
Timer FileSizeTimer(5000, checkFileSize);

void setup() {
    Particle.connect(); // must manually call Particle.connect() if system_mode is 
    // semi_automatic
    Serial.begin(115200); // debugging purposes
    sd.begin(chipSelect, SPI_FULL_SPEED); // init at full speed for best performance
    fileName = "testFile" + String(fileTracker) + ".txt";
    
    // Test: Open a file. 
    flag = testFile.open(fileName, O_RDWR | O_CREAT | O_AT_END);
    if (!flag) {
        Serial.println("SETUP(): " + fileName + " O_RDWR | 0_CREAT | O_AT_END failed");
        digitalWrite(D7, HIGH);
    }
    
    if (flag) {
        Serial.println("SETUP(): " + fileName + " opened successfully.");
        digitalWrite(D7, LOW);
    }

    // Test: Close a file.
    flag = testFile.close();
    if (!flag) {
        Serial.println("SETUP(): " + fileName + " failed to close.");
        digitalWrite(D7, HIGH);
    }
    
    if (flag) {
        Serial.println("SETUP(): " + fileName + " closed successfully.");
        digitalWrite(D7, LOW);
    }
    
    WriteTimer.start();
    FileSizeTimer.start();
}

void loop() { 

} 
