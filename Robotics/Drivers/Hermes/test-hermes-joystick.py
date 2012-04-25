#!/usr/bin/env python
import nrtlib

nrtlib.loadModule(
  logicalpath  = '/Robotics/Drivers/HermesModule',
  bbnick       = 'hermes',
  parameters = {
    'baseframe' : 'world',
    'odomframe' : 'odometry',
    'serialdev' : '/dev/ttyUSB0',
  },
  subscribertopicfilters = {
    'VelocityCommand' : 'Auto000000',
  },
  postertopics = {
    'OdometryUpdatePort' : '',
    'CompassZ' : '',
    'GyroZ' : '',
  },
  position = (52, -95),
)

nrtlib.loadModule(
  logicalpath  = '/Robotics/Utils/iNRTJoystickModule',
  bbnick       = 'hermes',
  parameters = {
    'maxangular' : '1',
    'maxlinear' : '1',
    'port' : '61557',
    'webview' : 'http://google.com/',
  },
  postertopics = {
    'VelocityCommand' : 'Auto000000',
  },
  position = (-317, -110),
)

