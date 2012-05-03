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
MedianFilter battery(10);

// helper function for packets and packet ID definitions
template<class PacketT>
byte packet_id();

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

//template<> byte packet_id<MotorPacket>()   { return ID_MOTOR; }
//template<> byte packet_id<CompassPacket>() { return ID_COMPASS; }
//template<> byte packet_id<GyroPacket>()    { return ID_GYRO; }
//template<> byte packet_id<BatteryPacket>() { return ID_BATTERY; }
//
//template<class PacketT>
//void sendPacket(PacketT const& packet)
//{
//  byte packetId = packet_id<PacketT>();
//
//  Serial.write(255);
//  Serial.write(packetId);
//
//  byte checksum = 255 ^ packetId;
//  for (int i = 0; i < sizeof(PacketT); i++)
//  {
//    checksum ^= packet.raw[i];
//    Serial.write(packet.raw[i]);
//  }
//  Serial.write(checksum);
//  Serial.flush();
//}

void setMotors(unsigned char left, unsigned char right)
{
  leftMotor.writeMicroseconds(map(left, 0, 256, 1000, 2000)-MOTOR_PWM_OFFSET);
  rightMotor.writeMicroseconds(map(right, 0, 256, 1000, 2000)-MOTOR_PWM_OFFSET);
}

#endif // _HERMES_H
