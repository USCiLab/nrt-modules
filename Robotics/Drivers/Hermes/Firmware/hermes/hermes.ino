#include <Servo.h>
#include <Wire.h>
// #include <ADXL345.h>
#include "HMC5883L.h"
#include "ITG3200.h"
#include "hermes.h"
#include "sensordata.h"

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
	
	long currentTime = 0;
	long updatedTime = 0;
    
  Serial.begin(BAUDRATE);
  Wire.begin();

  LOG("Ready");
}

void loop()
{
	currentTime = millis();
  if (Serial.available() > 0) {
		updatedTime = millis();
		hermesState = ACTIVE;
    dispatch(Serial.read());
  } else {
		hermesState = IDLE;
  }
  sensorUpdate();
	if (currentTime - updatedTime > WATCHDOG_THRESHOLD)	{
		//etc.
	}
}
