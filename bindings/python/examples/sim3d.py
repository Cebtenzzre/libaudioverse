#demonstrates how to use the 3d simulation.
import libaudioverse
import collections
libaudioverse.initialize()

sim = libaudioverse.Simulation(device_index = -1)
world = libaudioverse.SimpleEnvironmentNode(sim, "default")
source = libaudioverse.SourceNode(sim, world)
print "Enter a path to a sound file."
filepath = raw_input()
f = libaudioverse.FileNode(sim, filepath)
f.looping = True

f.connect(0, source, 0)
world.connect_simulation(0)

print """Enter python expressions that evaluate to 3-tuples (x, y, z).
Positive x is to your right, positive y is above you, and positive z is behind you.
Enter quit to quit."""
while True:
	command = raw_input()
	if command == 'quit':
		break
	vect = eval(command)
	if not isinstance(vect, collections.Sized) or len(vect) != 3:
		print "Must evaluate to a 3-tuple.  Try again"
		continue
	source.position = vect

libaudioverse.shutdown()