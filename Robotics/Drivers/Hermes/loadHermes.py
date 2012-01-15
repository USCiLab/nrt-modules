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
		logicalpath="/Robotics/Utils/SixAxisModule",
		bbnick="local",
		instancename = 'MySixAxis',
		parameters = {'joystickdev': '/dev/input/js1'},
		postertopics = {'VelocityCommand' : 'HermesVelocity'},
		position = (-200, 0))
