//// //// //// //// ////
// Please read - the following section MUST be completed BEFORE attempting to commit
// changes to GitHub.
//// //// //// //// ////
// Title of File: PhotonDataDump.ino
// Name of Editor: Ashwin Sundar
// Date of GitHub commit: May 16, 2016
// What specific changes were made to this code, compared to the currently up-to-date code
// on GitHub?: Removed all extraneous variables, goal is to start from a clean slate. Added
// in 3 analog read variables just to read data from the ADXL335. 
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

// int's in C++ cannot have numbers in their name. Hence the bizarre nomenclature. 
int XAccel; // x acceleration from the ADXL335
int YAccel; // y acceleration from the ADXL335
int ZAccel; // z acceleration from the ADXL335
int VOut; // 3v0 output of the ADXL335
// 
int EKG; // analog reading of EKG

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
}

void loop()
{
 // read the raw ADXL335 acceleration using analogRead().
 XAccel = analogRead(A2);
 YAccel = analogRead(A1);
 ZAccel = analogRead(A0);
 
 // determine the reference voltage by querying VOut on A3
 VOut = analogRead(A3);
 
 
 // read the EKG, connected to A4 of the Photon. 
 EKG = analogRead(A4);
}

