//Return immediate value of an immediate instruction
int8_t get_immediate_value(u_int16_t instruction){
	int8_t tmp = instruction & 0xFF;
	return tmp;
}

//Return register field for immediate instruction
unsigned char get_immediate_register(u_int16_t instruction){
	unsigned char tmp = (instruction & 0xF00) >> 8;
	if (tmp > 9) return 0;
	return tmp;
}

//Return single register index
unsigned char get_single_register(u_int16_t instruction){
	unsigned char tmp = instruction & 0xF;
	if (tmp > 9) return 0;
	return tmp;
}

//Return double register index (DESTINATION)
unsigned char get_double_register_dst(u_int16_t instruction){
	unsigned char tmp = (instruction & 0xF0) >> 4;
	if (tmp > 9) return 0;
	return tmp;
}

//Return double register index (SOURCE)
unsigned char get_double_register_src(u_int16_t instruction){
	unsigned char tmp = instruction & 0xF;
	if (tmp > 9) return 0;
	return tmp; 
}