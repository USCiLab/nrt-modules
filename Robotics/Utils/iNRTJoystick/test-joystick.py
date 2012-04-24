#!/usr/bin/env python
import nrtlib

nrtlib.loadModule(
  logicalpath  = '/Robotics/Utils/TransformManagerModule',
  bbnick       = 'hermes',
  parameters = {
    'maxCacheSize' : '1000',
    'maxCacheTime' : '30',
  },
  subscribertopicfilters = {
    'TransformUpdatePort' : 'Auto000001|Auto000002',
    'TransformLookupPort' : 'Auto000003',
  },
  position = (978, -243),
)

nrtlib.loadModule(
  logicalpath  = '/Robotics/Utils/TransformPosterModule',
  bbnick       = 'hermes',
  parameters = {
    'from' : 'world',
    'rate' : '0',
    'to' : 'robot',
    'transform' : '0,0,0,0,0,0',
  },
  postertopics = {
    'TransformUpdate' : 'Auto000002',
  },
  position = (230, -430),
)

nrtlib.loadModule(
  logicalpath  = '/Robotics/Drivers/HermesModule',
  bbnick       = 'hermes',
  parameters = {
    'baseframe' : 'world',
    'odomframe' : 'odometry',
    'serialdev' : '',
  },
  subscribertopicfilters = {
    'VelocityCommand' : 'Auto000000',
  },
  postertopics = {
    'OdometryUpdatePort' : '',
    'CompassZ' : '',
    'GyroZ' : '',
  },
  position = (261, 239),
)

nrtlib.loadModule(
  logicalpath  = '/Robotics/Utils/TransformVisualizerModule',
  bbnick       = 'hermes',
  parameters = {
    'scale' : '10',
    'transforms' : 'robot',
    'world' : 'world',
  },
  postertopics = {
    'TransformLookup' : 'Auto000003',
  },
  position = (220, -2),
)

nrtlib.loadModule(
  logicalpath  = '/Robotics/Navigation/DeadReckoningModule',
  bbnick       = 'hermes',
  parameters = {
    'base-frame' : 'world',
    'output-frame' : 'robot',
  },
  subscribertopicfilters = {
    'VelocityCommand' : 'Auto000000',
    'CompassData' : '',
  },
  postertopics = {
    'DeadReckoningOutput' : 'Auto000001',
  },
  position = (239, -243),
)

nrtlib.loadModule(
  logicalpath  = '/Robotics/Utils/iNRTJoystickModule',
  bbnick       = 'joystick',
  parameters = {
    'maxangular' : '1',
    'maxlinear' : '1',
    'port' : '61557',
    'webview' : 'http://google.com/',
  },
  postertopics = {
    'VelocityCommand' : 'Auto000000',
  },
  position = (-60, 241),
)

