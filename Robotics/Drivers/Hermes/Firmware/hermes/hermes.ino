#include <Wire.h>
#include <Servo.h>
#include "Arduino.h"
#include "HMC5883L.h"
#include "ITG3200.h"
#include "MedianFilter.h"
#include "ByteBuffer.h" 
#include "hermes.h"

void setup()
{
  Serial.begin(9600);
  leftMotor.attach(LEFT_SERVO_PIN);
  rightMotor.attach(RIGHT_SERVO_PIN);

  buffer.init(4);

  magnetometer.SetMeasurementMode(Measurement_Continuous);

  gyro.reset();
  gyro.init(ITG3200_ADDR_AD0_LOW);

  battery = new MedianFilter(1024);

  watchdog = millis();
}

void loop()
{
  // read a byte
  if (Serial.available() > 0)
    buffer.put(Serial.read());

  // check the buffer for motor commands
  if (buffer.peek(0) == 255)
  {
    byte checksum = 0;
    for (int i = 0; i < sizeof(MotorPacket)+2; i++)
      checksum ^= buffer.peek(i);

    if (checksum == buffer.peek(sizeof(MotorPacket)+1))
    {
      MotorPacket packet;
      
      // discard the start byte
      buffer.get();
      for (int i = 0; i < sizeof(MotorPacket); i++)
        packet.raw[i] = buffer.get();

      // discard the checksum byte
      buffer.get();
      
      // set the motors
      setMotors(packet.left, packet.right);
      
      // reset watchdog
      watchdog = millis();
    }
  }

  // write any sensor data
  {
    battery->push(analogRead(BATTERY_PIN));
    BatteryPacket bp;
    bp.voltage = BATTERY_ADJUSTMENT*battery->median(); 
    sendPacket(bp);
  }

  {
    CompassPacket cp;
    MagnetometerRaw raw = magnetometer.ReadRawAxis();
    cp.heading = atan2(raw.YAxis, raw.XAxis);
    cp.heading += MAGNETIC_DECLINATION;

    // normalize
    if (cp.heading < 0)
      cp.heading += 2*PI;
    else if (cp.heading > 2*PI)
      cp.heading -= 2*PI;

    sendPacket(cp);
  }

  {
    if (gyro.isRawDataReady())
    {
      GyroPacket gp;
      gyro.readGyro(gp.xyz);
      sendPacket(gp);
    }
  }

  if (millis() - watchdog > WATCHDOG_THRESHOLD)
    setMotors(64, 64);
}
