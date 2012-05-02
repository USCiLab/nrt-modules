#ifndef _HERMES_H
#define _HERMES_H

#define MOTOR_PWM_OFFSET      6
#define BATTERY_ADJUSTMENT    0.0214
#define MAGNETIC_DECLINATION  219/1000.0 // declination in LA

#define BAUDRATE              9600
#define WATCHDOG_THRESHOLD	  1000

// pin defs
#define LEFT_SERVO_PIN        9
#define RIGHT_SERVO_PIN       10
#define BATTERY_PIN           A0
#define DIGITAL_RELAY         52

// packet definitions
union MotorPacket
{
  byte raw[2];
  struct
  {
    byte left;
    byte right;
  };
};

union BatteryPacket
{
  byte raw[4];
  float voltage;
};

union CompassPacket
{
  byte raw[4];
  float heading;
};

union GyroPacket
{
  byte raw[12];
  float xyz[3];
  struct
  {
    float x;
    float y;
    float z;
  };
};

// helper function for packets and packet ID definitions
template<class PacketT>
byte packet_id();

template<> byte packet_id<CompassPacket>() { return 99; }
template<> byte packet_id<GyroPacket>()    { return 100; }
template<> byte packet_id<BatteryPacket>() { return 101; }

// bookkeeping vars
ByteBuffer buffer;
unsigned int watchdog;

// motors
Servo leftMotor;
Servo rightMotor;

// sensors
HMC5883L magnetometer;
MagnetometerRaw magnetometerRaw;
ITG3200 gyro;
MedianFilter *battery;

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
}

void setMotors(int left, int right)
{
  if (left < 0 || right < 0)
    return;

  leftMotor.writeMicroseconds(map(left, 0, 128, 1000, 2000)-MOTOR_PWM_OFFSET);
  rightMotor.writeMicroseconds(map(right, 0, 128, 1000, 2000)-MOTOR_PWM_OFFSET);
}

#endif // _HERMES_H
