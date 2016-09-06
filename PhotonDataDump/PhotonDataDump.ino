#include <SdFat/SdFat.h> // Particle IDE is strange. To include this particular library, 
// you have to both declare it here, as well as manually add the library to the app
// over in the libraries link on the left sidebar. 
#include <math.h>
//// //// //// //// ////
// Please read - the following section MUST be completed BEFORE attempting to commit
//// //// //// //// ////
// Title of File: PhotonDataDump.ino
// Name of Editor: Ashwin Sundar
// Date of GitHub commit: September 6, 2016
// What specific changes were made to this code, compared to the currently up-to-date code
// on GitHub?: Manually setting time using setTime and inputting Unix Epoch Time. This is
// the number of seconds since 1 January 1970. I'm doing this for our special case where
// we don't have internet access during testing in ECB. We may want to consider using this 
// as well in actual device deployment, since the device may be powered on in the absence of 
// an internet connection. Additionally, I made a minor modification to how data is written
// to the card, to make it easier to plot later. Finally, I fixed the inaccurate temperature
// data. I implemented the voltage divider equation wrong - I had it set up such that the
// thermistor was wired to ground, not the 10k resistor. Appears to be functioning fine now -
// readings are far more reasonable. Yet to be verified. 
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
// Particle Photon connections to voltage divider
// 3.3V <-> thermistor <-> A5
//              ^          ^  
//              |----------|
//              v
//         10k resistor
//              ^
//              |
//              v
//             GND
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
FatFile globalTempFile;
FatFile globalEKGFile;
FatFile globalAccelFile;
String mockData = "gibberish gibberish gibberish gibberish gibberish gibberish gibberish gibberish gibberish gibberish ";
String fileName;
String globalTempFileName;
String globalEKGFileName;
String globalAccelFileName;
String EKGData; 
String accelData; 
int cardDetect = D6; // determines if a card is inserted or not. Doesn't work reliably. 
int EKG = 0;
bool flag; // flag checks whether certain events were successful or not
double accelMagnitude; 
double start;
double stop;
double thermistorAnalog;
double temperature; 
double R1 = 10000; // 10k ohm resistor. measure true resistance with multimeter and change as needed. 
double thermistorResistance;
int currentFileSize; 
int fileTracker = 1; 
int globalTempFileTracker = 1; 
int globalEKGFileTracker = 1;
int globalAccelFileTracker = 1;

void checkCloud() {
    Serial.print(millis()/1000); // prints time
    Serial.print(" ");
    Serial.print(Particle.connected()); // if Photon is connected to Cloud, returns 1. 
    Serial.println(";");
   
}

// int checkFileSize(int fileTracker) {
//     Serial.print("File Size: ");
//     currentFileSize = (testFile.fileSize());
//     Serial.println(currentFileSize);
    
//     if (testFile.fileSize() > 1000000) // 1 MB
//     {
//         return fileTracker++; // move to the next file
//     }
    
//     else {
//         return fileTracker; 
//     }
// }

void checkAccel() {
    globalAccelFileName = "globalAccel" + String(globalAccelFileTracker) + ".txt";
    globalAccelFile.open(globalAccelFileName, O_RDWR | O_CREAT | O_AT_END);
    
    accelMagnitude = floor(sqrt(pow(analogRead(A0),2) + pow(analogRead(A1),2) + pow(analogRead(A2),2)) + 0.5); 
    accelData += String(accelMagnitude).remove(4); 
    
        // maximum String length is 622 bytes. Let's just be safe and say 616. 
    if (accelData.length() > 616) {
        accelData += " " + Time.timeStr(); 
        globalAccelFile.write(accelData);
        accelData = "";
    }
    
    if (accelData.length() <= 616) { 
        accelData += "\n"; // each data point is on its own line
    }
    
    if (globalAccelFile.fileSize() > 1000000) // if the accel file exceeds 1MB, on to the next one
    {
        globalAccelFileTracker++;
    }
    
    globalAccelFile.close();
}

void checkCurrentTime() { 
    Serial.println("current time: " + String(Time.now()));
}

void checkEKG() {
    globalEKGFileName = "globalEKG" + String(globalEKGFileTracker) + ".txt";
    globalEKGFile.open(globalEKGFileName, O_RDWR | O_CREAT | O_AT_END);
    
    // not printing error messages here - sample rate of 200Hz will flood the serial monitor
    
    EKG = analogRead(A4);
    EKGData += String(EKG);
    
    // maximum String length is 622 bytes. Let's just be safe and say 616. 
    if (EKGData.length() > 616) {
        EKGData += " " + Time.timeStr(); // print the time in the next column 
        globalEKGFile.write(EKGData);
        EKGData = "";
    }
    
    // maximum String length is 622 bytes. Let's just be safe and say 616. 
    if (EKGData.length() <= 616) { 
        EKGData += "\n"; // each data point is on its own line
    }
    
    if (globalEKGFile.fileSize() > 1000000) // if the EKG file exceeds 1MB, on to the next on on to the next one
    {
        globalEKGFileTracker++;   
    }
    
    // once again, no error messages because I'll flood the serial monitor. Maybe implement a flashing LED? 
    globalEKGFile.close();
}

