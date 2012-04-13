#include <Servo.h>
#include <Wire.h>
// #include <ADXL345.h>
#include "HMC5883L.h"
#include "ITG3200.h"
#include "hermes.h"
#include "sensordata.h"

void setup()
{
  // Attach servos
  Left.attach(LEFT_SERVO);
  Right.attach(RIGHT_SERVO);
    
  Serial.begin(BAUDRATE);
  Wire.begin();
  
  sensorSetup();
  
  // Power computer
  digitalWrite(DIGITAL_RELAY, HIGH);
}

void loop()
{
  if (Serial.available() > 0) {
    dispatch(Serial.read());
  } else {
    
  }
  
  sensorUpdate();
}
