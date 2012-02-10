float xyz[3];
void sensorUpdate()
{  
  if(millis()%500 == 0)
  {
    MagnetometerRaw raw = magnetometer.ReadRawAxis();
    float heading = atan2(raw.YAxis, raw.XAxis);
    if(heading < 0)
      heading += 2*PI;
    Serial.write(SEN_COMPASS);
    Serial.write(heading);
  }
  if(millis()%300 == 0)
  {
    if(gyro.isRawDataReady())
    {
      gyro.readGyro(xyz);
      Serial.write(SEN_GYRO);
      Serial.write(xyz[0]);
      Serial.write(xyz[1]);
      Serial.write(xyz[2]);
    }
  }
}