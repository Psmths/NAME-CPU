#include <curses.h>
#include <cstring>
#include <form.h>

//Create a new window and return its pointer
WINDOW *create_window(int height, int width, int posx, int posy){
	WINDOW *tmpwin;
	tmpwin = newwin(height, width, posx, posy);
	wattron(tmpwin, COLOR_PAIR(1));
	box(tmpwin,0,0);
	wrefresh(tmpwin);
	return tmpwin;
}

void update_helpscreen(WINDOW *helpscreen){
	int h = 20;
	int w = 90;
	int y = (LINES - h)/2;
	int x = (COLS - w)/2;
	clear();
	refresh();
	helpscreen = create_window(h,w,y,x);
	mvwprintw(helpscreen, 0, 1, "HELP");
	mvwprintw(helpscreen, 1, 1, "The following are a list of commands supported by the NAME user interface:");
	mvwprintw(helpscreen, 3, 1, "\tq\t-\tQuit program");
	mvwprintw(helpscreen, 4, 1, "\tr\t-\tSignal RESET");
	mvwprintw(helpscreen, 5, 1, "\th\t-\tEnter HALT");
	mvwprintw(helpscreen, 6, 1, "\tb\t-\tToggle breakpoint at disassembly cursor position");
	mvwprintw(helpscreen, 7, 1, "\te\t-\tSwitch focus between memory dump and disassembly");
	mvwprintw(helpscreen, 8, 1, "\tf\t-\tToggle single step");
	mvwprintw(helpscreen, 9, 1, "\to\t-\tLock/Unlock disassembly view to PC");
	mvwprintw(helpscreen, 10, 1, "\tp\t-\tLock/Unlock memory dump to PC");
	mvwprintw(helpscreen, 11, 1, "\tw/W\t-\tIncrement focused view");
	mvwprintw(helpscreen, 12, 1, "\ts/S\t-\tDecrement focused view");
	mvwprintw(helpscreen, 13, 1, "\tSPACE\t-\tSingle step PC");
	mvwprintw(helpscreen, 14, 1, "\tc\t-\tContinue execution at breakpoint");
	mvwprintw(helpscreen, 15, 1, "\tm\t-\tSwitch between screen and memory dump");
	mvwprintw(helpscreen, 18, 1, "Press any key to return...");
	wmove(helpscreen, h/2, 36);
	timeout(-1);
	wrefresh(helpscreen);
	refresh();
	getch(); //Any key to exit 
	noecho();
	wborder(helpscreen, ' ', ' ', ' ',' ',' ',' ',' ',' ');
	wrefresh(helpscreen);
	delwin(helpscreen);
	clear();
	refresh();
	timeout(100);
}

void update_registers(WINDOW *registers, cpu cpu_pointer){
	std::string register_names[] = {"RA", "RB", "RC", "RD", "PC", "FP", "BP", "RX", "RF", "RL","SP"};
	registers = create_window(12,20,1,1);
	mvwprintw(registers, 0, 1, "REGISTERS");
	
	for (auto i = 0 ; i < sizeof(register_names)/sizeof(register_names[0]); i++){
		mvwprintw(registers, 1+i, 1, "%s: 0x%04x", register_names[i].c_str(), cpu_pointer.registers[i]);
	}
	wrefresh(registers);
	
}

void update_status(WINDOW *status, cpu cpu_pointer, bool catch_keyboard){

	
	status = create_window(12,20,13,1);

	mvwprintw(status, 0, 1, "STATUS");
	
	if (cpu_pointer.halt){
		wattron(status,COLOR_PAIR(6));
		mvwprintw(status, 1, 1, "HALT: ON");
	}else{
		wattron(status,COLOR_PAIR(1));
		mvwprintw(status, 1, 1, "HALT: OFF");
	}
	
	if (cpu_pointer.supervisor){
		wattron(status,COLOR_PAIR(5));
		mvwprintw(status, 2, 1, "MODE: SUPERVISOR");
		wattron(status,COLOR_PAIR(1));		
	}else{
		wattron(status,COLOR_PAIR(1));
		mvwprintw(status, 2, 1, "MODE: USER");		
	}
	
	u_int16_t flags = cpu_pointer.registers[8];
	
	mvwprintw(status, 4, 1, "CARRY:\t\t%d", flags & 0x1);
	mvwprintw(status, 5, 1, "OVERFLOW:\t%d", (flags & 0x2)>>1);
	mvwprintw(status, 6, 1, "ZERO:\t\t%d", (flags & 0x4)>>2);
	mvwprintw(status, 7, 1, "NEGATIVE:\t%d", (flags & 0x8)>>3);
	
	if (catch_keyboard){
		mvwprintw(status, 9, 1, "MODE: KEYBOARD");
	}else{
		mvwprintw(status, 9, 1, "MODE: DEBUG");
	}
	
	
	u_int16_t tmpinst = cpu_pointer.memory[cpu_pointer.registers[4]] << 8;
	tmpinst = tmpinst + cpu_pointer.memory[cpu_pointer.registers[4]+1];
	
	wattron(status,COLOR_PAIR(1));
	
	wrefresh(status);
	
}

void clear_memory_dump(WINDOW *memoryview){
	wborder(memoryview, ' ', ' ', ' ',' ',' ',' ',' ',' ');
	wrefresh(memoryview);
	delwin(memoryview);
	clear();
	refresh();
}

