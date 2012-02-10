float xyz[3];
void sensorUpdate()
{  
  if(millis()%500 == 0)
  {
    MagnetometerRaw raw = magnetometer.ReadRawAxis();
    compassPacket packet;
    packet.heading = atan2(raw.YAxis, raw.XAxis);
    if(packet.heading < 0)
      packet.heading += 2*PI;
    Serial.write(SEN_COMPASS);
    for(int i=0; i<sizeof(compassPacket); i++)
      Serial.write(packet.raw[i]);
  }
  if(millis()%300 == 0)
  {
    if(gyro.isRawDataReady())
    {
      gyroPacket packet;
      gyro.readGyro(packet.xyz);
      Serial.write(SEN_GYRO);
      for(int i=0; i<sizeof(gyroPacket); i++)
        Serial.write(packet.raw[i]);
      
    }
  }
}