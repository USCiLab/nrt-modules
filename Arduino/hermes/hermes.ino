#include <Servo.h>
#include <Wire.h>
// #include <ADXL345.h>
#include "HMC5883L.h"
#include <ITG3200.h>
#include "hermes.h"

HMC5883L compass;
ITG3200 gyro;

void setup()
{
  Left.attach(LEFT_SERVO);
  Right.attach(RIGHT_SERVO);

  compass = HMC5883L();
  gyro = ITG3200();

  Serial.begin(9600);//BAUDRATE);
  Wire.begin();
  
  Serial.println("Setting scale to +/- 1.3 Ga");
    int error = compass.SetScale(1.3); // Set the scale of the compass.
    if(error != 0) // If there is an error, print it out.
      Serial.println(compass.GetErrorText(error));

    Serial.println("Setting measurement mode to continuous.");
    error = compass.SetMeasurementMode(Measurement_Continuous); // Set the measurement mode to Continuous
    if(error != 0) // If there is an error, print it out.
      Serial.println(compass.GetErrorText(error));
    
  LOG("Ready");
}

// void loop()
// {
//   if (Serial.available() > 0)
//   {
//     dispatch(Serial.read());
//   }
//   
//   if(millis() % 1000 == 0){
//     MagnetometerRaw data = magnetometer.ReadRawAxis();
//     // float gyroX, gyroY, gyroZ;
//     // gyro.readGyro(&gyroX, &gyroY, &gyroZ);
//     Serial.println("Data-----");
//     Serial.println(gyroX);
//     Serial.println(gyroY);
//     Serial.println(gyroZ);
//     Serial.println(millis());
//   }
//   
// }

void loop()
{
  // Retrive the raw values from the compass (not scaled).
  MagnetometerRaw raw = compass.ReadRawAxis();
  // Retrived the scaled values from the compass (scaled to the configured scale).
  MagnetometerScaled scaled = compass.ReadScaledAxis();
  
  // Calculate heading when the magnetometer is level, then correct for signs of axis.
  float heading = atan2(raw.YAxis, raw.XAxis);
   
  // Correct for when signs are reversed.
  if(heading < 0)
    heading += 2*PI;
   
  // Convert radians to degrees for readability.
  float headingDegrees = heading * 180/M_PI; 

  // Output the data via the serial port.
  Output(raw, scaled, heading, headingDegrees);
}

void Output(MagnetometerRaw raw, MagnetometerScaled scaled, float heading, float headingDegrees)
{
   Serial.print("Raw:\t");
   Serial.print(raw.XAxis);
   Serial.print("   ");   
   Serial.print(raw.YAxis);
   Serial.print("   ");   
   Serial.print(raw.ZAxis);
   Serial.print("   \tScaled:\t");
   
   Serial.print(scaled.XAxis);
   Serial.print("   ");   
   Serial.print(scaled.YAxis);
   Serial.print("   ");   
   Serial.print(scaled.ZAxis);

   Serial.print("   \tHeading:\t");
   Serial.print(heading);
   Serial.print(" Radians   \t");
   Serial.print(headingDegrees);
   Serial.println(" Degrees   \t");
}

