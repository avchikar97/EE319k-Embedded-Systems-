; Print.s
; Student names: Akaash Chikarmane/Milan Feliciello
; Last modification date: 4/9/16
; Runs on LM4F120 or TM4C123
; EE319K lab 7 device driver for any LCD
;
; As part of Lab 7, students need to implement these LCD_OutDec and LCD_OutFix
; This driver assumes two low-level LCD functions
; ST7735_OutChar   outputs a single 8-bit ASCII character
; ST7735_OutString outputs a null-terminated string 

    IMPORT   ST7735_OutChar
    IMPORT   ST7735_OutString
	IMPORT	 UART_OutChar
    EXPORT   LCD_OutDec
    EXPORT   LCD_OutFix
	PRESERVE8

    AREA    |.text|, CODE, READONLY, ALIGN=2
    THUMB
tennum			EQU		10
onenum			EQU		1
count			EQU		3
counter			EQU		0	;local variable binding
counterfix		EQU		0
  

;-----------------------LCD_OutDec-----------------------
; Output a 32-bit number in unsigned decimal format
; Input: R0 (call by value) 32-bit unsigned number
; Output: none
; Invariables: This function must not permanently modify registers R4 to R11
LCD_OutDec
    PUSH {R4}				;allocation
    MOV R12, SP				;R12 is the frame register
    PUSH {R4-R11}
    PUSH {LR}
    LDR R4, [R12,#counter]	;R12,#counter is the counter
    MOV R4, #0
    STR R4, [R12,#counter]	;initialize local variable
again
    LDR R1,=tennum
    BL  remainder
    CMP R0, #0				;stop pushing when the last digit is in and subtracted (R0 = 0)
    BNE again
decagain					;for outputting the numbers from the stack
    LDR R4, [R12,#counter]
    CMP R4, #0
    BEQ leavedec
    POP {R0}
    BL  ST7735_OutChar
    LDR R4, [R12,#counter]
    SUB R4, #1
    STR R4, [R12,#counter]  ;decrement each time through to write correct number of digits
    B   decagain
remainder					;for putting the numbers on the stack
    UDIV R2, R0, R1			;if 627, R3 = 7 at the end
	MUL	R3,R2,R1
	SUB	R3,R0,R3
    SUB R0, R0, R3
    UDIV R0, R0, R1			;subtracts remainder and moves to least significant
    LDR R4, [R12,#counter]
    ADD R4, #1
    STR R4, [R12,#counter]  ;increment counter each time through
    ADD R3, #0x30
	PUSH {R3}
    BX  LR
leavedec
    POP {LR}
    POP {R4-R11}
    ADD SP, #4
	
      BX  LR
;* * * * * * * * End of LCD_OutDec * * * * * * * *

; -----------------------LCD _OutFix----------------------
; Output characters to LCD display in fixed-point format
; unsigned decimal, resolution 0.001, range 0.000 to 9.999
; Inputs:  R0 is an unsigned 32-bit number
; Outputs: none
; E.g., R0=0,    then output "0.000 "
;       R0=3,    then output "0.003 "
;       R0=89,   then output "0.089 "
;       R0=123,  then output "0.123 "
;       R0=9999, then output "9.999 "
;       R0>9999, then output "*.*** "
; Invariables: This function must not permanently modify registers R4 to R11
LCD_OutFix
    PUSH {R4}           ;allocation
    MOV R12, SP         ;R12 is the frame register
    PUSH {R4-R11}
    PUSH {LR}
    LDR R4, [R12,#counterfix]           ;R12,#counter is the counter
    MOV R4, #0
    STR R4, [R12,#counterfix]          ;initialize local variable to 4
	LDR R1,=tennum
	UDIV R3, R0, R1
	UDIV R3, R3, R1
	UDIV R3, R3, R1
    CMP R3, #10
    BHS stars
againfix
    LDR R1,=tennum
    BL  remainderfix
    CMP R0, #0
    BNE againfix
    LDR R4, [R12, #counterfix]
    CMP R4, #4
    BNE notyet
startprint
    MOV R4, #3
    STR R4, [R12,#counterfix]
    POP {R0}
    BL  ST7735_OutChar
    MOV R0, #0x2E           ;the decimal point
    BL  ST7735_OutChar
printfix
    LDR R4, [R12,#counterfix]
    CMP R4, #0
    BEQ leavefix
    POP {R0}
    BL  ST7735_OutChar
    LDR R4, [R12,#counterfix]
    SUB R4, #1
    STR R4, [R12,#counterfix]
    B   printfix
stars
    MOV R0, #0x2A
    BL  ST7735_OutChar
    MOV R0, #0x2E
    BL  ST7735_OutChar
    MOV R0, #0x2A
    BL  ST7735_OutChar
	MOV R0, #0x2A
    BL  ST7735_OutChar
	MOV R0, #0x2A
    BL  ST7735_OutChar
    B   leavefix
notyet
    MOV R3, #0x30
    PUSH {R3}
    LDR R4, [R12,#counterfix]
    ADD R4, #1
    STR R4, [R12,#counterfix]
    CMP R4, #4
    BNE notyet
    B   startprint
remainderfix					;for putting the numbers on the stack
    UDIV R2, R0, R1			;if 627, R3 = 7 at the end
	MUL	R3,R2,R1
	SUB	R3,R0,R3
    SUB R0, R0, R3
    UDIV R0, R0, R1			;subtracts remainder and moves to least significant
    LDR R4, [R12,#counterfix]
    ADD R4, #1
    STR R4, [R12,#counterfix]  ;increment counter each time through
    ADD R3, #0x30
	PUSH {R3}
    BX  LR
leavefix
    POP {LR}
    POP {R4-R11}
    ADD SP, #4
		
     BX   LR
 
     ALIGN
;* * * * * * * * End of LCD_OutFix * * * * * * * *

     ALIGN                           ; make sure the end of this section is aligned
     END                             ; end of file
