import re
import struct

source = open("source.asm", "r")
asm = open("rom.bin", "wb")

image = bytearray()

sr = ["CLR","INC","DEC","NOT","BRA","BNE","BRE","BLE","BGE","BRZ","BNZ","PSH","POP","CAL"]
dr = ["ADD","SUB","LSL","RSR","AND","ORR","EOR","XCH","MOV","LDR","STR"]
ir = ["XXX", "ADD","SUB","LSL","RSR","AND","ORR","EOR","MOV","CMP"]
r = ["RA","RB","RC","RD","PC","FP","BP","RX","RF","RL",]

for line in source:
	
	offset = re.search('^.org', line)
	if (offset):
		tmp = line.split(" ")
		print("Offset is: " + tmp[1])
		for x in range(2*int(tmp[1])):
			image.append(0)
			
	for m in sr:
		s_found = re.search('^'+m, line)
		if (s_found):
			tmp = line.strip().split(" ")
			ind_m = sr.index(tmp[0]) + 1
			ind_r = r.index(tmp[1])
			op_r = (ind_m << 4) + ind_r
			image.append(0)
			image.append(op_r)
			
	for m in dr:
		s_found = re.search('^'+m, line)
		if (s_found):
			tmp = line.strip().split()
			tmp[1] = tmp[1].replace(',','')
			ind_r_d = r.index(tmp[1])
			try:
				ind_r_s = r.index(tmp[2])
			except:
				#This is an immediate register command!
				ind_m = ir.index(tmp[0])
				imm = int(tmp[2])
				op_r_h = ((ind_m + 1) << 4) + (ind_r_d) 
				op_r_l = imm
				image.append(op_r_h)
				image.append(op_r_l) 
				break
				
			ind_m = dr.index(tmp[0])
			op_r_h = (ind_m) + 1
			op_r_l = (ind_r_d << 2) + ind_r_s
			print(line)
			print (op_r_h)
			print (op_r_l)
			image.append(op_r_h)
			image.append(op_r_l)
			
	s_found = re.search('^BRI', line)
	if (s_found):
		tmp = line.strip().split()
		image.append(16)
		if re.search('^-', tmp[1]):
			image.append(256-abs(int(tmp[1])))
		else:
			image.append(abs(int(tmp[1])))
			
	offset = re.search('^.data', line)
	if (offset):
		tmp = line.split(" ")
		print("Data segment offset is: " + tmp[1])
		for x in range(2*int(tmp[1])):
			image.append(0)
			
	offset = re.search('^.string', line)
	if (offset):
		tmp = line.strip().split(':')
		image.extend(map(ord, tmp[1]))
		image.append(0)
asm.write(image)