void update_memory_dump(WINDOW *memoryview, cpu cpu_pointer, u_int16_t pos, bool locked, bool hover){
	memoryview = create_window(35,95,1,60);
	wattron(memoryview,COLOR_PAIR(1));
	if (!hover) wattron(memoryview,COLOR_PAIR(5));
	if (locked) mvwprintw(memoryview,0,1,"MEMORY [LOCKED,    toggle: p]");
	if (!locked) mvwprintw(memoryview,0,1,"MEMORY [UNLOCKED, toggle: p]");
	wattron(memoryview,COLOR_PAIR(1));
	mvwprintw(memoryview,1,1,"ADDR.      0   1   2   3   4   5   6   7   8   9   A   B   C   D   E   F");	
	for (int i = 2; i < 34; i++){
		wattron(memoryview, COLOR_PAIR(1));
		mvwprintw(memoryview, i,1,"0x%04x",pos+((i-2)*16));
		for (int j = 0; j < 16; j++){
			uint8_t tmpb = cpu_pointer.memory[pos+((i-2)*16)+j];
			if (pos+((i-2)*16)+j > cpu_pointer.registers[5] && pos+((i-2)*16)+j <= cpu_pointer.registers[6]){
				wattron(memoryview,COLOR_PAIR(4));
				mvwprintw(memoryview,i,9+(j*4),"  %02x  ", tmpb);
			}
			else if (pos+((i-2)*16)+j == cpu_pointer.registers[5]){
				wattron(memoryview,COLOR_PAIR(5));
				mvwprintw(memoryview,i,9+(j*4),"  %02x  ", tmpb);
				wattron(memoryview,COLOR_PAIR(1));
				mvwprintw(memoryview,i,9+(j*4),"  ", tmpb);
			}
			else{
				wattron(memoryview,COLOR_PAIR(1));
				mvwprintw(memoryview,i,9+(j*4),"  %02x  ", tmpb);
			}
			
			wattron(memoryview,COLOR_PAIR(1));
			if ((tmpb >= 0 && tmpb <= 31) || (tmpb > 127)){
				mvwprintw(memoryview,i,74+(j),".");
			}else{
				mvwprintw(memoryview,i,74+(j),"%c", tmpb);
			}
			
			
			
		}
	}

	
	wattron(memoryview, COLOR_PAIR(1));
	wmove(memoryview, 1, 7);
	wvline(memoryview, 0,33);	
	wrefresh(memoryview);
}

void update_dissassembly(WINDOW *dissassembly, cpu cpu_pointer, u_int16_t pos, bool locked, bool hover){
	dissassembly = create_window(35,37,1,22);
	wattron(dissassembly,COLOR_PAIR(1));
	if (hover) wattron(dissassembly,COLOR_PAIR(5));
	if (locked) mvwprintw(dissassembly,0,1,"DISSASSEMBLY [LOCKED,    toggle: o]");
	if (!locked) mvwprintw(dissassembly,0,1,"DISSASSEMBLY [UNLOCKED, toggle: o]");
	
	for (int i = 0; i < 33; i++){
		int tmpc = cpu_pointer.registers[4]+(i*2);
		if (!locked) tmpc = pos+(i*2);
		u_int16_t tmpinst = cpu_pointer.memory[tmpc] << 8;
		tmpinst = tmpinst + cpu_pointer.memory[tmpc+1];
		std::string tmpmnem = cpu_pointer.instruction_to_mnemonic(tmpinst);
		
		//Breakpoint scan
		std::vector<u_int16_t>::iterator it = std::find(cpu_pointer.breakpoints.begin(), cpu_pointer.breakpoints.end(), tmpc);
		
		wattron(dissassembly, COLOR_PAIR(1));
		if (!strcmp(tmpmnem.c_str(), "ILLEGAL")) wattron(dissassembly, COLOR_PAIR(3));
		else if (tmpc == cpu_pointer.registers[4]) wattron(dissassembly, COLOR_PAIR(2));
		else if (it != cpu_pointer.breakpoints.end()) wattron(dissassembly, COLOR_PAIR(6));
		else wattron(dissassembly, COLOR_PAIR(1));
		
		mvwprintw(dissassembly, i+1, 1, "0x%04x  %04x\t%s", tmpc, tmpinst, tmpmnem.c_str());
	}
	wattron(dissassembly, COLOR_PAIR(1));
	wmove(dissassembly, 1, 7);
	wvline(dissassembly, 0,33);
	wrefresh(dissassembly);
}

void update_screen(WINDOW *screen, cpu cpu_pointer){
	wattron(screen,COLOR_PAIR(1));
	screen = create_window(35,95,1,60);
	wattron(screen,COLOR_PAIR(1));
	mvwprintw(screen,0,1,"SCREEN");
	//33 * 93 = 3069
	int screen_memory_begin = 0xC000;
	
	
	for (int i = 0; i < 6138; i=i+2){
		u_int16_t blob = (cpu_pointer.memory[screen_memory_begin + i] << 8);
		blob = blob + cpu_pointer.memory[screen_memory_begin + i + 1];
		char character = blob & 0xFF;
		
		u_int16_t posx = (i/2) % 93;
		u_int16_t posy = (i/2)/93;
		
		mvwprintw(screen,1+posy,1+posx,"%c",character);
		
	}
	
	wrefresh(screen);
}