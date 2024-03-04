N = int(input("N: "))
with open("maze.txt", "w") as f:
	f.write("X"*(2*N+1)+"\n")
	for i in range(2*N-1):
		if i == 0:
			f.write("P"+" "*(2*N-1)+"X\n")
		elif i == (2*N-2):
			f.write("X"+" "*(2*N-1)+"K\n")
		else:
			f.write("X"+" "*(2*N-1)+"X\n")

	f.write("X"*(2*N+1)+"\n")
