// UART.c
// Runs on LM4F120/TM4C123
// Provide functions that setup and interact with UART
// Last Modified: 4/12/2016 
// Student names: Akaash Chikarmane/Milan Feliciello
// Last modification date: 4/14/16

#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "FiFo.h"
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts

// UART initialization function 
// Input: none
// Output: none
void UART_Init(void){
	uint8_t i = 0;
	FiFo_Init();
	SYSCTL_RCGCUART_R |= 0x0002;		//activate UART1
	while(SYSCTL_RCGCUART_R == 0){};
	i++;
	i++;
	i++;
	SYSCTL_RCGC2_R |= 0x04;			//activate Port C
	while(SYSCTL_RCGC2_R == 0){};
	i++;
	i++;
	i++;
	UART1_CTL_R &= ~0x0001;					//disable UART
	UART1_IBRD_R = 50;							//(8e+7)/(16*100000) = 50
	UART1_FBRD_R = 0;
	UART1_LCRH_R = 0x0070;					//enable 8-bit word length
	UART1_IM_R |= 0x10;							//arm RXRIS
	UART1_IFLS_R &= (UART1_IFLS_R & ~0x00000038) | 0x10;		//set RXRIS when FIFO goes from 7 to 8 characters
	NVIC_PRI1_R = (NVIC_PRI1_R & ~0x00E00000) | 0x00400000;	//clear bits 21-23 and set bit 23
	NVIC_EN0_R = NVIC_EN0_INT6;
	UART1_CTL_R = 0x0301;						//enable RXE, TXE, and UART1	
	GPIO_PORTC_AFSEL_R |= 0x30;		//enable alt function on PC4, PC5
	GPIO_PORTC_PCTL_R = (GPIO_PORTC_PCTL_R & ~0x00FF0000) + 0x00220000;	//U1Rx = PC4, U1Tx = PC5
	GPIO_PORTC_AMSEL_R &= ~0x30;		//disable analog I/O on PC4, PC5
	GPIO_PORTC_DEN_R |= 0x30;				//enable digital I/O on PC4, PC5
}

//------------UART_OutChar------------
// Wait for new input,
// then return ASCII code
// Input: none
// Output: char read from UART
void UART_OutChar(char data){  
	while((UART1_FR_R & 0x20) != 0){
	}
	UART1_DR_R = data;
}

void static copyHardwareToSoftware (void){
	char letter;
	uint8_t i=0;
	for(i=0;i<8;i++){
		letter = UART1_DR_R;
		FiFo_Put(letter);
	}
}

	uint32_t RxCounter = 0;
void UART1_Handler (void){
	DisableInterrupts();
	UART1_ICR_R =0x10;
	GPIO_PORTF_DATA_R ^= 0x02;
	GPIO_PORTF_DATA_R ^= 0x04;
	if((UART1_FR_R &0x10)==0){
		copyHardwareToSoftware();
	}
	RxCounter++;
	GPIO_PORTF_DATA_R ^=0x04;
	EnableInterrupts();
	return;
}
