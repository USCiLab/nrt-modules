#!/usr/bin/env python
import nrtlib

nrtlib.loadModule(
  logicalpath  = '/Robotics/PointCloud/PointCloudTransformModule',
  bbnick       = 'ilab21.1',
  parameters = {
    'from' : 'world',
    'to' : 'kinect',
  },
  subscribertopicfilters = {
    'Input' : 'Auto000002',
  },
  postertopics = {
    'Output' : 'Auto000005',
    'TransformLookup' : 'Auto000006',
  },
  position = (-480, -237),
)

nrtlib.loadModule(
  logicalpath  = '/Robotics/Utils/TransformPosterModule',
  bbnick       = 'ilab21.1',
  parameters = {
    'from' : 'world',
    'rate' : '1',
    'to' : 'robot',
    'transform' : '0,0,0,0,0,0',
  },
  postertopics = {
    'TransformUpdate' : 'Auto000003',
  },
  position = (-809, 106),
)

nrtlib.loadModule(
  logicalpath  = '/Robotics/Utils/TransformManagerModule',
  bbnick       = 'ilab21.1',
  parameters = {
    'maxCacheSize' : '1000',
    'maxCacheTime' : '30',
  },
  subscribertopicfilters = {
    'TransformUpdatePort' : 'Auto000003|Auto000004',
    'TransformLookupPort' : 'Auto000006',
  },
  position = (101, 67),
)

nrtlib.loadModule(
  logicalpath  = '/Robotics/PointCloud/DepthImageToPointCloudModule',
  bbnick       = 'ilab21.1',
  subscribertopicfilters = {
    'Input' : 'Auto000000',
  },
  checkertopicfilters = {
    'FocalLength' : 'Auto000001',
  },
  postertopics = {
    'Output' : 'Auto000002',
  },
  position = (-793, -443),
)

nrtlib.loadModule(
  logicalpath  = '/Robotics/PointCloud/OpenNISourceModule',
  bbnick       = 'ilab21.1',
  parameters = {
    'framerate' : '-1',
    'data' : 'Image+Depth',
    'mirror' : 'false',
    'xml' : '',
  },
  postertopics = {
    'Image' : 'Auto000000',
    'FocalLength' : 'Auto000001',
    'Dims' : '',
    'FrameCount' : '',
    'Tick' : '',
    'Tock' : '',
    'Done' : '',
  },
  position = (-1088, -339),
)

nrtlib.loadModule(
  logicalpath  = '/Robotics/Utils/TransformPosterModule',
  bbnick       = 'ilab21.1',
  parameters = {
    'from' : 'robot',
    'rate' : '1',
    'to' : 'kinect',
    'transform' : '-0.127,0,-.3,3.14,1.65,1.57079633',
  },
  postertopics = {
    'TransformUpdate' : 'Auto000004',
  },
  position = (-810, 318),
)

nrtlib.loadModule(
  logicalpath  = '/Robotics/PointCloud/PointCloudSinkModule',
  bbnick       = 'ilab21.1',
  parameters = {
    'name' : '',
    'point size' : '0.100000001',
    'use color' : 'true',
    'out' : 'display',
  },
  subscribertopicfilters = {
    'Cloud' : 'Auto000005',
  },
  position = (-182, -317),
)

