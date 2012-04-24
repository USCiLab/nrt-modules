#!/usr/bin/env python
import nrtlib

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
  position = (70, 815),
)

nrtlib.loadModule(
  logicalpath  = '/Robotics/PointCloud/PointCloud2OccupancyModule',
  bbnick       = 'hermes',
  parameters = {
    'lowerthreshold' : '0.20000000000000001',
    'mapdims' : '8x8',
    'pixelspermeter' : '10',
    'shadows' : '128',
    'upperthreshold' : '3',
  },
  subscribertopicfilters = {
    'PointCloudInput' : 'WorldPointCloud',
  },
  postertopics = {
    'OccupancyGridOutput' : 'Auto000003',
    'PixelsPerMeter' : 'Auto000004',
  },
  position = (239, -287),
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
  position = (-1226, -191),
)

nrtlib.loadModule(
  logicalpath  = '/Robotics/ImageProc/IO/DisplaySinkModule',
  bbnick       = 'hermes',
  subscribertopicfilters = {
    'Image' : 'Auto000003',
  },
  checkertopicfilters = {
    'Label' : '',
  },
  postertopics = {
    'MouseClick' : '',
    'KeyboardPress' : '',
  },
  position = (690, -546),
)

nrtlib.loadModule(
  logicalpath  = '/Robotics/Navigation/GlobalPlannerModule',
  bbnick       = 'hermes',
  parameters = {
    'goal' : 'goal',
    'robot' : 'robot',
    'segmentLength' : '0.200000003',
    'showDebugMap' : 'true',
    'transform' : 'carrot',
    'updateRate' : '5',
  },
  checkertopicfilters = {
    'OccupancyMap' : 'Auto000003',
    'PixelsPerMeter' : 'Auto000004',
  },
  postertopics = {
    'TransformLookup' : 'Auto000005',
    'NextTransform' : 'Auto000006',
  },
  position = (664, -279),
)

nrtlib.loadModule(
  logicalpath  = '/Robotics/Utils/TransformPosterModule',
  bbnick       = 'hermes',
  parameters = {
    'from' : 'world',
    'rate' : '1',
    'to' : 'goal',
    'transform' : '0,.5,0,0,0,0',
  },
  postertopics = {
    'TransformUpdate' : 'Auto000007',
  },
  position = (71, 1010),
)

nrtlib.loadModule(
  logicalpath  = '/Robotics/Utils/TransformManagerModule',
  bbnick       = 'hermes',
  parameters = {
    'maxCacheSize' : '1000',
    'maxCacheTime' : '30',
  },
  subscribertopicfilters = {
    'TransformUpdatePort' : 'World2RobotTransform|Robot2KinectTransform|Auto000006|Auto000007',
    'TransformLookupPort' : 'World2KinectLookup|Auto000000|Auto000005',
  },
  position = (1043, 370),
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
  position = (72, 621),
)

nrtlib.loadModule(
  logicalpath  = '/Robotics/PointCloud/WorldDisplayModule',
  bbnick       = 'hermes',
  parameters = {
    'base' : 'world',
    'framerate' : '10',
    'transforms' : 'kinect,robot,goal,carrot',
  },
  checkertopicfilters = {
    'Cloud' : 'WorldPointCloud',
  },
  postertopics = {
    'TransformLookup' : 'Auto000000',
  },
  position = (-231, -423),
)

nrtlib.loadModule(
  logicalpath  = '/Robotics/PointCloud/DepthImageToPointCloudModule',
  bbnick       = 'hermes',
  subscribertopicfilters = {
    'Input' : 'KinectImage|Auto000001|Auto000001',
  },
  checkertopicfilters = {
    'FocalLength' : 'KinectFocalLength|Auto000002|Auto000002',
  },
  postertopics = {
    'Output' : 'RawPointCloud',
  },
  position = (-795, -457),
)

