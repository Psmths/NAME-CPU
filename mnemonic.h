#include <sstream>

std::string cpu::instruction_to_mnemonic(u_int16_t instruction){
	std::string register_names[16] = {"RA", "RB", "RC", "RD", "PC", "FP", "BP", "RX", "RF", "RL"};
	
	std::ostringstream tmpm;
	
	if (instruction == 0x0){
		return "NOP";
	}
	
	if (instruction == 0x2){
		return "HLT";
	}
	
	if (instruction == 0x3){
		return "RET";
	}

	if (instruction == 0x4){
		return "RTI";
	}
	
	if (instruction >= 0x8 && instruction <= 0xF){
		u_int8_t user_interrupt_number = instruction & 0x7;
		tmpm << "INT" << std::to_string(static_cast<int>(user_interrupt_number));
		return tmpm.str();
	}

	//------------------------------------------------------------------------------------------------------------
	//	SINGLE REGISTER COMMANDS
	//------------------------------------------------------------------------------------------------------------
	
	//0000 0000 0001 nnnn	16-31		CLR	R[n] <- 0
	if (instruction >= 0x10 && instruction <= 0x1F){
		char r = get_single_register(instruction);
		tmpm << "CLR " << register_names[r];
		return tmpm.str();
	}
	
	//0000 0000 0010 nnnn	32-47		INC	R[n] = R[n] + 1
	if (instruction >= 0x20 && instruction <= 0x2F){
		char r = get_single_register(instruction);
		tmpm << "INC " << register_names[r];
		return tmpm.str();
	}	
	//0000 0000 0011 nnnn	48-63		DEC	R[n] = R[n] - 1
	if (instruction >= 0x30 && instruction <= 0x3F){
		char r = get_single_register(instruction);
		tmpm << "DEC " << register_names[r];
		return tmpm.str();
	}	
	
	//0000 0000 0100 nnnn	64-79		NOT	R[n] = !R[n]
	if (instruction >= 0x40 && instruction <= 0x4F){
		char r = get_single_register(instruction);
		tmpm << "NOT " << register_names[r];
		return tmpm.str();
	}	
	
	//0000 0000 0101 nnnn	80-95		BRA	PC = PC +  2*R[n]
	if (instruction >= 0x50 && instruction <= 0x5F){
		char r = get_single_register(instruction);
		int16_t loc = registers[r];
		tmpm << "BRA " << register_names[r] << " ;#" << std::to_string(static_cast<int>(loc));
		return tmpm.str();
	}
	
	//0000 0000 0110 nnnn	96-111		BNE	PC <- R[n] if !E
	if (instruction >= 0x60 && instruction <= 0x6F){
		char r = get_single_register(instruction);
		int16_t loc = registers[r];
		tmpm << "BNE " << register_names[r] << " ;#" << std::to_string(static_cast<int>(loc));
		return tmpm.str();
	}
	
	//0000 0000 0111 nnnn	112-127		BRE	PC <- R[n] if E
	if (instruction >= 0x70 && instruction <= 0x7F){
		char r = get_single_register(instruction);
		int16_t loc = registers[r];
		tmpm << "BRE " << register_names[r] << " ;#" << std::to_string(static_cast<int>(loc));;
		return tmpm.str();
	}
	
	//0000 0000 1000 nnnn	128-143		BLE	PC <- R[n] if L
	if (instruction >= 0x80 && instruction <= 0x8F){
		char r = get_single_register(instruction);
		int16_t loc = registers[r];
		tmpm << "BLE " << register_names[r] << " ;#" << std::to_string(static_cast<int>(loc));
		return tmpm.str();
	}
	
	//0000 0000 1001 nnnn	144-159		BGE	PC <- R[n] if !L
	if (instruction >= 0x90 && instruction <= 0x9F){
		char r = get_single_register(instruction);
		int16_t loc = registers[r];
		tmpm << "BGE " << register_names[r] << " ;#" << std::to_string(static_cast<int>(loc));;
		return tmpm.str();
	}
	
	//0000 0000 1010 nnnn	160-175		BRZ	PC <- R[n] if Z
	if (instruction >= 0xA0 && instruction <= 0xAF){
		char r = get_single_register(instruction);
		int16_t loc = registers[r];
		tmpm << "BRZ " << register_names[r] << " ;#" << std::to_string(static_cast<int>(loc));;
		return tmpm.str();
	}
	
	//0000 0000 1010 nnnn	160-175		BNZ	PC <- R[n] if Z
	if (instruction >= 0xB0 && instruction <= 0xBF){
		char r = get_single_register(instruction);
		int16_t loc = registers[r];
		tmpm << "BNZ " << register_names[r] << " ;#" << std::to_string(static_cast<int>(loc));;
		return tmpm.str();
	}
	
	//0000 0000 1011 nnnn	178-191		PSH	m[SP] < R[n]
	if (instruction >= 0xC0 && instruction <= 0xCF){
		char r = get_single_register(instruction);
		tmpm << "PSH " << register_names[r];
		return tmpm.str();
	}
	
	//0000 0000 1100 nnnn	192-207		POP	R[n] < m[SP]
	if (instruction >= 0xD0 && instruction <= 0xDF){
		char r = get_single_register(instruction);
		tmpm << "POP " << register_names[r];
		return tmpm.str();
	}	
	
	//0000 0000 1101 nnnn			CAL	RL <- PC <- R[n]
	if (instruction >= 0xE0 && instruction <= 0xEF){
		char r = get_single_register(instruction);
		int16_t loc = registers[r];
		tmpm << "CAL " << register_names[r] << " ;#" << std::to_string(static_cast<int>(loc));
		return tmpm.str();
	}
	
	//------------------------------------------------------------------------------------------------------------
	//	DOUBLE REGISTER COMMANDS
	//------------------------------------------------------------------------------------------------------------
	
	//0000 0001 xxxx yyyy	ADD	R[x] = R[x] + R[y]
	if (instruction >= 0x100 && instruction <= 0x1FF){
		char rd = get_double_register_dst(instruction);
		char rs = get_double_register_src(instruction);
		tmpm << "ADD " << register_names[rd] << ", " << register_names[rs];
		return tmpm.str();
	}
	
	//0000 0010 xxxx yyyy	SUB	R[x] = R[x] - R[y]
	if (instruction >= 0x200 && instruction <= 0x2FF){
		char rd = get_double_register_dst(instruction);
		char rs = get_double_register_src(instruction);
		tmpm << "SUB " << register_names[rd] << ", " << register_names[rs];
		return tmpm.str();
	}
	
	//0000 0011 xxxx yyyy	LSL	R[x] = R[x] << R[y]
	if (instruction >= 0x300 && instruction <= 0x3FF){
		char rd = get_double_register_dst(instruction);
		char rs = get_double_register_src(instruction);
		u_int16_t shamt = registers[rs];
		tmpm << "LSL " << register_names[rd] << ", " << register_names[rs] << ";#" << std::to_string(static_cast<int>(shamt));
		return tmpm.str();
	}
	
	//0000 0100 xxxx yyyy	RSR	R[x] = R[x] >> R[y]
	if (instruction >= 0x400 && instruction <= 0x4FF){
		char rd = get_double_register_dst(instruction);
		char rs = get_double_register_src(instruction);
		u_int16_t shamt = registers[rs];
		tmpm << "RSR " << register_names[rd] << ", " << register_names[rs] << ";#" << std::to_string(static_cast<int>(shamt));
		return tmpm.str();
	}
	
	//0000 0101 xxxx yyyy	AND	R[x] = R[x] & R[y]
	if (instruction >= 0x500 && instruction <= 0x5FF){
		char rd = get_double_register_dst(instruction);
		char rs = get_double_register_src(instruction);
		tmpm << "AND " << register_names[rd] << ", " << register_names[rs];
		return tmpm.str();
	}
	
	//0000 0110 xxxx yyyy	ORR	R[x] = R[x] | R[y]
	if (instruction >= 0x600 && instruction <= 0x6FF){
		char rd = get_double_register_dst(instruction);
		char rs = get_double_register_src(instruction);
		tmpm << "ORR " << register_names[rd] << ", " << register_names[rs];
		return tmpm.str();
	}
	
	//0000 0111 xxxx yyyy	EOR	R[x] = R[x] ^ R[y]
	if (instruction >= 0x700 && instruction <= 0x7FF){
		char rd = get_double_register_dst(instruction);
		char rs = get_double_register_src(instruction);
		tmpm << "EOR " << register_names[rd] << ", " << register_names[rs];
		return tmpm.str();
	}
	
	//0000 1000 xxxx yyyy	XCH	R[x] SWAP R[y]
	if (instruction >= 0x800 && instruction <= 0x8FF){
		char rd = get_double_register_dst(instruction);
		char rs = get_double_register_src(instruction);
		tmpm << "XCH " << register_names[rd] << ", " << register_names[rs];
		return tmpm.str();
	}
	
	//0000 1001 xxxx yyyy	MOV	R[x] <- R[y]
	if (instruction >= 0x900 && instruction <= 0x9FF){
		char rd = get_double_register_dst(instruction);
		char rs = get_double_register_src(instruction);
		tmpm << "MOV " << register_names[rd] << ", " << register_names[rs];
		return tmpm.str();
	}
	
	//0000 1010 xxxx yyyy	LDR	R[x] = m(R[y])
	if (instruction >= 0xA00 && instruction <= 0xAFF){
		char rd = get_double_register_dst(instruction);
		char rs = get_double_register_src(instruction);
		tmpm << "LDR " << register_names[rd] << ", [" << register_names[rs] << "]";
		return tmpm.str();
	}
	
	//0000 1011 xxxx yyyy	STR	m(R[x]) = R[y]
	if (instruction >= 0xB00 && instruction <= 0xBFF){
		char rd = get_double_register_dst(instruction);
		char rs = get_double_register_src(instruction);
		tmpm << "STR " << register_names[rd] << ", [" << register_names[rs] << "]";
		return tmpm.str();
	}
	
	//0000 1100 xxxx yyyy		LDB		R[x] = m(R[y])
	if (instruction >= 0xC00 && instruction <= 0xCFF){
		char rd = get_double_register_dst(instruction);
		char rs = get_double_register_src(instruction);
		tmpm << "LDB " << register_names[rd] << ", [" << register_names[rs] << "]";
		return tmpm.str();
	}
	
	//0000 1101 xxxx yyyy		STB		m(R[x]) = R[y]
	if (instruction >= 0xD00 && instruction <= 0xDFF){
		char rd = get_double_register_dst(instruction);
		char rs = get_double_register_src(instruction);
		tmpm << "STB " << register_names[rd] << ", [" << register_names[rs] << "]";
		return tmpm.str();
	}
	
	//------------------------------------------------------------------------------------------------------------
	//	IMMEDIATE COMMANDS
	//------------------------------------------------------------------------------------------------------------
	
	//0001 #### xxxx xxxx	4096-8191	BRI	PC <- x
	if (instruction >= 0x1000 && instruction <= 0x1FFF){
		int8_t imm = get_immediate_value(instruction);
		tmpm << "BRI #" << std::to_string(static_cast<int>(imm*2));
		return tmpm.str();
	}
	
	//0010 rrrr xxxx xxxx	ADD	R[r] = R[r] + x
	if (instruction >= 0x2000 && instruction <= 0x2FFF){
		u_int8_t imm = get_immediate_value(instruction);
		unsigned char ri = get_immediate_register(instruction);
		tmpm << "ADD " << register_names[ri] << ", #" << std::to_string(static_cast<int>(imm));
		return tmpm.str();
	}
	
	//0011 rrrr xxxx xxxx	SUB	R[r] = R[r] - x
	if (instruction >= 0x3000 && instruction <= 0x3FFF){
		u_int8_t imm = get_immediate_value(instruction);
		unsigned char ri = get_immediate_register(instruction);
		tmpm << "SUB " << register_names[ri] << ", #" << std::to_string(static_cast<int>(imm));
		return tmpm.str();
	}
	
	//0100 rrrr xxxx xxxx	LSL	R[r] = R[r] << x
	if (instruction >= 0x4000 && instruction <= 0x4FFF){
		u_int8_t imm = get_immediate_value(instruction);
		unsigned char ri = get_immediate_register(instruction);
		tmpm << "LSL " << register_names[ri] << ", #" << std::to_string(static_cast<int>(imm));
		return tmpm.str();
	}
	
	//0101 rrrr xxxx xxxx	RSR	R[r] = R[r] >> x
	if (instruction >= 0x5000 && instruction <= 0x5FFF){
		u_int8_t imm = get_immediate_value(instruction);
		unsigned char ri = get_immediate_register(instruction);
		tmpm << "RSR " << register_names[ri] << ", #" << std::to_string(static_cast<int>(imm));
		return tmpm.str();
	}
	
	//0110 rrrr xxxx xxxx	AND	R[r] = R[r] & x
	if (instruction >= 0x6000 && instruction <= 0x6FFF){
		u_int8_t imm = get_immediate_value(instruction);
		unsigned char ri = get_immediate_register(instruction);
		tmpm << "AND " << register_names[ri] << ", #" << std::to_string(static_cast<int>(imm));
		return tmpm.str();
	}
	
	//0111 rrrr xxxx xxxx	ORR	R[r] = R[r] | x
	if (instruction >= 0x7000 && instruction <= 0x7FFF){
		u_int8_t imm = get_immediate_value(instruction);
		unsigned char ri = get_immediate_register(instruction);
		tmpm << "ORR " << register_names[ri] << ", #" << std::to_string(static_cast<int>(imm));
		return tmpm.str();
	}
	
	//1000 rrrr xxxx xxxx	EOR	R[r] = R[r] ^ x
	if (instruction >= 0x8000 && instruction <= 0x8FFF){
		u_int8_t imm = get_immediate_value(instruction);
		unsigned char ri = get_immediate_register(instruction);
		tmpm << "EOR " << register_names[ri] << ", #" << std::to_string(static_cast<int>(imm));
		return tmpm.str();
	}
	
	//1001 rrrr xxxx xxxx	MOV	R[r] = x
	if (instruction >= 0x9000 && instruction <= 0x9FFF){
		u_int8_t imm = get_immediate_value(instruction);
		unsigned char ri = get_immediate_register(instruction);
		tmpm << "MOV " << register_names[ri] << ", #" << std::to_string(static_cast<int>(imm));
		return tmpm.str();
	}
	
	//1010 rrrr xxxx xxxx	CMP	Compare R[r] and x
	if (instruction >= 0xA000 && instruction <= 0xAFFF){
		u_int8_t imm = get_immediate_value(instruction);
		unsigned char ri = get_immediate_register(instruction);
		tmpm << "CMP " << register_names[ri] << ", #" << std::to_string(static_cast<int>(imm));
		return tmpm.str();
	}
	
	return "ILLEGAL";
}