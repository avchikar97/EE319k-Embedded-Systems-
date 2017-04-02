// ADC.c
// Runs on LM4F120/TM4C123
// Provide functions that initialize ADC0
// Created: 4/9/16
// Student names: Akaash Chikarmane/Milan Feliciello
// Last modification date: 4/9/16

#include <stdint.h>
#include "tm4c123gh6pm.h"
uint32_t delay;
// ADC initialization function 
// Input: none
// Output: none
void ADC_Init(void){ 
	SYSCTL_RCGC2_R |= 0x10;				//1) activate PORTE clock
	while(SYSCTL_RCGC2_R == 0){		//null
	}
	GPIO_PORTE_DIR_R &= ~0x04;		//2) make PE2 input
	GPIO_PORTE_AFSEL_R |= 0x04;		//3) enable alternate function for PE2
	GPIO_PORTE_DEN_R &= ~0x04;		//4) disable digital I/O for PE2
	GPIO_PORTE_AMSEL_R |= 0x04;		//5) enable analong function on PE2
	SYSCTL_RCGCADC_R |= 0x01;			//6) activate ADC0
	delay = SYSCTL_RCGCADC_R;			//null
	delay = SYSCTL_RCGCADC_R;			//null
	delay = SYSCTL_RCGCADC_R;			//null
	delay = SYSCTL_RCGCADC_R;			//null
	ADC0_PC_R = 0x01;							//7) configure for 125k
	ADC0_SSPRI_R &= ~0x0123;			//8) Seq3 is highest priority
	ADC0_ACTSS_R &= ~0x0008;			//9) disable sample sequencer 3
	ADC0_EMUX_R &= ~0xF000;				//10) seq3 is software trigger
	ADC0_SSMUX3_R = (ADC0_SSMUX3_R & 0xFFFFFFF0) + 1;		//11) Ain1 (PE2)
	ADC0_SSCTL3_R = 0x0006;				//12) no TS0D0, yes IE0 END0
	ADC0_IM_R &= ~0x0008;					//13) disable SS3 interrupts
	ADC0_ACTSS_R |= 0x0008;				//14) enable sample sequencer 3
	ADC0_SAC_R = 0x04; // A N means 2^N (16 here) samples are averaged; 0<=N<=6
}

//------------ADC_In------------
// Busy-wait Analog to digital conversion
// Input: none
// Output: 12-bit result of ADC conversion
uint32_t ADC_In(void){  
	uint32_t result;
	ADC0_PSSI_R = 0x0008;							//1) initiate SS3
	while((ADC0_RIS_R & 0x08) == 0){	//2) wait for conversion to finish
	}
	result = ADC0_SSFIFO3_R & 0xFFF;	//3) read 12-bit result
	ADC0_ISC_R = 0x0008;							//4) acknowledge completion
	return(result);
}


