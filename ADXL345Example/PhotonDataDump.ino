//// //// //// //// ////
// Please read - the following section MUST be completed BEFORE attempting to commit
// changes to GitHub.
//// //// //// //// ////
// Title of File: PhotonDataDump.ino
// Name of Editor: Ashwin Sundar
// Date of GitHub commit: April 18, 2016
// What specific changes were made to this code, compared to the currently up-to-date code
// on GitHub?: Attempting to troubleshoot accelerometer. Getting all 0's when I connect
// to Photon over Serial. Also added Particle.publish() functionality so a user can
// subscribe to events.  
// ---------
// As an example, this is valid:
// Particle.publish("XAccel", myXAccel);
// But this is not valid:
// Particle.publish("myXAcceleration", myXAccel);
// And neither is this:
// Particle.publish("myXAccel", myXAcceleration);
// ---------
// The invalid examples exceed 12 characters either in the variable name published in quotes,
// or in the actual variable name. These will cause variable requests to mysteriously return
// 0 variables when you query the cloud for the number of variables available on the device.
//// //// //// //// ////
// Best coding practices
// 1) When you create a new variable or function, make it obvious what the variable or
// function does. The name of the variable or function should reflect its purpose.
// 2) Comment copiously. Created a new variable or function? Explain why. Explain what
// your functions do. Feel free to link to sites that you referenced for information.
// 3) Please compile your code and make sure it is functional BEFORE committing changes to GitHub.
//// //// //// //// ////
//// //// //// //// ////

// Particle Photon connections to ADXL345
// Photon     <-> ADXL345 Breakout Board
// Gnd         -  GND
// 3.3v        -  VCC
// GND         -  CS
// Digital 0   -  SDA
// Digital 1   -  SCL
//
// All of the addresses specified below are given in the datasheet, available here:
// http://www.analog.com/media/en/technical-documentation/data-sheets/ADXL345.pdf

#define DEVICE (0x53) // Device address as specified in ADXL345 data sheet. Must
// connect CS to ground.

byte _buff[6];

char POWER_CTL = 0x2D;	// Power Control Register
char DATA_FORMAT = 0x31;
char DATAX0 = 0x32;	// X-Axis Data 0
char DATAX1 = 0x33;	// X-Axis Data 1
char DATAY0 = 0x34;	// Y-Axis Data 0
char DATAY1 = 0x35;	// Y-Axis Data 1
char DATAZ0 = 0x36;	// Z-Axis Data 0
char DATAZ1 = 0x37;	// Z-Axis Data 1

void setup()
{
  int XAccel; // x acceleration from the ADXL345
  int YAccel; // y acceleration from the ADXL345
  int ZAccel; // z acceleration from the ADXL345

 // Let's register some Particle variables to the Particle cloud.
 // This means when we "ask" the Particle cloud for the string in quotes, we will get
 // the value after the comma. Note: The published variable name in quotes AND the
 // actual variable name MUST be 12 characters or less. Otherwise, the cloud will
 // ignore the variable and you won't be able to see it when you make a request.
 Particle.variable("XAccel", XAccel);
 Particle.variable("YAccel", YAccel);
 Particle.variable("ZAccel", ZAccel);

 Wire.begin(); // join i2c bus (address optional for master)
 //Put the ADXL345 into +/- 4G range by writing the value 0x01 to the DATA_FORMAT register.
 writeTo(DATA_FORMAT, 0x01);
 //Put the ADXL345 into Measurement Mode by writing 0x08 to the POWER_CTL register.
 writeTo(POWER_CTL, 0x08);

 // Serial is solely for debugging.
 Serial.begin(9600);
}

void loop()
{
 readAccel();
 delay(1000); // how quickly do you want to read data?
 // I'm going to publish these variables, and on the Terminal side subscribe to
 // these events
 Particle.publish("XAccel", XAccel);
 Particle.publish("YAccel", YAccel);
 Particle.publish("ZAccel", ZAccel);
 // debugging information. Must have micro USB plugged in. Type "particle serial
 // monitor" into Terminal to begin Serial monitor.
 Serial.print("x: ");
 Serial.print(XAccel);
 Serial.print(" y: ");
 Serial.print(YAccel);
 Serial.print(" z: ");
 Serial.println(ZAccel);
}

void readAccel() {
 uint8_t howManyBytesToRead = 6;
 readFrom( DATAX0, howManyBytesToRead, _buff); // read the acceleration data from the ADXL345

 // each axis reading comes in 10 bit resolution, ie 2 bytes.  Least Significant Byte first.
 // thus we are converting both bytes into a single int, for each axis.
 XAccel = (((int)_buff[1]) << 8) | _buff[0];
 YAccel = (((int)_buff[3]) << 8) | _buff[2];
 ZAccel = (((int)_buff[5]) << 8) | _buff[4];
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
