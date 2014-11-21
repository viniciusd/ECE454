import commands as c
from time import sleep
import numpy as np

l = []

for i in range(5):
	# Is the system overloaded?
	while float(c.getoutput("cat /proc/loadavg").split(' ')[0]) > 1.0:
		sleep(5) # Let's give it some time while it is overloaded
	# Ok, now let us time the randtrack
	output = c.getoutput("/usr/bin/time --format=%e ./randtrack 4 50")
	# Great! It is timed! Let us store the time
	l.append(float(output.split('\n')[-1]))
print np.mean(np.array(l))
