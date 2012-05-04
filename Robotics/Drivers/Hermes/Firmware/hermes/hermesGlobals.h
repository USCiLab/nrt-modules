#ifndef _HERMES_GLOBALS_H
#define _HERMES_GLOBALS_H

enum packetid
{
  ID_ERROR   = 1,
  ID_MOTOR   = 98,
  ID_MAG_X   = 99,
  ID_MAG_Y   = 100,
  ID_MAG_Z   = 101,
  ID_GYRO_X  = 102,
  ID_GYRO_Y  = 103,
  ID_GYRO_Z  = 104,
  ID_BATTERY = 105
};

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
union CommandPacket
{
  unsigned char raw[3];
  struct
  {
    unsigned char command;
    unsigned char data1;
    unsigned char data2;
  };
};

union ResponsePacket
{
  uint8_t raw[4];
  float data;
};

#endif // _HERMES_GLOBALS_H
