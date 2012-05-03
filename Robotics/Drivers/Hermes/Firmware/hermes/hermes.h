#ifndef _HERMES_H
#define _HERMES_H
#include "hermesGlobals.h"

// bookkeeping vars
CircularBuffer<4> buffer;
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

template<> byte packet_id<CompassPacket>() { return SEN_COMPASS; }
template<> byte packet_id<GyroPacket>()    { return SEN_GYRO; }
template<> byte packet_id<BatteryPacket>() { return SEN_BATTERY; }

template<class PacketT>
void sendPacket(PacketT const& packet)
{
  byte packetId = packet_id<PacketT>();

  Serial.write(255);
  Serial.write(packetId);

  byte checksum = 255 ^ packetId;
  for (int i = 0; i < sizeof(PacketT); i++)
  {
    checksum ^= packet.raw[i];
    Serial.write(packet.raw[i]);
  }
  Serial.write(checksum);
  Serial.flush();
}

void setMotors(unsigned char left, unsigned char right)
{
  leftMotor.writeMicroseconds(map(left, 0, 256, 1000, 2000)-MOTOR_PWM_OFFSET);
  rightMotor.writeMicroseconds(map(right, 0, 256, 1000, 2000)-MOTOR_PWM_OFFSET);
}

#endif // _HERMES_H
