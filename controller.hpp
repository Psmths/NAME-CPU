
void cpu::controller(u_int16_t instruction){
	
	//HLT
	if (instruction == 0x2){
		halt = true;
	}
	
	//RET
	if (instruction == 0x3){
		registers[4] = registers[9] + 2;
		holdoff = true;
	}
	
	//RTI
	if (instruction == 0x4 && servicing == true && supervisor == true){
		return_from_interrupt();
	}
	
	//INT
	if (instruction >= 0x8 && instruction <= 0xF){
		u_int8_t user_interrupt_number = instruction & 0x7;
		u_int8_t system_interrupt_number = user_interrupt_number + 0x7;
		trigger_interrupt(system_interrupt_number);
	}
	
	//------------------------------------------------------------------------------------------------------------
	//	SINGLE REGISTER COMMANDS
	//------------------------------------------------------------------------------------------------------------
	
	//0000 0000 0001 nnnn	16-31		CLR	R[n] <- 0
	if (instruction >= 0x10 && instruction <= 0x1F){
		char r = get_single_register(instruction);
		set_register(r,0);
	}
	
	//0000 0000 0010 nnnn	32-47		INC	R[n] = R[n] + 1
	if (instruction >= 0x20 && instruction <= 0x2F){
		char r = get_single_register(instruction);
		set_register(r,registers[r]+1);
	}
	
	//0000 0000 0011 nnnn	48-63		DEC	R[n] = R[n] - 1
	if (instruction >= 0x30 && instruction <= 0x3F){
		char r = get_single_register(instruction);
		set_register(r,registers[r]-1);
	}
	
	//0000 0000 0100 nnnn	64-79		NOT	R[n] = !R[n]
	if (instruction >= 0x40 && instruction <= 0x4F){
		char r = get_single_register(instruction);
		set_register(r,!registers[r]);
	}
	
	//0000 0000 0101 nnnn	80-95		BRA	PC = PC +  2*R[n]
	if (instruction >= 0x50 && instruction <= 0x5F){
		char r = get_single_register(instruction);
		u_int16_t loc = registers[r];
		registers[4] = loc;
		holdoff = true;
	}
	
	//0000 0000 0110 nnnn	96-111		BNE	PC <- R[n] if !E
	if (instruction >= 0x60 && instruction <= 0x6F){
		char r = get_single_register(instruction);
		u_int16_t loc = registers[r];
		if ((registers[8] & 0x4) >> 2 == 0){
			registers[4] = loc;
			holdoff = true;
		}
	}
	
	//0000 0000 0111 nnnn	112-127		BRE	PC <- R[n] if E
	if (instruction >= 0x70 && instruction <= 0x7F){
		char r = get_single_register(instruction);
		u_int16_t loc = registers[r];
		if ((registers[8] & 0x4) >> 2 == 1){
			registers[4] = loc;
			holdoff = true;
		}
	}
	
	//0000 0000 1000 nnnn	128-143		BLE	PC <- R[n] if L
	if (instruction >= 0x80 && instruction <= 0x8F){
		char r = get_single_register(instruction);
		u_int16_t loc = registers[r];
		if (((registers[8] & 0x4) >> 2 == 1) || (registers[8] & 0x8) >> 3 == 1){
			registers[4] = loc;
			holdoff = true;
		}
	}
	
	//0000 0000 1001 nnnn	144-159		BGE	PC <- R[n] if !L
	if (instruction >= 0x90 && instruction <= 0x9F){
		char r = get_single_register(instruction);
		u_int16_t loc = registers[r];
		if (((registers[8] & 0x4) >> 2 == 1) || (registers[8] & 0x8) >> 3!= 1){
			registers[4] = loc;
			holdoff = true;
		}
	}
	
	//0000 0000 1010 nnnn	160-175		BLT	PC <- R[n] if Z
	if (instruction >= 0xA0 && instruction <= 0xAF){
		char r = get_single_register(instruction);
		u_int16_t loc = registers[r];
		if ((registers[8] & 0x4) == 1){
			registers[4] = loc;
			holdoff = true;
		}
	}
	
	//0000 0000 1010 nnnn	160-175		BGT	PC <- R[n] if !Z
	if (instruction >= 0xB0 && instruction <= 0xBF){
		char r = get_single_register(instruction);
		u_int16_t loc = registers[r];
		if ((registers[8] & 0x4) != 1){
			registers[4] = loc;
			holdoff = true;
		}
	}
	
	//0000 0000 1011 nnnn	178-191		PSH	m[FP] < R[n]
	if (instruction >= 0xC0 && instruction <= 0xCF){
		char r = get_single_register(instruction);
		push_register(r);
	}
	
	//0000 0000 1100 nnnn	192-207		POP	R[n] < m[SP]
	if (instruction >= 0xD0 && instruction <= 0xDF){
		char r = get_single_register(instruction);
		pop_register(r);
	}
	
	//0000 0000 1101 nnnn			CAL	RL <- PC <- R[n]
	if (instruction >= 0xE0 && instruction <= 0xEF){
		char r = get_single_register(instruction);
		u_int16_t loc = registers[r];
		registers[9] = registers[4]; //Save PC
		registers[4] = loc;
		holdoff = true;
	}
	
	//------------------------------------------------------------------------------------------------------------
	//	DOUBLE REGISTER COMMANDS
	//------------------------------------------------------------------------------------------------------------
	
	//0000 0001 xxxx yyyy	ADD	R[x] = R[x] + R[y]
	if (instruction >= 0x100 && instruction <= 0x1FF){
		char rd = get_double_register_dst(instruction);
		char rs = get_double_register_src(instruction);
		set_register(rd,registers[rd] + registers[rs]);
	}
	
	//0000 0010 xxxx yyyy	SUB	R[x] = R[x] - R[y]
	if (instruction >= 0x200 && instruction <= 0x2FF){
		char rd = get_double_register_dst(instruction);
		char rs = get_double_register_src(instruction);
		set_register(rd,registers[rd] - registers[rs]);
	}
	
	//0000 0011 xxxx yyyy	LSL	R[x] = R[x] << R[y]
	if (instruction >= 0x300 && instruction <= 0x3FF){
		char rd = get_double_register_dst(instruction);
		char rs = get_double_register_src(instruction);
		u_int16_t shamt = registers[rs];
		set_register(rd,registers[rd] << registers[rs]);
	}
	
	//0000 0100 xxxx yyyy	RSR	R[x] = R[x] >> R[y]
	if (instruction >= 0x400 && instruction <= 0x4FF){
		char rd = get_double_register_dst(instruction);
		char rs = get_double_register_src(instruction);
		u_int16_t shamt = registers[rs];
		set_register(rd,registers[rd] >> registers[rs]);
	}
	
	//0000 0101 xxxx yyyy	AND	R[x] = R[x] & R[y]
	if (instruction >= 0x500 && instruction <= 0x5FF){
		char rd = get_double_register_dst(instruction);
		char rs = get_double_register_src(instruction);
		set_register(rd,registers[rd] & registers[rs]);
	}
	
	//0000 0110 xxxx yyyy	ORR	R[x] = R[x] | R[y]
	if (instruction >= 0x600 && instruction <= 0x6FF){
		char rd = get_double_register_dst(instruction);
		char rs = get_double_register_src(instruction);
		set_register(rd,registers[rd] | registers[rs]);
	}
	
	//0000 0111 xxxx yyyy	EOR	R[x] = R[x] ^ R[y]
	if (instruction >= 0x700 && instruction <= 0x7FF){
		char rd = get_double_register_dst(instruction);
		char rs = get_double_register_src(instruction);
		set_register(rd,registers[rd] ^ registers[rs]);
	}
	
	//0000 1000 xxxx yyyy	XCH	R[x] SWAP R[y]
	if (instruction >= 0x800 && instruction <= 0x8FF){
		char rd = get_double_register_dst(instruction);
		char rs = get_double_register_src(instruction);
		u_int16_t tmp = registers[rd];
		set_register(rd,registers[rs]);
		set_register(rs,tmp);
	}
	
	//0000 1001 xxxx yyyy	MOV	R[x] <- R[y]
	if (instruction >= 0x900 && instruction <= 0x9FF){
		char rd = get_double_register_dst(instruction);
		char rs = get_double_register_src(instruction);
		set_register(rd,registers[rs]);
	}
	
	//0000 1010 xxxx yyyy	LDR	R[x] = m(R[y])
	if (instruction >= 0xA00 && instruction <= 0xAFF){
		char rd = get_double_register_dst(instruction);
		char rs = get_double_register_src(instruction);
		u_int16_t tmp = memory[registers[rs]] << 8;
		tmp = tmp + memory[registers[rs]+1];
		set_register(rd,tmp);
	}
	
	//0000 1011 xxxx yyyy	STR	m(R[x]) = R[y]
	if (instruction >= 0xB00 && instruction <= 0xBFF){
		unsigned char rd = get_double_register_dst(instruction);
		unsigned char rs = get_double_register_src(instruction);
		memory[registers[rs]+1] = registers[rd] & 0x00FF;
		memory[registers[rs]] = (registers[rd] & 0xFF00) >> 8;
	}
	
	//0000 1100 xxxx yyyy		LDB		R[x] = m(R[y])
	if (instruction >= 0xC00 && instruction <= 0xCFF){
		char rd = get_double_register_dst(instruction);
		char rs = get_double_register_src(instruction);
		u_int16_t tmp = memory[registers[rs]];
		set_register(rd,tmp);
	}
	
	//0000 1101 xxxx yyyy		STB		m(R[x]) = R[y]
	if (instruction >= 0xD00 && instruction <= 0xDFF){
		unsigned char rd = get_double_register_dst(instruction);
		unsigned char rs = get_double_register_src(instruction);
		memory[registers[rs]] = registers[rd] & 0x00FF;
	}
	
	//------------------------------------------------------------------------------------------------------------
	//	IMMEDIATE COMMANDS
	//------------------------------------------------------------------------------------------------------------
	
	//0001 #### xxxx xxxx	4096-8191	BRI	PC <- 2*x
	if (instruction >= 0x1000 && instruction <= 0x1FFF){
		int8_t imm = get_immediate_value(instruction);
		registers[4] = registers[4] + 2*imm;
		holdoff = true;
	}
	
	//0010 rrrr xxxx xxxx	ADD	R[r] = R[r] + x
	if (instruction >= 0x2000 && instruction <= 0x2FFF){
		u_int8_t imm = get_immediate_value(instruction);
		unsigned char ri = get_immediate_register(instruction);
		set_register(ri,registers[ri] + imm);
	}
	
	//0011 rrrr xxxx xxxx	SUB	R[r] = R[r] - x
	if (instruction >= 0x3000 && instruction <= 0x3FFF){
		u_int8_t imm = get_immediate_value(instruction);
		unsigned char ri = get_immediate_register(instruction);
		set_register(ri,registers[ri] - imm);
	}
	
	//0100 rrrr xxxx xxxx	LSL	R[r] = R[r] << x
	if (instruction >= 0x4000 && instruction <= 0x4FFF){
		u_int8_t imm = get_immediate_value(instruction);
		unsigned char ri = get_immediate_register(instruction);
		set_register(ri,registers[ri] << imm);
	}
	
	//0101 rrrr xxxx xxxx	RSR	R[r] = R[r] >> x
	if (instruction >= 0x5000 && instruction <= 0x5FFF){
		u_int8_t imm = get_immediate_value(instruction);
		unsigned char ri = get_immediate_register(instruction);
		set_register(ri,registers[ri] >> imm);
	}
	
	//0110 rrrr xxxx xxxx	AND	R[r] = R[r] & x
	if (instruction >= 0x6000 && instruction <= 0x6FFF){
		u_int8_t imm = get_immediate_value(instruction);
		unsigned char ri = get_immediate_register(instruction);
		set_register(ri,registers[ri] & imm);
	}
	
	//0111 rrrr xxxx xxxx	ORR	R[r] = R[r] | x
	if (instruction >= 0x7000 && instruction <= 0x7FFF){
		u_int8_t imm = get_immediate_value(instruction);
		unsigned char ri = get_immediate_register(instruction);
		set_register(ri,registers[ri] | imm);
	}
	
	//1000 rrrr xxxx xxxx	EOR	R[r] = R[r] ^ x
	if (instruction >= 0x8000 && instruction <= 0x8FFF){
		u_int8_t imm = get_immediate_value(instruction);
		unsigned char ri = get_immediate_register(instruction);
		set_register(ri,registers[ri] ^ imm);
	}
	
	//1001 rrrr xxxx xxxx	MOV	R[r] = x
	if (instruction >= 0x9000 && instruction <= 0x9FFF){
		u_int8_t imm = get_immediate_value(instruction);
		unsigned char ri = get_immediate_register(instruction);
		set_register(ri,imm);
	}
	
	//1010 rrrr xxxx xxxx	CMP	Compare R[r] and x (R[r] - x)
	if (instruction >= 0xA000 && instruction <= 0xAFFF){
		
		//Only clear lower byte of FLAGS register
		registers[8] = registers[8] & 0xFF00;
		
		u_int8_t imm = get_immediate_value(instruction);
		unsigned char ri = get_immediate_register(instruction);
		
		u_int16_t operand = registers[ri];
		int16_t tmp = operand - imm;
		
		if (tmp == 0) registers[8] = (registers[8] | 0x4); //ZERO
		if (tmp < 0) registers[8] = (registers[8] | 0x8); //NEGATIVE
		
	}
	
	
}

//MMU checks
void cpu::set_register(char r, u_int16_t value){
	//Check if supervisor mode is ON for PC, BP, FP, RF!
	if (r == 0x4 || r == 0x8 || r == 0xA){
		if (supervisor == true){
			registers[r] = value;
			return;
		}else{
			trigger_interrupt(0);//Privilege violation
			return;
		}
	}else{
		registers[r] = value;
		return;		
	}
}
