static HMC5883L magnetometer;
static ITG3200 gyro;

static MagnetometerRaw mag;
static compassPacket magPacket;

static MedianFilter battery(1024);

void sensorSetup()
{
  // IMU
  magnetometer = HMC5883L();
  magnetometer.SetMeasurementMode(Measurement_Continuous);     
  
  gyro = ITG3200();
  gyro.reset();
  gyro.init(ITG3200_ADDR_AD0_LOW);
  // gyro.zeroCalibrate(2500,2); // samples,seconds
}

void sensorUpdate()
{
  if(millis()%200 == 0)
  {
    battery.push(analogRead(A0));
    batteryPacket bp;
    bp.voltage = 0.020708*battery.median();
    
    if(bp.voltage < 13){
      digitalWrite(DIGITAL_RELAY, LOW);
    }
        
    SERIALIZE(SEN_BATTERY, batteryPacket, bp);
  }
    
  if(millis()%100 == 0){
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
  if(gyro.isRawDataReady())
  {
    gyroPacket packet;
    gyro.readGyro(packet.xyz);
    
    SERIALIZE(SEN_GYRO, gyroPacket, packet);
  }
}
