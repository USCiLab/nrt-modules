#ifndef GLOBALS_H
#define GLOBALS_H

// 
// Debug 
// 
// #define DEBUG

#define POS __FUNCTION__ + "() line " + __LINE__ + ":\t"

#ifdef DEBUG
#define LOG_RAW(message) Serial.write(message);
#define LOG(message) { String msg = ""; Serial.println(msg + POS + message); }
#else
#define LOG_RAW(message) ;
#define LOG(message) ;
#endif

#ifndef DEBUG
#define SERIALIZE(type, object, var) Serial.write(type); for(int i=0; i<sizeof(object); i++) Serial.write(var.raw[i]);
#else
#define SERIALIZE(type, object, var) Serial.print("Sensor Data Type: "); Serial.print(type); Serial.print(" Payload: "); for(int i=0; i<sizeof(object); i++) { Serial.print(var.raw[i]); Serial.print(" "); } Serial.println("");
#endif

#ifndef DEBUG
#define RATE_LIMIT(duration) if(millis() % duration == 0)
#else
#define RATE_LIMIT(duration) if(millis() % 2000 == 0)
#endif

#endif
