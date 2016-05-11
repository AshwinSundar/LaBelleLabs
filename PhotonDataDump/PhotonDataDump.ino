//// //// //// //// ////
// Please read - the following section MUST be completed BEFORE attempting to commit
// changes to GitHub.
//// //// //// //// ////
// Title of File: PhotonDataDump.ino
// Name of Editor: Ashwin Sundar
// Date of GitHub commit: May 11, 2016
// What specific changes were made to this code, compared to the currently up-to-date code
// on GitHub?: Instead of storing data as an int or double, store it in a char. Since the
// char can have a max length of 622 bytes, keep appending data until you fill up the buffer,
// transmit the large set of data, erase the buffer and then write to it again. It's my 
// version of a circular buffer. To that end, I've converted EKGRead to a char. I will ex-
// periment with appending data to EKGRead tomorrow hopefully. 
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
// 
// Particle Photon connections to ADXL345
// Photon     <-> ADXL345 Breakout Board
// GND         -  GND
// 3V3         -  VCC
// 3V3         -  CS
// Digital 0   -  SDA
// Digital 1   -  SCL
//
// All of the addresses specified in code for the ADXL345 are given in the datasheet, 
// available here:
// http://www.analog.com/media/en/technical-documentation/data-sheets/ADXL345.pdf

// This #include statement was automatically added by the Particle IDE.
#include "SparkTime/SparkTime.h"

#define DEVICE (0x1D) // Device address as specified in ADXL345 data sheet. Must
// connect CS to ground.

byte _buff[6];

char POWER_CTL = 0x2D;	// Power Control Register
char DATA_FORMAT = 0x31;
char BW_RATE = 0x2C;
char DATAX0 = 0x32;	// X-Axis Data 0
char DATAX1 = 0x33;	// X-Axis Data 1
char DATAY0 = 0x34;	// Y-Axis Data 0
char DATAY1 = 0x35;	// Y-Axis Data 1
char DATAZ0 = 0x36;	// Z-Axis Data 0
char DATAZ1 = 0x37;	// Z-Axis Data 1

// int's in C++ cannot have numbers in their name. Hence the bizarre nomenclature. 
int XAccelA; // x acceleration from the ADXL335
int YAccelA; // y acceleration from the ADXL335
int ZAccelA; // z acceleration from the ADXL335
int VOutA; // 3v0 output of the ADXL335
// 
int XAccelB; // x acceleration from the ADXL345
int YAccelB; // y acceleration from the ADXL345
int ZAccelB; // z acceleration from the ADXL345
double currentTime; // stores the current time. 
char EKGRead; // analog reading of EKG

void setup()
{
 // Let's register some Particle variables to the Particle cloud.
 // This means when we "ask" the Particle cloud for the string in quotes, we will get
 // the value after the comma. Note: The published variable name in quotes AND the
 // actual variable name MUST be 12 characters or less. Otherwise, the cloud will
 // ignore the variable and you won't be able to see it when you make a request.
 Particle.variable("335XAccel", XAccelA);
 Particle.variable("335YAccel", YAccelA);
 Particle.variable("335ZAccel", ZAccelA);
 Particle.variable("335VOut", VOutA);
 Particle.variable("345XAccel", XAccelB);
 Particle.variable("345YAccel", YAccelB);
 Particle.variable("345ZAccel", ZAccelB);
 Particle.variable("Time", currentTime);
 Particle.variable("EKG", EKGRead);
 
 
 Wire.begin(); // join i2c bus (address optional for master)
 // Register 0x31 is Data_Format. See datasheet for more details.
 // Justification in page 59 of lab notebook 2645 - Ashwin Sundar
 // Self-test = 1 --> Applies a self-test force to the sensor, causing
 // a shift in the data.
 // SPI = 1 --> initialize 3-wire SPI mode.
 // Int_Invert = 0;
 // Full_Res = 1 --> Allows resolution to match the range
 // D4 = 0 --> unused
 // Justify = 0 --> Enable LSB
 // Range =  01 --> Enable +/- 4G mode
 // Result: 11001001 = 0xC9
 writeTo(DATA_FORMAT, 0xC9);
 // Register 0x2C is BW_Rate (Sampling Rate). See datasheet for more details.
 // Justification in page 59 of lab notebook 2645 - Ashwin Sundar
 // D7 = 0 --> unused
 // D6 = 0 --> unused
 // D5 = 0 --> unused
 // Low_Power = 0 --> Selects standard operation.
 // Rate = 0111 --> Select 12.5Hz mode.
 // Result: 00000111 = 0x07
 writeTo(BW_RATE, 0x07);
// Register 0x2D is Power Control. See datasheet for more details.
// Justification in page 59 of lab notebook 2645 - Ashwin Sundar
// Link = 0 --> disable activity/inactivity feature
// Auto_Sleep =  0 --> disable auto sleep when inactivity is detected
// Measure = 1 --> Enable measurement mode
// Sleep = 0 --> Keep device in normal operation mode
// Wakeup = 00 --> In sleep mode, device will make 8 readings per second. I don't
// think this actually matters for now, since I'm preventing the device from sleeping
// anyway. I'll use the max of 8 Hz anyway just to be safe.
 writeTo(POWER_CTL, 0x08);


 // Serial is solely for debugging.
 Serial.begin(230400);
}

