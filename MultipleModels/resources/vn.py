

def main():
	old = open("fuckingblender.obj", "r")
	new = open ("resultobj.obj", "w")

	for line in old:
		temp = line.split()
		if temp[0] != "vn":
			new.write(line)

if __name__ == "__main__":
	main()