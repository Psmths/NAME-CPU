# NAME-CPU
NAME is a 16-bit CPU simulator that provides the following:

1. ncurses interface
2. keyboard input
3. text-only screen for output
4. simple debugging features such as breakpoints and single-stepping 

The simulation offers several more advanced features such as:

1. Interrupts (Software/Hardware)
2. Interrupt Vector Table functionality
3. Privilege levels (Ring 0 Supervisor / Ring 1 User modes)
4. Memory protection (a la GPT)
5. Memory-mapped I/O

Currently, a very rudimentary assembler written in Python is included, though its functionality is limited as it does not support labels and good variable declaration support.
