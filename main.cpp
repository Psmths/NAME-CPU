#include <stdlib.h>
#include <iostream>
#include <string>
#include <algorithm>
#include <vector>
#include <array>
#include <cstring>
#include "cpu.hpp"
#include "ui.hpp"
#include "mnemonic.hpp"
#include "controller.hpp"

int main(){
	//Interface windows
	WINDOW *registers, *dissassembly, *memory, *helpscreen, *status, *terminal;
	
	//Simulation parameters
	uint16_t pos_m = 0x8000;		//Memory dump position
	uint16_t pos_d = 0x7FF0;		//Disassembly position
	bool show_helpscreen = false;		//If true, show HELP panel
	bool single_step = true;		//If true, require SPACE to advance CPU
	bool locked_d = false;			//Lock disassembly view to PC
	bool locked_m = false;			//Lock memory view to PC
	bool show_m = true;			//Show memory
	bool hover = false; 			//Focus on: 0 - disassembly, 1 - memory dump
	bool catch_keyboard = false; 		//If true, all keys pressed trigger interrupts
	
	int counter = 0;			//Holdoff updating screen for speed
	
	//Graphics setup
	initscr();
	raw();
	start_color();
	cbreak();
	noecho();
	timeout(-1);
	keypad(stdscr, TRUE);
	mouseinterval(0);
	
	//Color pairs
	init_pair(1, COLOR_YELLOW, COLOR_BLACK);
	init_pair(2, COLOR_BLACK, COLOR_GREEN);
	init_pair(3, COLOR_RED, COLOR_BLACK);
	init_pair(4, COLOR_WHITE, COLOR_BLUE);
	init_pair(5, COLOR_WHITE, COLOR_MAGENTA);
	init_pair(6, COLOR_WHITE, COLOR_RED);
	init_pair(6, COLOR_BLACK, COLOR_RED);
	
	//CPU setup
	cpu acpu;
	acpu.load_rom();		//Load image
	acpu.registers[4] = 0x8000;	//Set PC to 0x8000
	acpu.registers[5] = 0x7FFF;	//Set FP
	acpu.registers[6] = 0x7FFF;	//Set BP
	acpu.registers[8] = 0x0100;	//Enter supervisor mode to FLAGS
	acpu.halt = true;		//Halt CPU
	acpu.supervisor = true;		//Enter supervisor
	
	while(1){
		
		//Lock/unlock memory dump
		if (locked_m){
			pos_m = acpu.registers[4] - acpu.registers[4]%16;
		}else{
			pos_m = pos_m;
		}
		
		//Lock/unlock disassembly
		if (locked_d){
			pos_d = acpu.registers[4] - acpu.registers[4]%16;
		}else{
			pos_d = pos_d;
		}
		
		assume_default_colors(COLOR_YELLOW,COLOR_BLACK);
		
		//Show or hide screen/memory dump
		if (show_m) {
			update_memory_dump(memory, acpu, pos_m, locked_m, hover);
		}else{
			if (counter >= 100){
				clear_memory_dump(memory);
				update_screen(terminal, acpu);
				counter = 0;
			}else{ counter++; }				
		}
		
		//Update all the other panels
		update_registers(registers, acpu);
		update_dissassembly(dissassembly, acpu, pos_d, locked_d, hover);
		update_status(status, acpu,catch_keyboard);

		refresh();
		
		if (single_step){
			timeout(-1);
		}else{
			timeout(1);
			acpu.clock(false);
		}
		
		if (!catch_keyboard){
			//Menu options...
			char ch = getch();
			if (ch == 'q') break;
			if (ch == 'w') { 
				if (!hover) {
					pos_m = pos_m - 0x100; 
				}else{
					pos_d = pos_d - 0x2; 
				}
			}
			if (ch == 'W') { 
				if (!hover) {
					pos_m = pos_m - 0x1000; 
				}else{
					pos_d = pos_d - 0x14; 
				}
			}
			if (ch == 's') { 
				if (!hover) {
					pos_m = pos_m + 0x100; 
				}else{
					pos_d = pos_d + 0x2; 
				}
			}
			if (ch == 'S') { 
				if (!hover) {
					pos_m = pos_m + 0x1000; 
				}else{
					pos_d = pos_d + 0x14; 
				}
			}
			if (ch == 'h') acpu.halt = !acpu.halt;
			if (ch == 'r') { acpu.reset = true; acpu.clock(false); }
			if (ch == 'o') locked_d = !locked_d;
			if (ch == 'p') locked_m = !locked_m;
			if (ch == 'e') hover = !hover;
			if (ch == 'm') show_m = !show_m;
			if (ch == '?') update_helpscreen(helpscreen);
			if (ch == 'f' && single_step){
				single_step = false;
				timeout(1);
			}else if (ch == 'f' && !single_step){
				single_step = true;
				timeout(-1);
			}
			if (ch == 'b') acpu.toggle_bp(pos_d);
			if (ch == ' ') acpu.clock(false);
			if (ch == 't') acpu.trigger_interrupt(1);
			if (ch == 'c') {acpu.clock(true); }
			if (ch == 27) catch_keyboard = true;
			
		}else{
			char ch = getch();
			
			if (ch == 27) catch_keyboard = false;
			
			if (ch >= 32 && ch <= 127 && acpu.servicing == false){
				acpu.memory[0xE001] = ch;
				acpu.trigger_interrupt(1);
			}
		}
	}
	endwin();
	return 0;
}

