// Program written by: Akaash Chikarmane (avc536)
// Date Created: 1/22/2016 
// Last Modified: 1/22/2016 
// Section ***Tuesday 1-2***update this***
// Instructor: ***Ramesh Yerraballi**update this***
// Lab number: 4
// Brief description of the program
//   If the switch is presses, the LED toggles at 8 Hz
// Hardware connections
//  PE1 is switch input  (1 means pressed, 0 means not pressed)
//  PE0 is LED output (1 activates external LED on protoboard) 
//Overall functionality of this system is the similar to Lab 3, with three changes:
//1-  initialize SysTick with RELOAD 0x00FFFFFF 
//2-  add a heartbeat to PF2 that toggles every time through loop 
//3-  add debugging dump of input, output, and time
// Operation
//	1) Make PE0 an output and make PE1 an input. 
//	2) The system starts with the LED on (make PE0 =1). 
//   3) Wait about 62 ms
//   4) If the switch is pressed (PE1 is 1), then toggle the LED once, else turn the LED on. 
//   5) Steps 3 and 4 are repeated over and over

#include <stdint.h>
#include "inc/tm4c123gh6pm.h"
#include "systick.h"
#define SIZE 50
void Init(void);
void TExaS_Init(void);
void EnableInterrupts(void);
void Delay(void);
void Debug_Init(void);
void Debug_Capture(void);
uint32_t DataBuffer[50];
uint32_t TimeBuffer[50];
uint32_t *DataPt;
uint32_t *TimePt;
uint32_t delayvalue;
uint8_t count;
uint8_t initcount;
volatile uint32_t Rasputin;
volatile uint32_t Putin;

int main(void){ 
  TExaS_Init(); // 80 MHz and PD3 is scope/meter
	Debug_Init();
	EnableInterrupts();
	Init();
  while(1){
	Delay();
	Debug_Capture();
	GPIO_PORTF_DATA_R ^= 0x04;
	if((GPIO_PORTE_DATA_R & 0x02) - 0x02 == 0){
		GPIO_PORTE_DATA_R ^= 0x01;
	}
	else{
		GPIO_PORTE_DATA_R |= 0x01;
	}
  }
}

void Init(void){
	SYSCTL_RCGCGPIO_R |= 0x30;//clock on for PE, PF
	while((SYSCTL_RCGCGPIO_R & 0x30) != 0x30){};
	GPIO_PORTE_DEN_R |= 0x03;
	GPIO_PORTE_DIR_R |= 0x01;
	GPIO_PORTE_DIR_R &= 0xFD;
	GPIO_PORTE_AFSEL_R &= ~(0x03);
	GPIO_PORTE_DATA_R |= 0x01;
	
	GPIO_PORTF_DIR_R |= 0x04;
	GPIO_PORTF_DEN_R |= 0x04;
	GPIO_PORTF_AFSEL_R &= ~(0x04);
	GPIO_PORTF_DATA_R |= 0x04;
	
	count = 0;
}

void Delay(void){
	delayvalue = 0x48888;
	while(delayvalue > 0){
		delayvalue = delayvalue - 1;
	}
}

void Debug_Init(void){
	DataPt = &DataBuffer[0];
	TimePt = &TimeBuffer[0];
	initcount = 0;
	for(initcount=0;initcount<50;initcount++){
		DataBuffer[initcount] = 0xFFFFFFFF;
		TimeBuffer[initcount] = 0xFFFFFFFF;
	}
	NVIC_ST_CTRL_R = 0;
	NVIC_ST_CURRENT_R = 0;
	NVIC_ST_RELOAD_R = 0x00FFFFFF;
	NVIC_ST_CTRL_R = 5;
}

void Debug_Capture(void){
	if(count<50){
	*DataPt = (((GPIO_PORTE_DATA_R << 3) & 0x10) | (GPIO_PORTE_DATA_R &0x01));
	DataPt++;
	*TimePt = NVIC_ST_CURRENT_R;
	TimePt++;
	}
	count++;
}
