;****************** main.s ***************
; Program written by: Akaash Chikarmane/Milan Feliciello
; Date Created: 2/22/2016 
; Last Modified: 2/29/2016 
; Section Tuesday 2-3
; Instructor: Ramesh Yerraballi
; Lab number: 4
; Brief description of the program
;   If the switch is presses, the LED toggles at 8 Hz
; Hardware connections
;  PE1 is switch input  (1 means pressed, 0 means not pressed)
;  PE0 is LED output (1 activates external LED on protoboard) 
;Overall functionality of this system is the similar to Lab 3, with three changes:
;1-  initialize SysTick with RELOAD 0x00FFFFFF 
;2-  add a heartbeat to PF2 that toggles every time through loop 
;3-  add debugging dump of input, output, and time
; Operation
;	1) Make PE0 an output and make PE1 an input. 
;	2) The system starts with the LED on (make PE0 =1). 
;   3) Wait about 62 ms
;   4) If the switch is pressed (PE1 is 1), then toggle the LED once, else turn the LED on. 
;   5) Steps 3 and 4 are repeated over and over


SWITCH                  EQU 0x40024004  ;PE0
LED                     EQU 0x40024008  ;PE1
SYSCTL_RCGCGPIO_R       EQU 0x400FE608
SYSCTL_RCGC2_GPIOE      EQU 0x00000010   ; port E Clock Gating Control
SYSCTL_RCGC2_GPIOF      EQU 0x00000020   ; port F Clock Gating Control
GPIO_PORTE_DATA_R       EQU 0x400243FC
GPIO_PORTE_DIR_R        EQU 0x40024400
GPIO_PORTE_AFSEL_R      EQU 0x40024420
GPIO_PORTE_PUR_R        EQU 0x40024510
GPIO_PORTE_DEN_R        EQU 0x4002451C
GPIO_PORTF_DATA_R       EQU 0x400253FC
GPIO_PORTF_DIR_R        EQU 0x40025400
GPIO_PORTF_AFSEL_R      EQU 0x40025420
GPIO_PORTF_DEN_R        EQU 0x4002551C
NVIC_ST_CTRL_R          EQU 0xE000E010
NVIC_ST_RELOAD_R        EQU 0xE000E014
NVIC_ST_CURRENT_R       EQU 0xE000E018
count EQU 0x131000
countone EQU 50
counttwo EQU 50
FILLone EQU 0xFFFFFFFF
FILLtwo EQU 0xFFFFFFFF
           THUMB
           AREA    DATA, ALIGN=4
SIZE       EQU    50
;You MUST use these two buffers and two variables
;You MUST not change their names
;These names MUST be exported
           EXPORT DataBuffer  
           EXPORT TimeBuffer  
           EXPORT DataPt [DATA,SIZE=4] 
           EXPORT TimePt [DATA,SIZE=4]

DataBuffer SPACE SIZE*4
TimeBuffer SPACE SIZE*4
DataPt SPACE 4
TimePt SPACE 4
    
      ALIGN  
	  PRESERVE8
      AREA    |.text|, CODE, READONLY, ALIGN=2
      THUMB
      EXPORT  Start
      IMPORT  TExaS_Init
	  IMPORT  SysTick_Init


Start BL   TExaS_Init
;Initializes memory dump and initializes SysTick
	  BL Debug_Init; running at 80 MHz, scope voltmeter on PD3
;Port E Initialization
	LDR R1, =SYSCTL_RCGCGPIO_R		;initialize port E
	LDR R0, [R1]
	ORR R0, R0, #0x10
	STR R0, [R1]
	NOP
	NOP
	NOP
	NOP
	LDR R1, =GPIO_PORTE_DEN_R
	ORR R0, #0x03
	STR R0, [R1]
	LDR R1, =GPIO_PORTE_AFSEL_R
	BIC R0, #0x03
	STR R0, [R1]
	LDR R1, =GPIO_PORTE_DIR_R		;pin 0 output, pin 1 input
	ORR R0, #0x01
	BIC R0, #0x02
	STR R0, [R1]
