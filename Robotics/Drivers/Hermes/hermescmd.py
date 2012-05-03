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

def move(leftSpeed, rightSpeed):
  l = max(0, min(255, int(leftSpeed  / 100.0 * 128 + 128)))
  r = max(0, min(255, int(rightSpeed / 100.0 * 128 + 128)))
  c = checksum([255, l, r])

  cmd = struct.pack('BBBB', 255, l, r, c)

  screen.addstr(10, 1, "Writing (%d, %d): %s               " % (leftSpeed, rightSpeed, ''.join(["%s " % str(ord(x)) for x in cmd])))
  ser.write(cmd)
  ser.flush()

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

    if abs(time.time() - lastmotortime) > .1:
      screen.addstr(18, 1, "TIME: " + str(time.time() - lastmotortime))
      lastmotortime = time.time()
      counter += 1
      screen.addstr(19, 1, "CMD COUNT: " + str(counter))
      move(left*speed, right*speed)


    endtime = time.time() + .01
    while(time.time() < endtime):
      if ser.inWaiting() > 0:
        databuf.append( ser.read(1) )

    # battery
    if len(databuf) >= 7 and databuf[0] == 255 and databuf[1] == 101 and databuf[6] == checksum(databuf[0:6]):
      screen.addstr(11, 1, "##### Battery: %.8f" % struct.unpack('f', ''.join([chr(x) for x in databuf[2:6]]))[0])

      for i in range(0, 6):
        databuf.pop(0)

    # compass
    elif len(databuf) >= 7 and databuf[0] == 255 and databuf[1] == 99 and databuf[6] == checksum(databuf[0:6]):
      screen.addstr(12, 1, "Compass: %.8f" % struct.unpack('f', ''.join([chr(x) for x in databuf[2:6]]))[0])

      for i in range(0, 6):
        databuf.pop(0)

    # gyro 
    elif len(databuf) >= 15 and databuf[0] == 255 and databuf[1] == 100 and databuf[14] == checksum(databuf[0:14]):
      screen.addstr(13, 1, "Gyro: %.8f %.8f %.8f" % (struct.unpack('f', ''.join([chr(x) for x in databuf[2:6]]))[0],
          struct.unpack('f', ''.join([chr(x) for x in databuf[6:10]]))[0],
          struct.unpack('f', ''.join([chr(x) for x in databuf[10:14]]))[0]))

      for i in range(0, 14):
        databuf.pop(0)

    elif len(databuf) >= 15:
      databuf.pop(0)

    time.sleep(0.001)
    screen.refresh()

finally:
  curses.echo()
  curses.nocbreak()
  screen.keypad(0);
  screen.nodelay(0)
  curses.endwin()
