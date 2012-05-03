#ifndef _HERMES_H
#define _HERMES_H
#include "hermesGlobals.h"

// bookkeeping vars
CircularBuffer<5> buffer;
unsigned long watchdog;

// motors
Servo leftMotor;
Servo rightMotor;

// sensors
HMC5883L magnetometer;
MagnetometerRaw magnetometerRaw;
ITG3200 gyro;

void sendResponse(packetid id, float value)
{
  Serial.write(255);
  Serial.write((byte)id);
  byte checksum = 255 ^ (byte)id;
  ResponsePacket response;
  response.data = value;
  for(int i=0; i<4; ++i)
  {
    checksum ^= response.raw[i];
    Serial.write(response.raw[i]);
  }
  Serial.write(checksum);
}

void setMotors(unsigned char left, unsigned char right)
{
  leftMotor.writeMicroseconds(map(left, 0, 256, 1000, 2000)-MOTOR_PWM_OFFSET);
  rightMotor.writeMicroseconds(map(right, 0, 256, 1000, 2000)-MOTOR_PWM_OFFSET);
}

#endif // _HERMES_H
