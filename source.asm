.org 0
ADD RX, 2		;0x0002 is the IVT entry for S. INT 1
ADD RA, 128
LSL RA, 8		;Load 0x0400 into RA
ADD RA, 32
STR RA, RX
EOR RF, RF	;Exit supervisor mode
HLT
BRI -1	;;Infinite ring loop for interrupt test
NOP
NOP
NOP
NOP
NOP
NOP
NOP
NOP
NOP
NOP
NOP
NOP
NOP
NOP
NOP
NOP
NOP
NOP
MOV RD, 100
MOV RX, 192	;Indexing register
LSL RX, 8		;Set to 0xC0000
MOV RA, 224
LSL RA, 8
ADD RA, 1	;Set RA to keyboard GPIO location
LDR RB, RD	;Load cursor location
ADD RB, 2	
STR RB, RD	;Store back into memory
ADD RX, RB	;Add terminal offset
LDB RC, RA	;Get character
STR RC, RX
RTI