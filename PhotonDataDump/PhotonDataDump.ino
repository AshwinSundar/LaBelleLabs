//// //// //// //// ////
// Please read - the following section MUST be completed BEFORE attempting to commit
// changes to GitHub.
//// //// //// //// ////
// Title of File: PhotonDataDump.ino
// Name of Editor: Ashwin Sundar
// Date of GitHub commit: August 12, 2016
// What specific changes were made to this code, compared to the currently up-to-date code
// on GitHub?: The second round of SD testing confirmed my guess - the larger the file, the
// longer it takes to write to the file. I started at 5.96 ms per write, ended at 8.29 ms
// after about 18 hours of continuous writes spaced by 100ms. Now, I'm going to create a 
// Cloud uptime test. I want to see how much uptime I have on each Photon. 
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

SYSTEM_MODE(SEMI_AUTOMATIC); // allows Photon code to execute without being connected
// to a WiFi network. You must manually call Particle.connect(). In AUTOMATIC mode, 
// Particle.connect() is automatically called before any code is executed, and the 
// device waits to be connected to WiFi before executing any of your code. 

Timer WiFiTimer(1000, checkCloud);

void setup() {
    Particle.connect(); // must manually called Particle.connect() if system_mode is 
    // semi_automatic
    Serial.begin(115200);
    WiFiTimer.start();
}

void loop() {
    

}

void checkCloud() {
    Serial.print(millis()/1000); // prints time
    Serial.print(" ");
    Serial.print(Particle.connected()); // if Photon is connected to Cloud, returns 1. 
    Serial.println(";");
   
}


