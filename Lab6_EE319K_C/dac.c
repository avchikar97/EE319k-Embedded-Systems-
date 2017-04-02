// dac.c
// This software configures DAC output
// Runs on LM4F120 or TM4C123
// Program written by: Akaash Chikarmane
// Date Created: 8/25/2014 
// Last Modified: 3/17/2015 
// Section 2-3pm     TA: Wooseok Lee
// Lab number: 6
// Hardware connections
// TO STUDENTS "REMOVE THIS LINE AND SPECIFY YOUR HARDWARE********

#include <stdint.h>
#include "tm4c123gh6pm.h"
//#include "Lab6.c"
#include "globals.h"

// Code files contain the actual implemenation for public functions
// this file also contains an private functions and private data

// **************DAC_Init*********************
// Initialize 4-bit DAC, called once 
// Input: none
// Output: none
void DAC_Init(void){
	SYSCTL_RCGC2_R |= 0x02;//initialize Port B
	while(SYSCTL_RCGC2_R == 0){
	}
	GPIO_PORTB_AFSEL_R &= 0xF0;
	GPIO_PORTB_DIR_R  |= 0x0F;//Port B 0-3 are output
	GPIO_PORTB_DEN_R |= 0x0F;
	GPIO_PORTB_DR8R_R |=0x0F;
}

// **************DAC_Out*********************
// output to DAC
// Input: 4-bit data, 0 to 15 
// Output: none
//variable definitions
void DAC_Out(uint32_t Data){
	GPIO_PORTB_DATA_R = Data;
}
