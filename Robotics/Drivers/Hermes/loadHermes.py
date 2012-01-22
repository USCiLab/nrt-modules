import nrtlib
import os

nrtlib.loadModule(
		logicalpath="/Robotics/Drivers/HermesModule",
		bbnick="local",
		instancename = 'CoolHermes',
		parameters = {'serialdev' : '/dev/rfcomm0'},
		subscribertopicfilters = {'VelocityCommand' : 'HermesVelocity'},
		position = (200, 0))

nrtlib.loadModule(
		logicalpath="/Robotics/Utils/iNRTJoystickModule",
		bbnick="local",
		instancename = 'MyiPhone',
		postertopics = {'VelocityCommand' : 'HermesVelocity'},
		position = (-200, 200))

#nrtlib.loadModule(
#		logicalpath="/Robotics/Utils/ThreeSixtyControllerModule",
#		bbnick="local",
#		instancename = 'MySixAxis',
#		parameters = {'joystickdev': '/dev/input/js1'},
#		postertopics = {'VelocityCommand' : 'HermesVelocity'},
#		position = (-200, 0))
