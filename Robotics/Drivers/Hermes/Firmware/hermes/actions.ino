/**
* Contains functions that respond to commands
*/

/**
* Blocks until aBytes come in
* @param aBytes bytes to wait for
* @return false if timed out, true if successful
*/
bool waitForBytes(int aBytes)
{
  int start = millis();
  while(Serial.available() < aBytes)
  {
    // LOG(millis() - start);
    ;
  }
  // int count = Serial.available();
  // if(count < aBytes)
  // {
  //   // Clear the buffer of fragmented data
  //   for(int i=0; i<count; i++) {
  //     Serial.read();
  //   }
  //   LOG("Bytes not recieved, cleared data fragment.");
  //   return false;
  // }
  
  return true;
}

/**
* Scales speed (0-128) to motor speed (1000-2000). Idle is 64 and 1500, respectively.
*/
unsigned int microsFromByte(byte speed)
{
  if(speed > 64)
    return BOUND_STOP + (BOUND_FORWARD-BOUND_STOP) * ((float)(speed-64)/64);
  else
    return BOUND_BACKWARD + (BOUND_STOP-BOUND_BACKWARD) * ((float)(speed)/64);
}

void cmd_setSpeed(int leftSpeed, int rightSpeed)
{
  if(leftSpeed < 0 || rightSpeed < 0)
    return;
    
  Left.writeMicroseconds(microsFromByte(rightSpeed));
  Right.writeMicroseconds(microsFromByte(leftSpeed));
}

void dispatch(char cmd)
{
  int now;
  switch(cmd)
  {
    case(CMD_RESET):
      ;
    break;

    case(CMD_SETSPEED):
      if (!waitForBytes(2))
        break;
      cmd_setSpeed(Serial.read(), Serial.read());
      LOG("Speed set");
    break;

    default:
      LOG("Unrecognized cmd");
    break;
  }
}
