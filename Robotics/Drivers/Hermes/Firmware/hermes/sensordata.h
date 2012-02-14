#ifndef SENSORDATA_H
#define SENSORDATA_H

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

#endif // SENSORDATA_H
