import libaudioverse
import time

#initialize libaudioverse.
libaudioverse.initialize()

#make a device using the default (always stereo) output.
sim = libaudioverse.Simulation(device_index = -1)
#make a sine node.
sine_node = libaudioverse.SineNode(sim)

sine_node.frequency = 440

sine_node.connect_simulation(0)

time.sleep(5.0)

libaudioverse.shutdown()
