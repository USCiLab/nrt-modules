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

def move(leftSpeed, rightSpeed):
  l = max(0, min(255, int(leftSpeed  / 100.0 * 128 + 128)))
  r = max(0, min(255, int(rightSpeed / 100.0 * 128 + 128)))
  c = checksum([255, l, r])

  cmd = struct.pack('BBBB', 255, l, r, c)

  print "Writing (%d, %d): %s " % (leftSpeed, rightSpeed, ''.join(["%s " % str(ord(x)) for x in cmd]))
  ser.write(cmd)
  ser.flush()

def writePacket(packet):
  if len(packet) != 3:
    raise "Packet must be 3 bytes!"

  packet.insert(0, 255)

  s = struct.pack('BBBBB', packet[0], packet[1], packet[2], packet[3], checksum(packet))
  ser.write(s)


ser.write(chr(99))
ser.write(chr(98))
writePacket([1,2,3])
ser.write(chr(255))
ser.write(chr(255))
writePacket([4,5,6])

#ser.write(struct.pack('BBBB', 1, 2, 3, 4, 5))

#move(20, 20)
while True:
  print ser.readline()
