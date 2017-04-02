// ***** 0. Documentation Section *****
// TableTrafficLight.c for (Lab 10 edX), Lab 5 EE319K
// Runs on LM4F120/TM4C123
// Program written by: Akaash Chikarmane/Milan Feliciello
// Date Created: 1/24/2015 
// Last Modified: 3/7/2016 
// Section 2-3pm     TA: Jun Qi Lau/Arman Behroozi
// Lab number: 5
// Hardware connections
// TO STUDENTS "REMOVE THIS LINE AND SPECIFY YOUR HARDWARE********
// east/west red light connected to PE2
// east/west yellow light connected to PE1
// east/west green light connected to PE0
// north/south facing red light connected to PE5
// north/south facing yellow light connected to PE4
// north/south facing green light connected to PE3
// pedestrian detector connected to PB0 (1=pedestrian present)
// north/south car detector connected to PB2 (1=car present)
// east/west car detector connected to PB1 (1=car present)
// "walk" light connected to PF3 (built-in green LED)
// "don't walk" light connected to PF1 (built-in red LED)

// ***** 1. Pre-processor Directives Section *****
#include <stdint.h>
#include "TExaS.h"
#include "tm4c123gh6pm.h"
#include "Systick.h"
#define	GoS			0
#define WaitSP	1
#define GoW			2
#define	WaitWP	3
#define GoP			4
#define	WaitSW	5
#define	WaitWS	6
#define	WaitPS0	7
#define WaitPW0	8
#define	WaitPN0	9
#define	WaitPP0	10
#define	WaitPS1	11
#define WaitPW1	12
#define	WaitPN1	13
#define	WaitPP1	14
#define	WaitPS2	15
#define WaitPW2	16
#define	WaitPN2	17
#define	WaitPP2	18
#define	WaitPS3	19
#define WaitPW3	20
#define	WaitPN3	21
#define	WaitPP3	22
#define	StopP		23

struct FSMNode{
	uint8_t outE;
	uint8_t outF;
	uint32_t delay;
	uint8_t next[8];
};

typedef struct FSMNode FSMType;
const FSMType FSM[24]={
	{0x0C, 0x02, 200, {GoS, WaitSP, WaitSW, WaitSP, GoS, WaitSP, WaitSW, WaitSW}},
	{0x14, 0x02, 100, {GoP, GoP, GoP, GoP, GoP, GoP, GoP, GoP}},
	{0x21, 0x02, 200, {GoW, WaitWP, GoW, WaitWP, WaitWS, WaitWP, WaitWS, WaitWP}},
	{0x22, 0x02, 100, {GoP, GoP, GoP, GoP, GoP, GoP, GoP, GoP}},
	{0x24, 0x08, 200, {WaitPN0, WaitPP0, WaitPW0, WaitPW0, WaitPS0, WaitPS0, WaitPW0, WaitPW0}},
	{0x14, 0x02, 100, {GoW, GoW, GoW, GoW, GoW, GoW, GoW, GoW}},
	{0x22, 0x02, 100, {GoS,	GoS, GoS, GoS, GoS,	GoS, GoS,	GoS}},
	
	{0x24, 0x00, 35, {WaitPS1, WaitPS1, WaitPS1, WaitPS1, WaitPS1, WaitPS1, WaitPS1, WaitPS1}},
	{0x24, 0x00, 35, {WaitPW1, WaitPW1, WaitPW1, WaitPW1, WaitPW1, WaitPW1, WaitPW1, WaitPW1}},
	{0x24, 0x00, 35, {WaitPN1, WaitPN1, WaitPN1, WaitPN1, WaitPN1, WaitPN1, WaitPN1, WaitPN1}},
	{0x24, 0x00, 35, {WaitPP1, WaitPP1, WaitPP1, WaitPP1, WaitPP1, WaitPP1, WaitPP1, WaitPP1}},
	
	{0x24, 0x02, 35, {WaitPS2, WaitPS2, WaitPS2, WaitPS2, WaitPS2, WaitPS2, WaitPS2, WaitPS2}},
	{0x24, 0x02, 35, {WaitPW2, WaitPW2, WaitPW2, WaitPW2, WaitPW2, WaitPW2, WaitPW2, WaitPW2}},
	{0x24, 0x02, 35, {WaitPN2, WaitPN2, WaitPN2, WaitPN2, WaitPN2, WaitPN2, WaitPN2, WaitPN2}},
	{0x24, 0x02, 35, {WaitPP2, WaitPP2, WaitPP2, WaitPP2, WaitPP2, WaitPP2, WaitPP2, WaitPP2}},
	
	{0x24, 0x00, 35, {WaitPS3, WaitPS3, WaitPS3, WaitPS3, WaitPS3, WaitPS3, WaitPS3, WaitPS3}},
	{0x24, 0x00, 35, {WaitPW3, WaitPW3, WaitPW3, WaitPW3, WaitPW3, WaitPW3, WaitPW3, WaitPW3}},
	{0x24, 0x00, 35, {WaitPN3, WaitPN3, WaitPN3, WaitPN3, WaitPN3, WaitPN3, WaitPN3, WaitPN3}},
	{0x24, 0x00, 35, {WaitPP3, WaitPP3, WaitPP3, WaitPP3, WaitPP3, WaitPP3, WaitPP3, WaitPP3}},
	
	{0x24, 0x02, 35, {GoS, GoS, GoS, GoS, GoS, GoS, GoS, GoS}},
	{0x24, 0x02, 35, {GoW, GoW, GoW, GoW, GoW, GoW, GoW, GoS}},
	{0x24, 0x02, 35, {StopP, StopP, StopP, StopP, StopP, StopP, StopP, StopP}},
	{0x24, 0x02, 35, {StopP, StopP, StopP, StopP, StopP, StopP, StopP, StopP}},
	
	{0x24, 0x02, 100, {StopP, GoP, GoW, GoP, GoS, GoP, GoW, GoP}}
};
// ***** 2. Global Declarations Section *****
uint8_t CState;
uint8_t Input;
// FUNCTION PROTOTYPES: Each subroutine defined
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
void Init(void);
int main(void){ 
  TExaS_Init(SW_PIN_PE210, LED_PIN_PB543210); // activate grader and set system clock to 80 MHz
	Init();
 
  
  EnableInterrupts();
	CState = GoS;
  while(1){
    GPIO_PORTE_DATA_R = FSM[CState].outE;//output whatever traffic signals
		GPIO_PORTF_DATA_R = FSM[CState].outF;//output whatever pedestrian signals
		SysTick_Wait10ms(FSM[CState].delay);//delay for however long
		Input = GPIO_PORTB_DATA_R;//take the input
		CState = FSM[CState].next[Input];//change the state based on the next input
  }
}

// ***** 3. Subroutines Section *****
void Init(void){
	SYSCTL_RCGC2_R |= 0x00000032;
	while((SYSCTL_RCGC2_R& 0x00000032) != 0x00000032) {};
	GPIO_PORTB_DEN_R |=0x07;
	GPIO_PORTE_DEN_R |=0x3F;
	GPIO_PORTF_DEN_R |=0x0A;
	GPIO_PORTB_AFSEL_R &=0xF8;
	GPIO_PORTE_AFSEL_R &=0xC0;
	GPIO_PORTF_AFSEL_R &=0xF5;
	GPIO_PORTB_DIR_R &=0xF8;

	

	GPIO_PORTE_DIR_R |=0x3F;
	
	GPIO_PORTF_DIR_R |=0x0A;
	SysTick_Init();
	}


