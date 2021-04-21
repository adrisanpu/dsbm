/**************************************************************

 i n t . c

 Interrupt test source file

***************************************************************/

#include "Base.h"     // Basic definitions
#include "int.h"      // This module header file
#include "accel.h"
#include "lcd.h"

/********** PUBLIC FUNCTIONS TO IMPLEMENT ************************
 Those functions will be used outside of lcd.c so there will
 be prototypes for them in lcd.h.
******************************************************************/

// Configures the button (PA0) ISR
// PA0 is already configured as input in the halinit() call
// This function must:
//
//    * Activate the SYSCFG clock
//
//    * Assigna port A to EXTI0
//
//    * Enable the EXTI0 interrupt
//    * Acctivate EXTI0 on rising edge
//    * Clear the pending interrupt flag at EXTI0
//
//    * Enable EXTI0 at NVIC

volatile int switchFlag;

// initializes the registers for proper interruptions and executes the code for the experiment
//     
void interruptTest(void){
	//Enable the clock of the SYSCFG peripheral of the APB2 bus
	RCC->APB2ENR = (RCC->APB2ENR) | (RCC_APB2ENR_SYSCFGEN);
	//Set port A to EXTI0 field of the EXTICR1(EXTICR[0]) register (0000)
	SYSCFG->EXTICR[0] = (SYSCFG->EXTICR[0]) & (~SYSCFG_EXTICR1_EXTI0);
	//Enable the EXTI0 interruption (set “1” = remove the mask)
	EXTI->IMR = (EXTI->IMR) | EXTI_IMR_MR0;
	//Enable the EXTI0 interruption (for rising flank)
	EXTI->RTSR = (EXTI->RTSR) | EXTI_RTSR_TR0;
	//Delete the EXTI0 interruption request(Pending Register = “1”)
	EXTI->PR = EXTI_PR_PR0;
	//Enable the EXTI0 interruption with nvicEnableVector function (set the properly priority)
	nvicEnableVector(EXTI0_IRQn,CORTEX_PRIORITY_MASK(STM32_EXT_EXTI0_IRQ_PRIORITY));

	while(1){
		//Clear switchFlag
		switchFlag = 0;
		//Waiting for some function to activate switchFlag to exit the loop
		while(!switchFlag);
		char dada[7];
		LCD_ClearDisplay(); //Clear display
		int32_t iny = readAccel(0x2b,1); //Read “y” of the accelerometer
		LCD_GotoXY(0,0);
		LCD_SendString("Y = "); //Show the value
		LCD_GotoXY(3,0);
		LCD_SendString(itoa(iny,dada,10));
	}
}


/********************* ISR FUNCTIONS ******************************
Those functions should never be called directly or indirectly from
the main program. They are automatically called by the NVIC
interrupt subsystem.
*******************************************************************/

//code to execute when an interruption is given (user button clicked)
CH_IRQ_HANDLER(EXTI0_IRQHandler)
 {
	//Reserve system registers
	CH_IRQ_PROLOGUE();
	//Erase the pending interrupt flag
	EXTI->PR = EXTI_PR_PR0;
	//Change the green led status
	LEDS_PORT->ODR=GREEN_LED_BIT ^ (LEDS_PORT->ODR);
	//Activate the flag so that the accelerometer Y axis is read
	switchFlag = 1;
	//Recover system registers
	CH_IRQ_EPILOGUE();
 }

