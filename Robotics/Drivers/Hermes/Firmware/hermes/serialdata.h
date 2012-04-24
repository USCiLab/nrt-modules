#ifndef SERIALDATA_H
#define SERIALDATA_H

#define SEN_COMPASS   99
union compassPacket {
  unsigned char raw[4];
  float heading;
};

#define SEN_GYRO      100
union gyroPacket {
  unsigned char raw[12];
  float xyz[3];
};

#define SEN_BATTERY 101
union batteryPacket {
  unsigned char raw[4];
  float voltage;
};

// Commands
#define CMD_RESET     97
#define CMD_SETSPEED  98 // {leftSpeed 0-127} {rightSpeed 0-127}
union motorSpeedPacket { // TODO
  unsigned char raw[2];
  struct values {
    unsigned char left;
    unsigned char right;
  };
};

#endif // SERIALDATA_H
