import os
l = [1,2,4,8]
for n in l:
	print(n)
	os.system("make clean > out.o")
	os.system("time make -j"+str(n)+" > j"+str(n)+".txt")
	os.system("make clean > out.o")
	os.system("time make -j"+str(n)+" >> j"+str(n)+".txt")
	os.system("make clean > out.o")
	os.system("time make -j"+str(n)+" >> j"+str(n)+".txt")	
	os.system("make clean > out.o")
	os.system("time make -j"+str(n)+" >> j"+str(n)+".txt")	
	os.system("make clean > out.o")
	os.system("time make -j"+str(n)+" >> j"+str(n)+".txt")
	os.system("make clean > out.o")
