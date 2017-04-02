// Lab9.c
// Runs on LM4F120 or TM4C123
// Student names: change this to your names or look very silly
// Last modification date: change this to the last modification date or look very silly
// Last Modified: 4/12/2016 

// Analog Input connected to PE2=ADC1
// displays on Sitronox ST7735
// PF3, PF2, PF1 are heartbeats
// UART1 on PC4-5
// * Start with where you left off in Lab8. 
// * Get Lab8 code working in this project.
// * Understand what parts of your main have to move into the UART1_Handler ISR
// * Rewrite the SysTickHandler
// * Implement the s/w FiFo on the receiver end 
//    (we suggest implementing and testing this first)

#include <stdint.h>
#include "ST7735.h"
#include "TExaS.h"
#include "ADC.h"
#include "print.h"
#include "tm4c123gh6pm.h"
#include "UART.h"
#include "FiFo.h"

//*****the first three main programs are for debugging *****
// main1 tests just the ADC and slide pot, use debugger to see data
// main2 adds the LCD to the ADC and slide pot, ADC data is on Nokia
// main3 adds your convert function, position data is no Nokia

void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
int main_fifo(void);
char datapt;

#define PF1       (*((volatile uint32_t *)0x40025008))
#define PF2       (*((volatile uint32_t *)0x40025010))
#define PF3       (*((volatile uint32_t *)0x40025020))
uint32_t Data;      // 12-bit ADC
uint32_t Position;  // 32-bit fixed-point 0.001 cm
uint32_t TxCounter = 0;


void PortF_Init(void){unsigned long volatile delay;
 	SYSCTL_RCGC2_R |= 0x21;					//also sets clock for Port A for the LCD
	uint8_t i = 0;
	i++;
	i++;
	i++;
	GPIO_PORTF_DEN_R |= 0x0E;
	GPIO_PORTF_AFSEL_R &= ~(0x0E);
	GPIO_PORTF_DIR_R |= 0x0E;
	GPIO_PORTF_DATA_R |= 0x0E;
}

// Get fit from excel and code the convert routine with the constants
// from the curve-fit
uint32_t Convert(uint32_t input){
  uint32_t converted;
	converted = input*100;
	converted = (converted + 29203);
	converted = (converted/223);

	return(converted);
}

void SysTick_Init(void){
	NVIC_ST_CTRL_R = 0;
	NVIC_ST_CURRENT_R = 0;
	NVIC_ST_RELOAD_R = 1000000;		//80MHz/(1/25ms)
	NVIC_SYS_PRI3_R = (NVIC_SYS_PRI3_R & 0x00FFFFFF) | 0x40000000;
	NVIC_ST_CTRL_R = 0x00000007;
}


// final main program for bidirectional communication
// Sender sends using SysTick Interrupt
// Receiver receives using RX
int main(void){
  TExaS_Init();       // Bus clock is 80 MHz 
  ST7735_InitR(INITR_REDTAB);
  ADC_Init();				// initialize to sample ADC1
  PortF_Init();				//heartbeat init
  UART_Init();       // initialize UART, FIFO
	ST7735_SetCursor(0,0);
  LCD_OutFix(0);
  ST7735_OutString(" cm");
	SysTick_Init();
  EnableInterrupts();
  while(1){
		uint32_t getstat = FiFo_Get(&datapt);
		if(getstat & (datapt == 0x02)){
			DisableInterrupts();
			ST7735_SetCursor(0,0);
			for(uint8_t i = 0; i<5; i++){
				FiFo_Get(&datapt);
				ST7735_OutChar(datapt);
			}
			EnableInterrupts();
		}
	}
}

	char message[8];
void SysTick_Handler(void){ // every 25 ms
	GPIO_PORTF_DATA_R ^= 0x08;		//toggle PF1
	uint32_t data = ADC_In();			//sample the ADC
	GPIO_PORTF_DATA_R ^= 0x04;		//toggle PF2
	uint32_t measurement = Convert(data);		//convert to distance (ex. 1234 = 1.234)
	
	uint32_t digit4 = (measurement%10);//digit4 = 4
	measurement = measurement/10;		//measurement = 123
	uint32_t digit3 = (measurement%10);//digit3 = 3
	measurement = measurement/10;//measurement = 12
	uint32_t digit2 = (measurement%10);//digit2 = 2
	measurement = measurement/10;//measurement = 1
	uint32_t digit1 = measurement;//digit1 = 1
	message[0] = 0x02;					//Start
	message[1] = digit1 + 0x30;	//Digit1
	message[2] = 0x2E;					//.
	message[3] = digit2 + 0x30;	//Digit2
	message[4] = digit3 + 0x30;	//Digit3
	message[5] = digit4 + 0x30;	//Digit4
	message[6] = 0x0D;					// \r
	message[7] = 0x03;					//End
	for(uint8_t i = 0; i < 8; i++){
		UART_OutChar(message[i]);
	}
	TxCounter++;
	GPIO_PORTF_DATA_R ^= 0x08;
	return;
}


uint32_t Status[20];             // entries 0,7,12,19 should be false, others true
char GetData[10];  // entries 1 2 3 4 5 6 7 8 should be 1 2 3 4 5 6 7 8
int main_fifo(void){ // Make this main to test FiFo
  FiFo_Init();
  for(;;){
    Status[0]  = FiFo_Get(&GetData[0]);  // should fail,    empty
    Status[1]  = FiFo_Put(1);            // should succeed, 1 
    Status[2]  = FiFo_Put(2);            // should succeed, 1 2
    Status[3]  = FiFo_Put(3);            // should succeed, 1 2 3
    Status[4]  = FiFo_Put(4);            // should succeed, 1 2 3 4
    Status[5]  = FiFo_Put(5);            // should succeed, 1 2 3 4 5
    Status[6]  = FiFo_Put(6);            // should succeed, 1 2 3 4 5 6
    Status[7]  = FiFo_Put(7);            // should fail,    1 2 3 4 5 6 
    Status[8]  = FiFo_Get(&GetData[1]);  // should succeed, 2 3 4 5 6
    Status[9]  = FiFo_Get(&GetData[2]);  // should succeed, 3 4 5 6
    Status[10] = FiFo_Put(7);            // should succeed, 3 4 5 6 7
    Status[11] = FiFo_Put(8);            // should succeed, 3 4 5 6 7 8
    Status[12] = FiFo_Put(9);            // should fail,    3 4 5 6 7 8 
    Status[13] = FiFo_Get(&GetData[3]);  // should succeed, 4 5 6 7 8
    Status[14] = FiFo_Get(&GetData[4]);  // should succeed, 5 6 7 8
    Status[15] = FiFo_Get(&GetData[5]);  // should succeed, 6 7 8
    Status[16] = FiFo_Get(&GetData[6]);  // should succeed, 7 8
    Status[17] = FiFo_Get(&GetData[7]);  // should succeed, 8
    Status[18] = FiFo_Get(&GetData[8]);  // should succeed, empty
    Status[19] = FiFo_Get(&GetData[9]);  // should fail,    empty
  }
}

