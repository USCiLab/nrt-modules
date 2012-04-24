#include "hermes.h"
#include <Wire.h>
#include <Servo.h>
// #include <ADXL345.h>
#include "HMC5883L.h"
#include "ITG3200.h"
#include "globals.h"
#include "serialdata.h"
#include "MedianFilter.h"

class Motors
{
public:
  Servo Left;
  Servo Right;
  bool enabled;
  
public:
  Motors () 
  {
    // Attach servos
    Left.attach(LEFT_SERVO);
    Right.attach(RIGHT_SERVO);
    
    enabled = false;
    
    setSpeed(64,64);
  }
  
  /**
  * Scales speed (0-128) to motor speed (1000-2000). Idle is 64 and 1500, respectively.
  */
  unsigned int microsFromByte(byte speed)
  {
    if(speed > 64)
      return BOUND_STOP + (BOUND_FORWARD-BOUND_STOP) * ((float)(speed-64)/64);
    else
      return BOUND_BACKWARD + (BOUND_STOP-BOUND_BACKWARD) * ((float)(speed)/64);
  }
  
  void setSpeed(int leftSpeed, int rightSpeed)
  {
    if(!enabled)
    {
      leftSpeed = 64;
      rightSpeed = 64;
    }
    
    if(leftSpeed < 0 || rightSpeed < 0)
      return;
  
    int writeLeft = (leftSpeed == 64) ? 0 : microsFromByte(leftSpeed);
    int writeRight = (rightSpeed == 64) ? 0 : microsFromByte(rightSpeed);
    LOG(leftSpeed);
    Left.writeMicroseconds(microsFromByte(leftSpeed-6));
    Right.writeMicroseconds(microsFromByte(rightSpeed-6));
  }
  
  void disable()
  {
    enabled = false;
  }
  
  void enable()
  {
    enabled = true;
  }
};

class Sensors
{
public:
  HMC5883L magnetometer;
  ITG3200 gyro;

  MagnetometerRaw mag;
  compassPacket magPacket;

  MedianFilter *battery;
  
public:
  Sensors ()
  {
    // IMU
    // magnetometer = HMC5883L();
    magnetometer.SetMeasurementMode(Measurement_Continuous);     

    // gyro = ITG3200();
    gyro.reset();
    gyro.init(ITG3200_ADDR_AD0_LOW);
    // // gyro.zeroCalibrate(2500,2); // samples,seconds
    // 
    battery = new MedianFilter(1024);
  }
  
  ~Sensors() { delete battery; }
  
  void tick()
  {
    RATE_LIMIT(200) {
      battery->push(analogRead(A0));
      batteryPacket bp;
      bp.voltage = 0.020708*battery->median();
    
      if(bp.voltage < 13){
        digitalWrite(DIGITAL_RELAY, LOW);
      }
        
      SERIALIZE(SEN_BATTERY, batteryPacket, bp);
    }
        
    RATE_LIMIT(200){
      // Magnetometer
      mag = magnetometer.ReadRawAxis();
      magPacket.heading = atan2(mag.YAxis, mag.XAxis);
      float declination = 219/1000.0; // Magnetic declination in Los Angeles
      magPacket.heading += declination;
    
      // Normalize 
      if(magPacket.heading < 0)
        magPacket.heading += 2*PI;
      else if(magPacket.heading > 2*PI)
        magPacket.heading -= 2*PI;
    
      SERIALIZE(SEN_COMPASS, compassPacket, magPacket);
    }
  
    // Gyro
    RATE_LIMIT(100){
      if(gyro.isRawDataReady())
      {
        gyroPacket packet;
        gyro.readGyro(packet.xyz);
    
        SERIALIZE(SEN_GYRO, gyroPacket, packet);
      }
    }
  }
};

typedef enum {IDLE, ACTIVE} State;
class Hermes
{
public:
  Motors motors;
  Sensors sensors;
  State state;
  unsigned int lastUpdate;
  
public:
  Hermes ()
  {
    setState(IDLE);
    lastUpdate = millis();
  }
  
  void setState(State newState)
  {
    if(newState == IDLE)
     {
       LOG("Deactivating");
       motors.setSpeed(64,64);
       motors.disable();
     }
     if(newState == ACTIVE)
     {
       motors.enable();
       LOG("Activating");
     }
     state = newState;
   }
  
  void dispatch(char cmd)
  {
    switch(cmd)
    {
      case(CMD_RESET):
        motors.enable();
        setState(ACTIVE);
      break;
    
      case(CMD_SETSPEED):
        if(state == IDLE) 
          waitForBytes(2); // Throw away 2 bytes
        
        if (!waitForBytes(2))
          break;
        motors.setSpeed(Serial.read(), Serial.read());
      break;
    
      default:
        LOG("Unrecognized cmd");
      break;
    }
  }
  
  void tick()
  {
    if(state == ACTIVE)
    {
      if(millis() > (lastUpdate + WATCHDOG_THRESHOLD))
        setState(IDLE);
    }
    
    if (Serial.available() > 0) {
      dispatch(Serial.read());
      lastUpdate = millis();
    } else {
    
    }
    
    sensors.tick();
  }
};

static Hermes* hermes;

void setup()
{
  Serial.begin(BAUDRATE);
  Wire.begin();
  hermes = new Hermes();
      
  // Power computer
  digitalWrite(DIGITAL_RELAY, HIGH);
}

void loop()
{
  hermes->tick();
}
