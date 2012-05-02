#!/usr/bin/env python

import curses
import time
import serial
from optparse import OptionParser

parser = OptionParser()
parser.add_option('-d', "--dev", dest="dev", default="/dev/ttyUSB0", help="The device file")
parser.add_option('-b', "--baud", dest="baud", default=9600, help="The serial port baud rate")
(options, args) = parser.parse_args()

ser = None
try:
  ser = serial.Serial(options.dev, options.baud)
except:
  print "Could not open serial port: " + options.dev
  exit()

def move(left, right):
  cmd = bytearray(4)
  cmd[0] = max(0, min(255, int(255))
  cmd[1] = max(0, min(255, int((left  / 100.0) * 64))
  cmd[2] = max(0, min(255, int((right / 100.0) * 64))
  cmd[3] = int(0 ^ cmd[0] ^ cmd[1] ^ cmd[2])
  ser.write(cmd)

screen = curses.initscr()
curses.noecho()
curses.cbreak()
screen.keypad(1);
screen.nodelay(1)
screen.border(0)

speed = 25
left = 0
right = 0

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
    running = False
  elif cmd == ord('w'):
    left  = speed
    right = speed
  elif cmd == ord('a'):
    left  = -speed
    right = -speed
  elif cmd == ord('s'):
    left  = -speed
    right = speed
  elif cmd == ord('d'):
    left  = speed
    right = -speed
  elif cmd == ord(' '):
    left  = 0
    right = 0
  elif cmd == ord('+') or cmd == ord('='):
    speed += 1
  elif cmd == ord('-') or cmd == ord('_'):
    speed -= 1

  move(left, right)

  screen.refresh()
  time.sleep(.1)


curses.echo()
curses.nocbreak()
screen.keypad(0);
screen.nodelay(0)
curses.endwin()
