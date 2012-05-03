#ifndef _HERMES_GLOBALS_H
#define _HERMES_GLOBALS_H

#define SEN_COMPASS 99
#define SEN_GYRO 100
#define SEN_BATTERY 101

#define MOTOR_PWM_OFFSET      24
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
  unsigned char raw[2];
  struct
  {
    unsigned char left;
    unsigned char right;
  };
};

union BatteryPacket
{
  unsigned char raw[4];
  float voltage;
};

union CompassPacket
{
  unsigned char raw[4];
  float heading;
};

union GyroPacket
{
  unsigned char raw[12];
  float xyz[3];
  struct
  {
    float x;
    float y;
    float z;
  };
};

#endif // _HERMES_GLOBALS_H
