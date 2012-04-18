#!/usr/bin/env python
import nrtlib

nrtlib.loadModule(
  logicalpath  = '/Robotics/PointCloud/DepthImageToPointCloudModule',
  bbnick       = 'hermes',
  subscribertopicfilters = {
    'Input' : 'KinectImage|Auto000001',
  },
  checkertopicfilters = {
    'FocalLength' : 'KinectFocalLength|Auto000002',
  },
  postertopics = {
    'Output' : 'RawPointCloud',
  },
  position = (-795, -457),
)

nrtlib.loadModule(
  logicalpath  = '/Robotics/Utils/TransformPosterModule',
  bbnick       = 'hermes',
  instancename = 'World2RobotTransformer',
  parameters = {
    'from' : 'world',
    'rate' : '1',
    'to' : 'robot',
    'transform' : '0,0,0,0,0,0',
  },
  postertopics = {
    'TransformUpdate' : 'World2RobotTransform',
  },
  position = (-809, 106),
)

nrtlib.loadModule(
  logicalpath  = '/Robotics/Utils/TransformPosterModule',
  bbnick       = 'hermes',
  instancename = 'Robot2KinectTransformer',
  parameters = {
    'from' : 'robot',
    'rate' : '1',
    'to' : 'kinect',
    'transform' : '-0.2,0,.32,0,1.67,1.6',
  },
  postertopics = {
    'TransformUpdate' : 'Robot2KinectTransform',
  },
  position = (-807, 323),
)

nrtlib.loadModule(
  logicalpath  = '/Robotics/PointCloud/PointCloudTransformModule',
  bbnick       = 'hermes',
  instancename = 'Kinect2WorldTransformer',
  parameters = {
    'from' : 'world',
    'to' : 'kinect',
  },
  subscribertopicfilters = {
    'Input' : 'RawPointCloud',
  },
  postertopics = {
    'Output' : 'WorldPointCloud',
    'TransformLookup' : 'World2KinectLookup',
  },
  position = (-480, -267),
)

nrtlib.loadModule(
  logicalpath  = '/Robotics/PointCloud/OpenNISourceModule',
  bbnick       = 'hermes',
  parameters = {
    'framerate' : '-1',
    'data' : 'Image+Depth',
    'mirror' : 'false',
    'xml' : '',
  },
  postertopics = {
    'Image' : 'Auto000001',
    'FocalLength' : 'Auto000002',
    'Dims' : '',
    'FrameCount' : '',
    'Tick' : '',
    'Tock' : '',
    'Done' : '',
  },
  position = (-1034, -379),
)

nrtlib.loadModule(
  logicalpath  = '/Robotics/Utils/TransformManagerModule',
  bbnick       = 'hermes',
  parameters = {
    'maxCacheSize' : '1000',
    'maxCacheTime' : '30',
  },
  subscribertopicfilters = {
    'TransformUpdatePort' : 'World2RobotTransform|Robot2KinectTransform',
    'TransformLookupPort' : 'World2KinectLookup|Auto000000',
  },
  position = (101, 67),
)

nrtlib.loadModule(
  logicalpath  = '/Robotics/PointCloud/WorldDisplayModule',
  bbnick       = 'hermes',
  parameters = {
    'base' : 'world',
    'transforms' : 'kinect',
  },
  subscribertopicfilters = {
    'Cloud' : 'WorldPointCloud',
  },
  postertopics = {
    'TransformLookup' : 'Auto000000',
  },
  position = (-126, -267),
)

