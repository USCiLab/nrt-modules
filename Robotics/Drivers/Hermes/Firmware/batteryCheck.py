#!/usr/bin/env python
import sys
import serial
import struct

def main():
  if(len(sys.argv) != 3):
    print "Don't you mean..."
    print "./batteryCheck.py /dev/ttyUSBn 9600"
    return
  
  port = sys.argv[1]
  baud = sys.argv[2]

  ser = serial.Serial(port, baud, timeout=1)

  while(True):
    byte = ser.read()
    if(ord(byte) != 101):
      continue
    
    # we have a battery
    vals = ser.read(4)
    print "%.8f" % struct.unpack('f',vals)[0]
    break

  ser.close()


if __name__ == "__main__":
  main()
