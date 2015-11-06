

def main():
	res = open('result.obj','w')
	f = open("cannon.obj",'r')
	for line in f:
		temp = line.split()
		if temp != 4:
			res.write(line)

if __name__ == "__main__":
	main()
