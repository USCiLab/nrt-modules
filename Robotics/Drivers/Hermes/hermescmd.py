#!/usr/bin/env python

import curses
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

screen = curses.initscr()
curses.noecho()
curses.cbreak()
screen.keypad(1);
screen.nodelay(1)
screen.border(0)

def checksum(l):
  checksum = 0
  for n in l:
    checksum ^= n
  return checksum

def writePacket(packet):
  if len(packet) != 3:
    raise Exception("Packet must be 3 bytes!")

  packet.insert(0, 255)

  #                        255........Command....Data1......Data2......Checksum.......
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

speed   = 0
left    = 0
right   = 0
databuf = []

lastmotortime = time.time()
counter = 0

try:
  #move(0, 0)
  running = True
  while running:

    screen.addstr(1, 1, " _     _ _______ ______  _______ _______  ______  ")
    screen.addstr(2, 1, "(_)   (_|_______|_____ \\(_______|_______)/ _____) ")
    screen.addstr(3, 1, " _______ _____   _____) )_  _  _ _____  ( (____   ")
    screen.addstr(4, 1, "|  ___  |  ___) |  __  /| ||_|| |  ___)  \\____ \\  ")
    screen.addstr(5, 1, "| |   | | |_____| |  \\ \\| |   | | |_____ _____) ) ")
    screen.addstr(6, 1, "|_|   |_|_______)_|   |_|_|   |_|_______|______/  ")
    
    cmd = screen.getch()
    screen.addstr(7, 1, "Welcome to HERMES CONTROL. Press q to quit")
    screen.addstr(9, 1, "Current Speed: " + str(speed) + " (use +/- to change)")

    if cmd == ord('q'):
      #move(0, 0)
      running = False
    elif cmd == ord('w'):
      left  = 1
      right = 1
    elif cmd == ord('s'):
      left  = -1
      right = -1
    elif cmd == ord('a'):
      left  = -1
      right = 1
    elif cmd == ord('d'):
      left  = 1
      right = -1
    elif cmd == ord(' '):
      left  = 0
      right = 0
      speed = 0
    elif cmd == ord('+') or cmd == ord('='):
      speed += 10
    elif cmd == ord('-') or cmd == ord('_'):
      speed -= 10

    speed = max(0, min(100, speed))

    # Move this robot
    move(speed*left, speed*right)

    
    # battery
    screen.addstr(11, 1, "Battery: %.3f" % writePacket([105,0,0]))

    # magnetometer
    screen.addstr(13, 1, "MAGNETOMETER")
    screen.addstr(14, 1, " > X: %.3f       " % writePacket([99,0,0]))
    screen.addstr(15, 1, " > Y: %.3f       " % writePacket([100,0,0]))
    screen.addstr(16, 1, " > Z: %.3f       " % writePacket([101,0,0]))

    # gyroscope
    screen.addstr(18, 1, "GYROSCOPE")
    screen.addstr(19, 1, " > X: %.3f       " % writePacket([102,0,0]))
    screen.addstr(20, 1, " > Y: %.3f       " % writePacket([103,0,0]))
    screen.addstr(21, 1, " > Z: %.3f       " % writePacket([104,0,0]))

    time.sleep(0.001)
    screen.refresh()

finally:
  curses.echo()
  curses.nocbreak()
  screen.keypad(0);
  screen.nodelay(0)
  curses.endwin()
