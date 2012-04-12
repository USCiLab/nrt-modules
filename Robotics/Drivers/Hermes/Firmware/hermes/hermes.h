#ifndef HERMES_H
#define HERMES_H

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

// Globals
Servo Left;
Servo Right;

// State
typedef enum {IDLE,ACTIVE} state;
state hermesState;

// Pins
#define BATTERY_IN A0
#define DIGITAL_RELAY 52

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

#ifndef DEBUG
#define SERIALIZE(type, object, var) Serial.write(type); for(int i=0; i<sizeof(object); i++) Serial.write(var.raw[i]);
#else
#define SERIALIZE(type, object, var) Serial.print("Sensor Data Type: "); Serial.print(type); Serial.print(" Payload: "); for(int i=0; i<sizeof(object); i++) { Serial.print(var.raw[i]); Serial.print(" "); } Serial.println("");
#endif

#endif