// Lab6.c
// Runs on LM4F120 or TM4C123
// Use SysTick interrupts to implement a 4-key digital piano
// MOOC lab 13 or EE319K lab6 starter
// Program written by: Akaash Chikarmane
// Date Created: 1/24/2015 
// Last Modified: 3/17/2015 
// Section 2-3pm     TA: Wooseok Lee
// Lab number: 6
// Hardware connections
// TO STUDENTS "REMOVE THIS LINE AND SPECIFY YOUR HARDWARE********


#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "Sound.h"
#include "Piano.h"
#include "TExaS.h"
#include "DAC.h"
#include "globals.h"
// basic functions defined at end of startup.s
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
void DAC_Out(uint32_t data);
	extern uint32_t period;
	uint8_t keysPressed;
	uint8_t i;
	uint32_t count;
int main(void){      
	TExaS_Init(SW_PIN_PE3210,DAC_PIN_PB3210,ScopeOn);    // bus clock at 80 MHz
  Piano_Init();
  Sound_Init(0);
	SYSCTL_RCGC2_R |= 0x20;
	while(SYSCTL_RCGC2_R == 0){
	}
	GPIO_PORTF_DIR_R |= 0x04;
	GPIO_PORTF_DEN_R |= 0x04;
	GPIO_PORTF_AFSEL_R &= ~0x04;
	count = 0;
  // other initialization
  EnableInterrupts();
	i = 0;
  while(1){
		if(count == 131000){
			GPIO_PORTF_DATA_R ^= 0x04;
			count = 0;
		}
		count ++;
		Piano_In();//determines which key is pressed for setting period
		if(key == 0x01){
			Sound_Play(4780);//C note
		}
		if(key == 0x02){
			Sound_Play(3794);//E note
		}
		if(key == 0x04){
			Sound_Play(4258);//D note
		}											//plays the sound set by if statements
  }
}
