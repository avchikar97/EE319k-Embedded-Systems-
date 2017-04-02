;****************** main.s ***************
; Program written by: Akaash Chikarmane
; Date Created: 8/25/2013 
; Last Modified: 3/22/2015 
; Section 1-2pm     TA: Youngchun Kim
; Lab number: 5
; Brief description of the program
;   A traffic light controller with 3 inputs and 8 output
; Hardware connections
;The “don’t walk” and “walk” lights must be PF1 and PF3 respectively, but where to attach the others have some flexibility. 
;Obviously, you will not connect both inputs and outputs to the same pin.

;Red south       PE5
;Yellow south    PE4
;Green south     PE3
;Red west        PE2
;Yellow west     PE1
;Green west      PE0
;Table 5.1. Possible ports to interface the traffic lights (PF1=red don’t walk, PF3=green walk).

;Walk sensor     PA4
;South sensor    PA3
;West sensor     PA2
;Table 5.2. Possible ports to interface the sensors.

SYSCTL_RCGCGPIO_R       EQU 0x400FE608
GPIO_PORTA_DATA_R       EQU 0x400043FC
GPIO_PORTA_DIR_R        EQU 0x40004400
GPIO_PORTA_AFSEL_R      EQU 0x40004420
GPIO_PORTA_DEN_R        EQU 0x4000451C
GPIO_PORTB_DATA_R       EQU 0x400053FC
GPIO_PORTB_DIR_R        EQU 0x40005400
GPIO_PORTB_AFSEL_R      EQU 0x40005420
GPIO_PORTB_DEN_R        EQU 0x4000551C
GPIO_PORTD_DATA_R       EQU 0x400073FC
GPIO_PORTD_DIR_R        EQU 0x40007400
GPIO_PORTD_AFSEL_R      EQU 0x40007420
GPIO_PORTD_DEN_R        EQU 0x4000751C
GPIO_PORTE_DATA_R       EQU 0x400243FC
GPIO_PORTE_DIR_R        EQU 0x40024400
GPIO_PORTE_AFSEL_R      EQU 0x40024420
GPIO_PORTE_DEN_R        EQU 0x4002451C
GPIO_PORTF_DATA_R       EQU 0x400253FC
GPIO_PORTF_DIR_R        EQU 0x40025400
GPIO_PORTF_AFSEL_R      EQU 0x40025420
GPIO_PORTF_DEN_R        EQU 0x4002551C
NVIC_ST_CURRENT_R       EQU 0xE000E018
NVIC_ST_CTRL_R          EQU 0xE000E010
NVIC_ST_RELOAD_R        EQU 0xE000E014


      AREA   DATA, ALIGN=2


      ALIGN          
      AREA    |.text|, CODE, READONLY, ALIGN=2
      THUMB
      EXPORT  Start
      IMPORT  PLL_Init

Start 
; the DCM.DLL -pCM4 -dLaunchPadDLL debugger setting does NOT grade or simulate traffic
; the DCM.DLL -pCM4 -dEE319KLab5 debugger setting DOES not test all specifications for EE319K
     BL   PLL_Init    ; running at 80 MHz

loop  
	  B    loop



      ALIGN          ; make sure the end of this section is aligned
      END            ; end of file
      