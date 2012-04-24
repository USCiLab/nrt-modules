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
  
  return true;
}

