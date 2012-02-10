#include <Servo.h>
#include <Wire.h>
// #include <ADXL345.h>
#include "HMC5883L.h"
#include "ITG3200.h"
#include "hermes.h"

HMC5883L magnetometer;
ITG3200 gyro;

void setup()
{
  // Attach servos
  Left.attach(LEFT_SERVO);
  Right.attach(RIGHT_SERVO);

  // IMU
  magnetometer = HMC5883L();
  magnetometer.SetMeasurementMode(Measurement_Continuous);     
  
  gyro = ITG3200();
  gyro.reset();
  gyro.init(ITG3200_ADDR_AD0_LOW);
  // gyro.zeroCalibrate(2500,2); // samples,seconds

  Serial.begin(BAUDRATE);
  Wire.begin();

  LOG("Ready");
}

void loop()
{
  if (Serial.available() > 0)
  {
    dispatch(Serial.read());
  }
  
  sensorUpdate();  
}