// Program written by: Akaash Chikarmane
// Date Created: 1/22/2016 
// Last Modified: 2/9/2016 
// Section Tuesday 2-3
// Instructor: Ramesh Yerraballi
// Lab number: 1 (due as HW3)
// Brief description of the program
// The overall objective of this system is a digital lock
// Hardware connections
//  PE3 is switch input  (1 means switch is not pressed, 0 means switch is pressed)
//  PE4 is switch input  (1 means switch is not pressed, 0 means switch is pressed)
//  PE5 is switch input  (1 means switch is not pressed, 0 means switch is pressed)
//  PE2 is LED output (0 means door is locked, 1 means door is unlocked) 
// The specific operation of this system is to 
//   unlock if all three switches are pressed

#include <stdint.h>
#include "inc/tm4c123gh6pm.h"
 uint8_t PE3;
 uint8_t PE4;
 uint8_t PE5;

int main(void){ //initialize here
	volatile unsigned long delay;
	SYSCTL_RCGCGPIO_R |= 0x10; //Initialize clock for Port E
	delay = 100; //Wait for clock to stabilize
	SYSCTL_RCGC2_R |= 0x10;
	delay = 100; //Wait for clock to stabilize
	GPIO_PORTE_DEN_R |= 0x3C; //enable digital I/O
	GPIO_PORTE_AMSEL_R &= 0xC3; //disable analog I/O
	GPIO_PORTE_DIR_R &= ~(0x38); //PE3,4,5 as input
	GPIO_PORTE_DIR_R |= 0x04; //PE2 as output
	GPIO_PORTE_AFSEL_R &= ~(0x3C); //clear alternate function for PE2,3,4,5
	
  while(1){
	PE3 = ((~(GPIO_PORTE_DATA_R) >> 1) & 0x04);
	PE4 = ((~(GPIO_PORTE_DATA_R) >> 2) & 0x04);
	PE5 = ((~(GPIO_PORTE_DATA_R) >> 3) & 0x04);
	GPIO_PORTE_DATA_R &= 0x04;
	GPIO_PORTE_DATA_R |= (PE3 & PE4 & PE5);
	
  }
}

