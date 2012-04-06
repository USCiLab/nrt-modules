void sensorUpdate()
{
  // if(millis()%100 == 0)
  // {
  //   batteryPacket battery;
  //   battery.voltage = 0.020793*analogRead(A0);
  //   Serial.println(battery.voltage);
  // }
  return;
  MagnetometerRaw raw = magnetometer.ReadRawAxis();
  compassPacket packet;
  packet.heading = atan2(raw.YAxis, raw.XAxis);
  if(packet.heading < 0)
    packet.heading += 2*PI;
  Serial.write(SEN_COMPASS);
  for(int i=0; i<sizeof(compassPacket); i++)
    Serial.write(packet.raw[i]);
  if(gyro.isRawDataReady())
  {
    gyroPacket packet;
    gyro.readGyro(packet.xyz);
    Serial.write(SEN_GYRO);
    for(int i=0; i<sizeof(gyroPacket); i++)
      Serial.write(packet.raw[i]);

  }
}
