#include <Servo.h>

#define FORWARD   2000
#define STOP      1500
#define BACKWARD  1000

Servo left;
Servo right;
int leftPos = 0;
int inByte = 0;

void setup()
{
  left.attach(9);
  right.attach(10);

  Serial.begin(115200);
  Serial.write("Ready\n");
}

int calculateMillis(int speed)
{
  if(speed > 0)
    return STOP + (FORWARD - STOP)*speed / 100; 
  if(speed <= 0)
    return STOP - (FORWARD - STOP)*(-speed) / 100; 
}

void loop()
{
  if (Serial.available() > 0)
  {
    inByte = Serial.read();

    int speed = ((inByte - '0') - 5)*20;
    Serial.write("Speed: ");
    Serial.println(speed);

    left.writeMicroseconds(calculateMillis(speed));
    right.writeMicroseconds(calculateMillis(speed));
  }
}
