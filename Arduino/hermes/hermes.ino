#include <Servo.h>
#include "hermes.h"

void setup()
{
  Left.attach(LEFT_SERVO);
  Right.attach(RIGHT_SERVO);

  Serial.begin(BAUDRATE);
  LOG("Ready");
}

void loop()
{
  if (Serial.available() > 0)
  {
    dispatch(Serial.read());
  }
}
