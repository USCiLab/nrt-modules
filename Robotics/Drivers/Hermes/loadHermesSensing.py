#!/usr/bin/env python
import nrtlib

nrtlib.loadModule(
  logicalpath  = '/Robotics/ImageProc/IO/DisplaySinkModule',
  bbnick       = 'ilab21.1',
  subscribertopicfilters = {
    'Image' : 'grid|Auto000000',
  },
  checkertopicfilters = {
    'Label' : '',
  },
  postertopics = {
    'MouseClick' : '',
    'KeyboardPress' : '',
  },
  position = (215, -633),
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
  position = (89, -277),
)

nrtlib.loadModule(
  logicalpath  = '/Robotics/PointCloud/PointCloudTransformModule',
  bbnick       = 'ilab21.1',
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
  logicalpath  = '/Robotics/Utils/TransformPosterModule',
  bbnick       = 'ilab21.1',
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
  logicalpath  = '/Robotics/ImageProc/IO/DisplaySinkModule',
  bbnick       = 'ilab21.1',
  subscribertopicfilters = {
    'Image' : 'KinectImage',
  },
  checkertopicfilters = {
    'Label' : '',
  },
  postertopics = {
    'MouseClick' : '',
    'KeyboardPress' : '',
  },
  position = (-716, -756),
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
  logicalpath  = '/Robotics/Utils/TransformPosterModule',
  bbnick       = 'ilab21.1',
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
  position = (-795, -457),
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

nrtlib.loadModule(
  logicalpath  = '/Robotics/PointCloud/PointCloud2OccupancyModule',
  bbnick       = 'ilab21.1',
  parameters = {
    'lowerthreshold' : '.1',
    'mapdims'        : '3x3',
    'pixelspermeter' : '50',
    'upperthreshold' : '3',
  },
  subscribertopicfilters = {
    'PointCloudInput' : 'WorldPointCloud',
  },
  postertopics = {
    'OccupancyGridOutput' : 'grid',
  },
  position = (-223, -592),
)

