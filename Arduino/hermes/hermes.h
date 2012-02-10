#define BOUND_FORWARD   2000
#define BOUND_STOP      1500
#define BOUND_BACKWARD  1000

#define BAUDRATE  115200

// Pins
#define LEFT_SERVO 9
#define RIGHT_SERVO 10

// Commands
#define CMD_RESET     97
#define CMD_SETSPEED  98 // {leftSpeed 0-127} {rightSpeed 0-127}

// Sensors
#define SEN_COMPASS   99
union compassPacket {
  byte raw[4];
  float heading;
};
#define SEN_GYRO      100
union gyroPacket {
  byte raw[12];
  float xyz[3];
};

// Globals
Servo Left;
Servo Right;

// 
// Debug 
// 
// #define DEBUG

#ifdef DEBUG
#define LOG_RAW(message) Serial.write(message);
#define LOG(message) Serial.println(message);
#else
#define LOG_RAW(message) ;
#define LOG(message) ;
#endif

int timeRunning;