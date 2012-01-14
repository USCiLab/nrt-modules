#include <Servo.h>
#include "hermes.h"

static Servo left;
static Servo right;

void setup()
{
  left.attach(LEFT_SERVO);
  right.attach(RIGHT_SERVO);

  Serial.begin(BAUDRATE);
  Serial.write("Ready\n");
}

void dispatch(char cmd)
{
  char leftSpeed, rightSpeed;
  int now;
  switch(cmd)
  {
    case(CMD_RESET):
      Serial.write("Reset\n");
    break;
    
    case(CMD_SETSPEED):
      Serial.write("Set Speed\n");
      
      now = millis();
      while(Serial.available() < 2 && now+30 > millis())
      {
        //Serial.write("Waiting...\n");
      }
      if(Serial.available() < 2)
      {
        Serial.read();
        Serial.write("Bailing\n");
        break;
      }
      
      leftSpeed = Serial.read();
      rightSpeed = Serial.read();
      
      Serial.println((byte)leftSpeed);
      Serial.println((byte)rightSpeed);
      
      if(leftSpeed < 0 || rightSpeed < 0)
        break;
      
      left.writeMicroseconds(microsFromByte(rightSpeed));
      right.writeMicroseconds(microsFromByte(leftSpeed));
    break;
    
    default:
      Serial.write("Unrecognized cmd\n");
    break;
  }
}

unsigned int microsFromByte(byte speed)
{
  if(speed > 64)
    return BOUND_STOP + (BOUND_FORWARD-BOUND_STOP) * ((float)(speed-64)/64);
  else
    return BOUND_BACKWARD + (BOUND_STOP-BOUND_BACKWARD) * ((float)(speed)/64);
}

void loop()
{
  if (Serial.available() > 0)
  {
    dispatch(Serial.read());
  }
}