;Port F Initialization
	LDR R1, =SYSCTL_RCGCGPIO_R
	LDR R0, [R1]
	ORR R0, #0x20
	STR R0, [R1]
	NOP
	NOP
	LDR R1, =GPIO_PORTF_DIR_R
	LDR R0, [R0]
	ORR R0, #0x04
	STR R0, [R1]
	LDR R1, =GPIO_PORTF_AFSEL_R
	LDR R0, [R1]
	AND R0, #0xFB
	STR R0, [R1]
	LDR R1, =GPIO_PORTF_DEN_R
	LDR R0, [R1]
	ORR R0, #0x04
	STR R0, [R1]

	
      CPSIE  I    ; TExaS voltmeter, scope runs on interrupts

loop	
	BL   Debug_Capture
	BL	delay
;heartbeat
	LDR R0,=GPIO_PORTF_DATA_R
	LDR R1, [R0]
	MOV R2, R1				;R2 = original data
	AND R1, #0x04			;clear all but PF2
	EOR R1, #0x04			;toggle PF2
	BIC R2, #0x04
	ORR R2, R1, R2
	STR R2, [R0]

;input PE1 test output PE0
	LDR R0,=GPIO_PORTE_DATA_R
	LDR R1, [R0]			;read the switch (R1 = adjusting PE0)
	MOV R2, R1				;R2 = checking PE1
	MOV R3, R1				;R3 = original data
	AND R2, #0x02			;clear all but PE1
	SUBS R2, #0x02			;branch taken if PE1 = 0
	
	BNE	SETON				;if PE1 = 0, turn on LED
	AND R1, #0x01			;clear all but PE0
	EOR R1, #0x01			;toggle PE0
	BIC R3, #0x01			;clear original PE0
	ORR R3, R1, R3			;recombine
	STR R3, [R0]
	B		loop

SETON
	ORR R3, #0x01
	STR R3, [R0]
	  B    loop



delay 
	LDR R2, =count
	
again
	SUBS R2,R2, #0x01
	BNE again
	BX LR


	
;------------Debug_Init------------
; Initializes the debugging instrument
; Input: none
; Output: none
; Modifies: none
; Note: push/pop an even number of registers so C compiler is happy
Debug_Init
	PUSH{R14}
	PUSH{R0,R12}
	LDR R1, =DataBuffer			;initialize pointers by filling them with starting locations
	LDR R0, =DataPt
	STR R1, [R0]
	LDR R1, =TimeBuffer
	LDR R0, =TimePt
	STR R1, [R0]
	
	LDR R0, =countone 	;R0 is the count
	LDR R2, =FILLone
	LDR R1, =DataPt
	LDR R1, [R1]
	
againinit 
			;pointer
	STR R2, [R1]			;fill DataBuffer array with 0xFFFFFFFF
	ADD R1, #0x04
	SUB R0, #0x01
	CMP R0, #0
	BNE againinit
	LDR R0, =counttwo
	LDR R2, =FILLtwo
	LDR R1, =TimePt
	LDR R1, [R1]
	
againinittwo 
	

	STR R2, [R1]			;fill TimeBuffer array with 0xFFFFFFFF
	ADD R1, #0x04
	SUB R0, #0x01
	CMP R0, #0
	BNE againinittwo
	
	BL SysTick_Init
	POP{R0,R12}
	POP{R14}
	BX LR
	

;------------Debug_Capture------------
; Dump Port E and time into buffers
; Input: none
; Output: none
; Modifies: none
; Note: push/pop an even number of registers so C compiler is happy
Debug_Capture
	PUSH{R14}
	PUSH{R0,R12}
	LDR R0, =countone
	LDR R1, [R0] ;If we have already filled the array we return to main
	SUB R1, #0x01
	CMP R1, #0
	BLS stop
	STR R1, [R0]
	LDR R1, =GPIO_PORTE_DATA_R 
	LDR R0, [R1]
	MOV R2, R0
	AND R2, #0x02
	AND R0, #0x01
	LSL R2, #3
	ORR R0, R0, R2				;R0 is data to store
	LDR R1, =DataPt
	LDR R2, [R1]
	STR R0, [R2]
	ADD R2, #0x04
	STR R2, [R1]
	LDR R1, =NVIC_ST_CURRENT_R
	LDR R0, [R1]
	LDR R2, =TimePt
	LDR R3, [R2]
	STR R0, [R3]

	ADD R3, #0x04
	STR R3, [R2]
stop  
	POP{R0,R12}
	POP{R14}
	BX LR
	



    ALIGN                           ; make sure the end of this section is aligned
    END                             ; end of file
        