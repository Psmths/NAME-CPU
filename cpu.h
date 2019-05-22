#include "decoder.h"
class cpu{
	public:
		//Registers
		//0  1  2  3  4  5  6  7  8  9 A
		//ra,rb,rc,rd,pc,fp,bp,rx,rf,rl,sp
		u_int16_t registers [16];
	
		//Control
		bool reset;
		bool halt;
		bool supervisor;
		bool holdoff;
		bool servicing = false;
	
		//Memory
		u_int8_t memory[65535];
	
		void load_rom();
		std::string instruction_to_mnemonic(u_int16_t instruction);
		void clock(bool skip_bp);
		void controller(u_int16_t instruction);
	
		void push_register(char r);
		void pop_register(char r);
		
		void enter_supervisor();
		void exit_supervisor(); //#TODO
	
		//MMU Functions
		void set_register(char r, u_int16_t value);
		void write_memory_byte(u_int8_t value, u_int16_t address);//#TODO
		void write_memory_word(u_int16_t value, u_int16_t address);//#TODO
		
		//Interrupts
		void trigger_interrupt(u_int8_t interrupt);
		void return_from_interrupt();//#TODO
		
		//Simulation storage
		std::vector<u_int16_t> breakpoints; //Breakpoint storage
		void toggle_bp(u_int16_t pos);
};

//Load ROM file
void cpu::load_rom(){
	FILE *file = fopen("rom.bin", "rb");
	
	u_int8_t *buffer;
	int size;

	fseek(file, 0L, SEEK_END);
	size = ftell(file);
	rewind(file);

	//Limit to ROM size of 16K
	if (size > 0x4000){
		size = 0x4000;
		std::cout << "ROM file truncated!" << std::endl;
	}
	//dont laugh this was ported from C >:|
	buffer = static_cast<u_int8_t*>(calloc(1,size+1));
	fread(buffer, size, 1, file);

	for (int i=32768; i<size+32768; i++){
		memory[i] = buffer[i-32768];
	}
}

#include "mnemonic.h"

//skip_bp ignores breakpoints
void cpu::clock(bool skip_bp){
	if (reset) {
		for (int i = 0; i < 9; i++){
			registers[i] = 0x0;
		}
		registers[4] = 0x8000;
		registers[5] = 0x7FFF;
		registers[6] = 0x7FFF;
		registers[8] = 0x0100;
		reset = false;
		holdoff = true;
	}
	//Are we at a breakpoint?
	std::vector<u_int16_t>::iterator it = std::find(breakpoints.begin(), breakpoints.end(), registers[4]);
	//If so, halt execution
	if (it != breakpoints.end() && !skip_bp){
		halt = true;
		return;
	}else if (it != breakpoints.end() && skip_bp){
		halt = false;
	}
	if (halt) return; //Do nothing
	//Fetch instruction
	u_int16_t tmpinst = memory[registers[4]] << 8;
	tmpinst = tmpinst + memory[registers[4]+1];
	//Send to controller
	controller(tmpinst);
	//Check if supervisor bit has been set to 0!
	if ((registers[8] & 256) == 0){
		supervisor = false;
	}
	//Increment PC
	if (!holdoff) registers[4] = registers[4] + 2;
	//Reset holdoff
	if (holdoff) holdoff = false;
}

#include "controller.h"

void cpu::push_register(char r){
	u_int16_t fp = registers[5];
	memory[fp] = registers[r];
	memory[fp-1] = registers[r] >> 8;
	registers[5] = registers[5] - 2;
}

void cpu::pop_register(char r){
	if (registers[5] < registers[6]){
		registers[5] = registers[5] + 2;
		u_int16_t fp = registers[5];
		set_register(r,memory[fp]);
		set_register(r,registers[r] + (memory[fp-1] << 8));
	}
}

void cpu::toggle_bp(u_int16_t pos){
	//Does the breakpoint already exist?
	std::vector<u_int16_t>::iterator it = std::find(breakpoints.begin(), breakpoints.end(), pos);
	//If so, delete it
	if (it != breakpoints.end()){
		breakpoints.erase(it);
		return;
	}
	breakpoints.push_back(pos);
	return;
}

void cpu::enter_supervisor(){
	//Switch to ring 0
	supervisor = true;
	registers[8] = registers[8] | 256;
	
	//Push user FP to stack at 0x1000
	memory[0x0FFF] = registers[5] & 0x00FF;
	memory[0x0FFE] = (registers[5] & 0xFF00) >> 8;
	
	//Push FLAGS
	memory[0x0FFD] = registers[8] & 0x00FF;
	memory[0x0FFC] = (registers[8] & 0xFF00) >> 8;
	
	//Push PC
	memory[0x0FFB] = registers[4] & 0x00FF;
	memory[0x0FFA] = (registers[4] & 0xFF00) >> 8;
	
	//Set FP to 0x0FFE
	registers[5] = 0x0FF9;
	
	//Set BP to 0x1000
	registers[6] = 0x0FFF;
	return;
}

void cpu::exit_supervisor(){
	registers[4] = memory[0x0FFB];
	registers[4] = registers[4] + (memory[0x0FFA] << 8);
	
	registers[8] = memory[0x0FFD];
	registers[8] = registers[8] + (memory[0x0FFC] << 8);
	
	registers[6] = 0x7FFF;
	//Return FP
	registers[5] = memory[0x0FFF];
	registers[5] = registers[5] + (memory[0x0FFE] << 8);
	//Switch to ring 1
	registers[8] = registers[8] & 0xFEFF;
	supervisor = false;
	holdoff = true;
} 
//0  1  2  3  4  5  6  7  8  9  A
//ra,rb,rc,rd,pc,fp,bp,rx,rf,rl,sp
void cpu::trigger_interrupt(u_int8_t interrupt){
	servicing = true;
	halt = false;
	//Get IVT address
	u_int16_t ivt_pos = memory[interrupt*2] << 8;
	ivt_pos = ivt_pos + memory[(interrupt*2)+1];
	
	//Enter supervisor mode
	enter_supervisor();
	
	//Jump to ISR
	registers[4] = ivt_pos;
	
	return;
}

void cpu::return_from_interrupt(){
	exit_supervisor();
	servicing = false;
}

