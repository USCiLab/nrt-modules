#!/usr/bin/env python
import nrtlib

nrtlib.loadModule(
  logicalpath  = '/Robotics/Navigation/DeadReckoningModule',
  bbnick       = 'ubuntu',
  parameters = {
    'base-frame' : 'world',
    'output-frame' : 'deadreckoning',
  },
  subscribertopicfilters = {
    'VelocityCommand' : 'vel',
    'CompassData' : '',
  },
  postertopics = {
    'DeadReckoningOutput' : 'Auto000000',
  },
  position = (-473, 101),
)

nrtlib.loadModule(
  logicalpath  = '/Robotics/Utils/TransformManagerModule',
  bbnick       = 'ubuntu',
  parameters = {
    'maxCacheSize' : '1000',
    'maxCacheTime' : '30',
  },
  subscribertopicfilters = {
    'TransformUpdatePort' : 'Auto000000|Auto000002',
    'TransformLookupPort' : 'Auto000001',
  },
  position = (-185, 102),
)

nrtlib.loadModule(
  logicalpath  = '/Robotics/Utils/TransformPosterModule',
  bbnick       = 'ubuntu',
  parameters = {
    'from' : 'world',
    'rate' : '0',
    'to' : 'robot',
    'transform' : '0,0,0,0,0,0',
  },
  postertopics = {
    'TransformUpdate' : 'Auto000002',
  },
  position = (-481, -160),
)

nrtlib.loadModule(
  logicalpath  = '/Robotics/Utils/TransformVisualizerModule',
  bbnick       = 'ubuntu',
  parameters = {
    'scale' : '10',
    'transforms' : 'robot',
    'world' : 'world',
  },
  postertopics = {
    'TransformLookup' : 'Auto000001',
  },
  position = (-484, 328),
)

