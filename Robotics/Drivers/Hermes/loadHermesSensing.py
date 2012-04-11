#!/usr/bin/env python
import nrtlib

nrtlib.loadModule(
  logicalpath  = '/Robotics/Utils/TransformPosterModule',
  bbnick       = 'ilab21.1',
  instancename = 'Robot2KinectTransformer',
  parameters = {
    'from' : 'robot',
    'rate' : '1',
    'to' : 'kinect',
    'transform' : '-0.2,0,.3,0,1.67,1.57079633',
  },
  postertopics = {
    'TransformUpdate' : 'Robot2KinectTransform',
  },
  position = (-807, 323),
)

nrtlib.loadModule(
  logicalpath  = '/Robotics/PointCloud/PointCloudTransformModule',
  instancename = 'Kinect2WorldTransformer',
  bbnick       = 'ilab21.1',
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
  position = (-480, -237),
)

nrtlib.loadModule(
  logicalpath  = '/Robotics/PointCloud/PointCloudSinkModule',
  bbnick       = 'ilab21.1',
  parameters = {
    'name' : '',
    'point size' : '1',
    'use color' : 'true',
    'out' : 'display',
  },
  subscribertopicfilters = {
    'Cloud' : 'WorldPointCloud',
  },
  position = (128, -331),
)

nrtlib.loadModule(
  logicalpath  = '/Robotics/Utils/TransformPosterModule',
  instancename = 'World2RobotTransformer',
  bbnick       = 'ilab21.1',
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
  logicalpath  = '/Robotics/PointCloud/DepthImageToPointCloudModule',
  bbnick       = 'ilab21.1',
  subscribertopicfilters = {
    'Input' : 'KinectImage',
  },
  checkertopicfilters = {
    'FocalLength' : 'KinectFocalLength',
  },
  postertopics = {
    'Output' : 'RawPointCloud',
  },
  position = (-793, -443),
)

nrtlib.loadModule(
  logicalpath  = '/Robotics/Utils/TransformManagerModule',
  bbnick       = 'ilab21.1',
  parameters = {
    'maxCacheSize' : '1000',
    'maxCacheTime' : '30',
  },
  subscribertopicfilters = {
    'TransformUpdatePort' : 'World2RobotTransform|Robot2KinectTransform',
    'TransformLookupPort' : 'World2KinectLookup',
  },
  position = (101, 67),
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
    'Image' : 'KinectImage',
    'FocalLength' : 'KinectFocalLength',
    'Dims' : '',
    'FrameCount' : '',
    'Tick' : '',
    'Tock' : '',
    'Done' : '',
  },
  position = (-1088, -339),
)

