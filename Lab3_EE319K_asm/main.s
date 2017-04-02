;****************** main.s ***************
; Program written by: Akaash Chikarmane
; Date Created: 1/22/2016 
; Last Modified: 2/22/2016 
; Section: Tuesday 2-3
; Instructor: Ramesh Yerraballi
; Lab number: 3
; Brief description of the program
;   If the switch is presses, the LED toggles at 8 Hz
; Hardware connections
;  PE1 is switch input  (1 means pressed, 0 means not pressed)
;  PE0 is LED output (1 activates external LED on protoboard) 
;Overall functionality of this system is the similar to Lab 2, with six changes:
;1-  the pin to which we connect the switch is moved to PE1, 
;2-  you will have to remove the PUR initialization because pull up is no longer needed. 
;3-  the pin to which we connect the LED is moved to PE0, 
;4-  the switch is changed from negative to positive logic, and 
;5-  you should increase the delay so it flashes about 8 Hz.
;6-  the LED should be on when the switch is not pressed
; Operation
;   1) Make PE0 an output and make PE1 an input. 
;   2) The system starts with the LED on (make PE0 =1). 
;   3) Wait about 62 ms
;   4) If the switch is pressed (PE1 is 1), then toggle the LED once, else turn the LED on (0). 
;   5) Steps 3 and 4 are repeated over and over


GPIO_PORTE_DATA_R       EQU   0x400243FC
GPIO_PORTE_DIR_R        EQU   0x40024400
GPIO_PORTE_AFSEL_R      EQU   0x40024420
GPIO_PORTE_DEN_R        EQU   0x4002451C
GPIO_PORTE_AMSEL_R      EQU   0x40024528
GPIO_PORTE_PCTL_R       EQU   0x4002452C
SYSCTL_RCGCGPIO_R       EQU   0x400FE608
       IMPORT  TExaS_Init
       AREA    |.text|, CODE, READONLY, ALIGN=2
       THUMB
       EXPORT  Start
Start
 ; TExaS_Init sets bus clock at 80 MHz
      BL  TExaS_Init ; voltmeter, scope on PD3
	LDR R0,=SYSCTL_RCGCGPIO_R
	LDR R1, [R0]
	ORR R1, #0x10			;enable clock for Port E (0001 0000)
	STR R1, [R0]
	NOP						;stabilize clock
	NOP

	LDR R0,=GPIO_PORTE_DEN_R
	LDR R1, [R0]
	ORR R1, #0x03			;enable digital I/O for PE3, PF4
	STR R1, [R0]
	
	LDR R0,=GPIO_PORTE_DIR_R
	LDR R1, [R0]
	ORR R1, #0x01			;PE0=output (1)
	BIC R1, #0x02			;PE1=input (0)
	STR R1, [R0]
	
	LDR R0,=GPIO_PORTE_AFSEL_R
	LDR R1, [R0]
	BIC R1, #0x03			;no alternate function for PE3,4
	STR R1, [R0]
	
	LDR R0,=GPIO_PORTE_DATA_R
	LDR R1, [R0]
	ORR R1, #0x01			;LED initially on
	STR R1, [R0]

      CPSIE  I    ; TExaS voltmeter, scope runs on interrupts

loop  
	BL	DELAY
	
	LDR R0,=GPIO_PORTE_DATA_R
	LDR R1, [R0]			;read the switch (R1 = adjusting PE0)
	MOV R2, R1				;R2 = checking PE1
	MOV R3, R1				;R3 = original data
	AND R2, #0x02			;clear all but PE1
	SUBS R2, #0x02
	
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
	   
DELAY
	LDR	R0,=COUNT		;start of delay subroutine (62 ms)
	LDR	R1, [R0]
REPEAT
	SUBS R1, R1, #1
	BNE REPEAT
	BX	LR

COUNT	DCD		0x130000
	



      ALIGN      ; make sure the end of this section is aligned
      END        ; end of file
       