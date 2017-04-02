// Program written by: ***Your Names**update this***
// Date Created: 1/22/2016 
// Last Modified: 1/22/2016 
// Section ***Tuesday 1-2***update this***
// Instructor: ***Ramesh Yerraballi**update this***
// Lab number: 3
// Brief description of the program
//   If the switch is presses, the LED toggles at 8 Hz
// Hardware connections
//  PE1 is switch input  (1 means pressed, 0 means not pressed)
//  PE0 is LED output (1 activates external LED on protoboard) 
//Overall functionality of this system is the similar to Lab 2, with six changes:
//1-  the pin to which we connect the switch is moved to PE1, 
//2-  you will have to remove the PUR initialization because pull up is no longer needed. 
//3-  the pin to which we connect the LED is moved to PE0, 
//4-  the switch is changed from negative to positive logic, and 
//5-  you should increase the delay so it flashes about 8 Hz.
//6-  the LED should be on when the switch is not pressed
// Operation
//   1) Make PE0 an output and make PE1 an input. 
//   2) The system starts with the LED on (make PE0 =1). 
//   3) Wait about 62 ms
//   4) If the switch is pressed (PE1 is 1), then toggle the LED once, else turn the LED on. 
//   5) Steps 3 and 4 are repeated over and over

#include <stdint.h>
#include "inc/tm4c123gh6pm.h"
void TExaS_Init(void);
int main(void){ 
  TExaS_Init(); // 80 MHz and PD3 is scope/meter
  while(1){

  }
}

