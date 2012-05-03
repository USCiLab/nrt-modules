#!/usr/bin/env python

import time
import serial
import struct
from optparse import OptionParser

parser = OptionParser()
parser.add_option('-d', "--dev", dest="dev", default="/dev/ttyUSB0", help="The device file")
parser.add_option('-b', "--baud", dest="baud", default=115200, help="The serial port baud rate")
(options, args) = parser.parse_args()

ser = None
try:
  ser = serial.Serial(options.dev, options.baud)
except:
  print "Could not open serial port: " + options.dev
  exit()

def checksum(l):
  checksum = 0
  for n in l:
    checksum ^= n
  return checksum

def writePacket(packet):
  if len(packet) != 3:
    raise Exception("Packet must be 3 bytes!")

  packet.insert(0, 255)

  s = struct.pack('BBBBB', packet[0], packet[1], packet[2], packet[3], checksum(packet))
  ser.write(s)

  buf = []
  endtime = time.time() + 1.0
  found = False
  while time.time() < endtime and found == False:
    if ser.inWaiting() > 0:
      buf.append(ord(ser.read(1)))
      if len(buf) > 7:
        buf.pop(0)

      if len(buf) == 7 and buf[0] == 255 and buf[1] == packet[1] and buf[6] == checksum(buf[0:6]):
        found = True

  if found == False:
    raise Exception("Timed out when receiving response: buffer was", buf)

  if buf[0] != 255:
    raise Exception("Bad start byte")

  if buf[1] != packet[1]:
    raise Exception("Got wrong packet back")

  if buf[6] != checksum(buf[0:6]):
    raise Exception("Bad Checksum")

  time.sleep(0.001)
  return struct.unpack('f', ''.join([chr(x) for x in buf[2:6]]))[0]

def move(leftSpeed, rightSpeed):
  l = max(0, min(255, int(leftSpeed  / 100.0 * 128 + 128)))
  r = max(0, min(255, int(rightSpeed / 100.0 * 128 + 128)))

  writePacket([98, l, r])

speed = 0
up = 1
inc = 0.1
maximum = 30
while True:
  move(speed, speed)
  if up:
    speed = speed + inc
    if speed > maximum:
      up = 0
  else:
    speed = speed - inc
    if speed < -maximum:
      up = 1


  response = writePacket([105,0,0])
  print "Got Battery: ", response

  response = writePacket([99,0,0])
  print "Got MagX: ",response

  response = writePacket([100,0,0])
  print "Got MagY: ",response

  response = writePacket([101,0,0])
  print "Got MagZ: ",response
   
  response = writePacket([102,0,0])
  print "Got GyroX: ",response
    
  response = writePacket([103,0,0])
  print "Got GyroY: ",response

  response = writePacket([104,0,0])
  print "Got GyroZ: ",response