void checkForErrors() {
    // if (sd.cardErrorCode) {
    //     digitalWrite(D7, HIGH);
    // }
    
    // else {
    //     digitalWrite(D7, LOW);
    // }
}

void checkTemp() {
    globalTempFileName = "globalTemp" + String(globalTempFileTracker) + ".txt";
    flag = globalTempFile.open(globalTempFileName, O_RDWR | O_CREAT | O_AT_END); 
    
    if (flag == 0) { 
        Serial.println("CHECKTEMP(): " + globalTempFileName + " O_RDWR | O_CREAT | O_AT_END failed.");
    }
    
    if (flag == 1) { 
        Serial.println("CHECKTEMP(): " + globalTempFileName + " opened successfully.");
    }
    
    thermistorAnalog = analogRead(A5); // read the raw thermistor analog data
    
     // convert resistance to ohms using voltage divider equation, where R1 = 10k ohms
     // shielded equation from divide by 0 by putting it inside an if statement
    if (thermistorAnalog < 4095) { 
        thermistorResistance = R1*(1-thermistorAnalog/4095)*(4095/thermistorAnalog);
    }
    
    else if (thermistorAnalog >= 4095) {
        // if A5 reads 4095, then there is a short and/or the thermistor has failed
        thermistorResistance = 0;
    }
    
    // calculated temperature by plotting a line of best fit using linear algebra and MATLAB. Error was found to be 3.7998*10^-4 Kelvins 
    // using Igor Pro. Compressed some of the math operations to reduce floating point error. 
    temperature = pow((.0011106 + 0.00023724*log(thermistorResistance) + 0.000000074738*pow(log(thermistorResistance), 3)), -1) - 273.15;
    
    globalTempFile.write(Time.timeStr() + " " + temperature + "; ");
    globalTempFile.write("thermistor Resistance: " + String(thermistorResistance) + ";\n");
    
    // if the file size exceeds 1MB, on to the next one
    if (globalTempFile.fileSize() > 1000000) { 
        globalTempFileTracker++;
    }
    
    flag = globalTempFile.close();
    if (flag == 0) {
        Serial.println("CHECKTEMP(): " + globalTempFileName + " failed to close.");
    }
    
    if (flag == 1) { 
        Serial.println("CHECKTEMP(): " + globalTempFileName + " closed successfully.");
    }
}

void writeBulkData() {
    
    // Test: Open a file. 
    flag = testFile.open(fileName, O_RDWR | O_CREAT | O_AT_END);
    Serial.println(flag);
    if (flag == 0) {
        Serial.println("WRITEBULKDATA(): " + fileName + " O_RDWR | O_CREAT | O_AT_END failed.");
    }
    
    if (flag == 1) {
        Serial.println("WRITEBULKDATA(): " + fileName + " opened successfully.");
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
    }
    
    if (flag == 1) {
        Serial.println("WRITEBULKDATA(): " + fileName + " closed successfully.");
    }
    
    // sd.errorPrint(); // these are currently private functions. waiting for Particle to rescan GitHub
    // sd.cardErrorCode();
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

// Timer WiFiTimer(1000, checkCloud);
// Timer WriteTimer(100, writeBulkData);
Timer CheckTempTimer(5000, checkTemp); // sample temp every 5s
Timer CheckEKGTimer(5, checkEKG); // sample EKG at 200 Hz
Timer CheckAccelTimer(50, checkAccel); // sample accelerometer at 20 Hz
Timer CheckForErrorsTimer(5000, checkForErrors);
Timer CheckTimeTimer(1000, checkCurrentTime);

void setup() {
    // Particle.connect(); // must manually call Particle.connect() if system_mode is 
    // semi_automatic
    // Time.zone(-7); // changes time zone. Does not adjust for DST, must manually change.
    Serial.begin(115200); // debugging purposes
    sd.begin(chipSelect, SPI_FULL_SPEED); // init at full speed for best performance
    pinMode(D7, OUTPUT); // initalize D7 as an output
    Time.setTime(1473379200); // set time to start on September 9 2016 00:00:00 GMT
    // fileName = "testFile" + String(fileTracker) + ".txt";
    
    // // Test: Open a file. 
    // flag = testFile.open(fileName, O_RDWR | O_CREAT | O_AT_END);
    // if (!flag) {
    //     Serial.println("SETUP(): " + fileName + " O_RDWR | 0_CREAT | O_AT_END failed");
    // }
    
    // if (flag) {
    //     Serial.println("SETUP(): " + fileName + " opened successfully.");
    // }

    // // Test: Close a file.
    // flag = testFile.close();
    // if (!flag) {
    //     Serial.println("SETUP(): " + fileName + " failed to close.");
    // }
    
    // if (flag) {
    //     Serial.println("SETUP(): " + fileName + " closed successfully.");
    // }
    
    CheckTempTimer.start();
    CheckEKGTimer.start();
    CheckAccelTimer.start();
    CheckForErrorsTimer.start();
    CheckTimeTimer.start();
}

void loop() { 

} 
