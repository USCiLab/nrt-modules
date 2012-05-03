#include "I2C.h"
#include "itg3200.h"

ITG3200 gyro;

void setup(){
  Serial.begin(115200);
  I2c.begin();
  gyro.begin(0x68);
  delay(1000);
  Serial.println("STARTED UP");
}

void loop(){
  Serial.print(" X=");
  Serial.print(gyro.getX());
  Serial.print(" Y=");
  Serial.print(gyro.getY());
  Serial.print(" Z=");
  Serial.println(gyro.getZ());
  //delay(400);
}

