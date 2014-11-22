import commands as c
import numpy as np
import os
import subprocess as sub
from time import sleep

spaces = 11

def justify(str_, n=spaces):
	n -= len(str_)
	return (' '*(int(n/2)+n%2))+ str_ + (' '*int(n/2))

results = []
versions = ['randtrack_global_lock', 'randtrack_tm', 'randtrack_list_lock']  

for m, exe in enumerate(versions):
	FNULL = open(os.devnull, 'w')
	sub.call("make clean && make "+exe, shell=True, stdout=FNULL, stderr=sub.STDOUT)
	FNULL.close()
	print(exe)
	results.append([])
	for i in [50, 100]:
		print(justify("* Running skipping "+str(i)+" samples", n=45))

		for j in [1, 2, 4]:
			print(justify("-> Number of threads running: "+str(j), n=60))
			l = []
			for k in range(5):
				# Is the system overloaded? Let us check the overload at the last minute
				while float(c.getoutput("cat /proc/loadavg").split(' ')[0]) > 1.0:
					sleep(5) # Let us give it some time while overload
				# Ok, now let us time the randtrack
				output = c.getoutput("/usr/bin/time --format=%e './"+exe+"' "+str(j)+" "+str(i))
				# Great! It is timed! Let us store the time
				l.append(float(output.split('\n')[-1]))
			results[m].append(np.mean(np.array(l)))
else: # It is over, let us wipe out our versions
	FNULL = open(os.devnull, 'w')
	sub.call("make clean", shell=True, stdout=FNULL, stderr=sub.STDOUT
	FNULL.close()
for i, v in enumerate(versions): # Now, it is time to print it out!
	print(v)
	print("1 thread    2 threads     4 threads".rjust(22+3*spaces+3))
	print("-------------------------------------".rjust(22+3*spaces+3))
	print("Skipping 50 samples  |"+justify(str(results[i][0]))+"|"+justify(str(results[i][1]))+"|"+justify(str(results[i][2]))+"|")
	print("Skipping 100 samples |"+justify(str(results[i][3]))+"|"+justify(str(results[i][4]))+"|"+justify(str(results[i][5]))+"|")
	print("\n\n")
