#include <SoftwareSerial.h>

#include "I2C.h"
#include <Servo.h>
#include "Arduino.h"
#include "HMC5883L.h"
#include "itg3200.h"
#include "MedianFilter.h"
#include "circbuf.h"
#include "hermes.h"

// ######################################################################
unsigned long start;
void setup()
{
  Serial.begin(115200);

  motorSerial.begin(9600);

  I2c.begin();
  I2c.timeOut(2);
  
  //leftMotor.attach(LEFT_SERVO_PIN);
  //rightMotor.attach(RIGHT_SERVO_PIN);
  setMotors(128, 128);

  gyro.begin(0x68);

  magnetometer.SetMeasurementMode(Measurement_Continuous);

  delay(1000);

  watchdog = millis();
  start    = millis();
}


// ######################################################################
bool checksumOK()
{
  byte checksum = 0;
  for(int i=0; i<buffer.capacity()-1; ++i)
    checksum ^= buffer.peek(i);
  return checksum == buffer.peek(buffer.capacity()-1);
}

// ######################################################################
void readLine(byte *line, int size)
{
  buffer.read(); // start
  for(int i=0; i<size; ++i) line[i] = buffer.read(); // cmd + data1 + data2
  buffer.read(); //checksum
}

// ######################################################################
void printBuffer()
{
  Serial.print("Buffer [");
  for(int i=0; i<buffer.capacity(); ++i)
  {
    Serial.print(buffer.peek(i));
    Serial.print(" ");
  }
  Serial.println("]");
}


// ######################################################################
MagnetometerRaw mag;
byte line[3];
void loop()
{
  if(Serial.available() > 0)
    buffer.write(Serial.read());

  if(buffer.isFull() && buffer.peek(0) == 255 && checksumOK())
  {
    readLine(line, sizeof(line));

    switch(line[0])
    {
      case ID_MOTOR:
        setMotors(line[1], line[2]);
        sendResponse(ID_MOTOR, 666);
        break;
      case ID_BATTERY:
        sendResponse(ID_BATTERY, BATTERY_ADJUSTMENT * analogRead(BATTERY_PIN));
        break;
      case ID_MAG_X:
        mag = magnetometer.ReadRawAxis();
        sendResponse(ID_MAG_X, mag.XAxis);
        break;
      case ID_MAG_Y:
        mag = magnetometer.ReadRawAxis();
        sendResponse(ID_MAG_Y, mag.YAxis);
        break;
      case ID_MAG_Z:
        mag = magnetometer.ReadRawAxis();
        sendResponse(ID_MAG_Z, mag.ZAxis);
        break;
      case ID_GYRO_X:
        sendResponse(ID_GYRO_X, gyro.getX());
        break;
      case ID_GYRO_Y:
        sendResponse(ID_GYRO_Y, gyro.getY());
        break;
      case ID_GYRO_Z:
        sendResponse(ID_GYRO_Z, gyro.getZ());
        break;
      default:
        sendResponse(ID_ERROR, 42.0);
        break;
    }

    // Reset the watchdog
    watchdog = millis();
  }

  // Check the watchdog
  if (millis() - watchdog > WATCHDOG_THRESHOLD)
  {
    setMotors(128, 128);
    watchdog = millis();
  }
}
