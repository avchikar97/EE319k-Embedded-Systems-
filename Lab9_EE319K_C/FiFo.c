// FiFo.c
// Runs on LM4F120/TM4C123
// Provide functions that implement the Software FiFo Buffer
// Last Modified: 4/12/2016 
// Student names: Akaash Chikarmane/Milan Feliciello
// Last modification date: 4/16/16

#include <stdint.h>
// Declare state variables for FiFo
//        size, buffer, put and get indexes
#define success 1
#define fail 0
#define size 9
char fifo[size];
uint8_t PutI;
uint8_t GetI;
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts

// *********** FiFo_Init**********
// Initializes a software FIFO of a
// fixed size and sets up indexes for
// put and get operations
void FiFo_Init(){
	PutI=GetI=0;
}

// *********** FiFo_Put**********
// Adds an element to the FIFO
// Input: Character to be inserted
// Output: 1 for success and 0 for failure
//         failure is when the buffer is full
uint32_t FiFo_Put(char data) {
	DisableInterrupts();
	if((PutI+1)%size==GetI){
		EnableInterrupts();
		return(fail);
	}
	fifo[PutI]=data;
	PutI=(PutI+1)%size;
	return(success);
	EnableInterrupts();
}

// *********** FiFo_Get**********
// Gets an element from the FIFO
// Input: Pointer to a character that will get the character read from the buffer
// Output: 1 for success and 0 for failure
//         failure is when the buffer is empty
uint32_t FiFo_Get(char *datapt){
	DisableInterrupts();
	if(PutI==GetI){
		EnableInterrupts();
		return(fail);
	}
	*datapt=fifo[GetI];
	GetI=(GetI+1)%size;
	return(success);
	EnableInterrupts();
}