void loop()
{
 // read the ADXL345 acceleration using Wire.
 readAccel();
 
 // read the raw ADXL335 acceleration using analogRead().
 XAccelA = analogRead(A2);
 YAccelA = analogRead(A1);
 ZAccelA = analogRead(A0);
 
 // determine the reference voltage by querying VOut on A3
 VOutA = analogRead(A3);
 
 // read the EKG, connected to A4 of the Photon. 
 while (sizeof(EKGRead) < 621) {
     EKGRead += ' ';
     EKGRead += analogRead(A4);
 } 
 
 if (sizeof(EKGRead) > 621) {
     EKGRead = analogRead(A4);
 }
 
 // debugging information. Must have micro USB plugged in. Type "particle serial
 // monitor" into Terminal to begin Serial monitor.
 currentTime = micros()/1000000.000;
 Serial.print(currentTime);
 Serial.print("s; ");
 Serial.println(EKGRead);
//  Serial.print("335XAccel: ");
//  Serial.print(XAccelA);
//  Serial.print(" 335YAccel: ");
//  Serial.print(YAccelA);
//  Serial.print(" 335ZAccel: ");
//  Serial.print(ZAccelA);
//  Serial.print(" 345XAccel: ");
//  Serial.print(XAccelB);
//  Serial.print(" 345YAccel: ");
//  Serial.print(YAccelB);
//  Serial.print(" 345ZAccel: ");
//  Serial.println(ZAccelB);
 
}

void readAccel() {
 uint8_t howManyBytesToRead = 6;
 readFrom( DATAX0, howManyBytesToRead, _buff); // read the acceleration data from the ADXL345

 // each axis reading comes in 10 bit resolution, ie 2 bytes.  Least Significant Byte first.
 // thus we are converting both bytes into a single int, for each axis.
 XAccelB = (((int)_buff[1]) << 8) | _buff[0];
 YAccelB = (((int)_buff[3]) << 8) | _buff[2];
 ZAccelB = (((int)_buff[5]) << 8) | _buff[4];
}

void writeTo(byte address, byte val) {
 Wire.beginTransmission(DEVICE); // start transmission to device
 Wire.write(address);             // send register address
 Wire.write(val);                 // send value to write
 Wire.endTransmission();         // end transmission
}

// Reads num bytes starting from address register on device in to _buff array
void readFrom(byte address, int num, byte _buff[]) {
 Wire.beginTransmission(DEVICE); // start transmission to device
 Wire.write(address);             // sends address to read from
 Wire.endTransmission();         // end transmission

 Wire.beginTransmission(DEVICE); // start transmission to device
 Wire.requestFrom(DEVICE, num);    // request 6 bytes from device

 int i = 0;
 while(Wire.available())         // device may send less than requested (abnormal)
 {
   _buff[i] = Wire.read();    // receive a byte
   i++;
 }
 Wire.endTransmission();         // end transmission
}

