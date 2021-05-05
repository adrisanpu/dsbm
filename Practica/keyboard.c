
#include "Base.h"     // Basic definitions
#include "keyboard.h"  // This module header file

volatile int32_t key;

// Initializes keyboard registers
void initKeyboard(){
	int32_t i;
	for(i=0;i<=3;i++){
		//PD0 to PD3 outputs
		//Output mode [01] even bits set to 1
		GPIOD->MODER = (GPIOD->MODER) | BIT(2*i);
		//Odd bits set to 0
		GPIOD->MODER = (GPIOD->MODER) &~ BIT(2*i+1);
		//PD6 to PD9 inputs
		//Input mode [00] even bits set to 1
		GPIOD->MODER = (GPIOD->MODER) &~ BIT(2*(i+6));
		//Odd bits set to 0
		GPIOD->MODER = (GPIOD->MODER) &~ BIT(2*(i+6)+1);
		//Open drain [1]
		GPIOD->OTYPER = (GPIOD->OTYPER) | BIT(i);
		//Pull-up [01]
		GPIOD->PUPDR = ((GPIOD->PUPDR) | BIT(2*(i+6))) &~ BIT(2*(i+6)+1);
	}
}

// Read the keyboard register
// In case of any key clicked it's position in keyboard's code
// In case of none key clicked returns 32
int32_t readKeyboard(void){
	int32_t i,j;
	//Set all rows to 1
	(KEY_PORT->BSRR.H.set) = KEY_ROW1_BIT | KEY_ROW2_BIT | KEY_ROW3_BIT | KEY_ROW4_BIT;
	//Loop for rows
	for(i=0;i<=3;i++){
		//Set each row to 0
		(KEY_PORT->BSRR.H.clear) = BIT(i);
		DELAY_US(50);
		//Loop for columns
		int32_t v[] = {KEY_COL1_BIT,KEY_COL2_BIT,KEY_COL3_BIT,KEY_COL4_BIT};
		j = 0;
		while(v[j]){
			//If any column is 0 Return the pertinent number key
			if((~(KEY_PORT->IDR) & v[j]) == v[j]) return (i*4 + j-6);
		}
		//Set the row to 1 again
		(KEY_PORT->BSRR.H.set) = BIT(i);
	}
	//else return a value out of key number's range
	return 32;
}

// Read the keyboard register using the interruptions
void intConfigKeyboard(){
	int32_t i;
	(KEY_PORT->BSRR.H.clear) = KEY_ROW1_BIT | KEY_ROW2_BIT | KEY_ROW3_BIT | KEY_ROW4_BIT;

	//Set PD6 and PD7 to EXTICR1 register
	//Set PD8 and PD9 to EXTICR2 register
	//Port D [0011]
	RCC->APB2ENR = (RCC->APB2ENR) | (RCC_APB2ENR_SYSCFGEN);

	SYSCFG->EXTICR[1] = ((SYSCFG->EXTICR[1]) & (~SYSCFG_EXTICR2_EXTI6)) & (~SYSCFG_EXTICR2_EXTI7));
	SYSCFG->EXTICR[2] = ((SYSCFG->EXTICR[2]) & (~SYSCFG_EXTICR3_EXTI8)) & (~SYSCFG_EXTICR3_EXTI9));

	SYSCFG->EXTICR[1] = SYSCFG->EXTICR[1] | (0b0011 << 8);
	SYSCFG->EXTICR[1] = SYSCFG->EXTICR[1] | (0b0011 << 12);
	SYSCFG->EXTICR[2] = SYSCFG->EXTICR[2] | 0b0011;
	SYSCFG->EXTICR[2] = SYSCFG->EXTICR[2] | (0b0011 << 4);

	//Enable the interruption of EXTI6, EXTI7, EXTI8, EXTI9
	//Assign 1 = Remove the mask
	EXTI->IMR = (EXTI_IMR_MR6 | EXTI_IMR_MR7 | EXTI_IMR_MR8 | EXTI_IMR_MR9);
	//Enable the downhill flank [1]
	EXTI->FTSR = (EXTI_FTSR_TR6 | EXTI_FTSR_TR7 | EXTI_FTSR_TR8 | EXTI_FTSR_TR9);
	//Disable the rising flank interruptions
	EXTI->RTSR = (EXTI->RTSR) &~(EXTI_RTSR_TR6 | EXTI_RTSR_TR7 | EXTI_RTSR_TR8 | EXTI_RTSR_TR9);
	//Erase the EXTI6 to EXTI9 interruption request (Pending Register = 1)
	EXTI->PR = (EXTI_PR_PR6 | EXTI_PR_PR7 | EXTI_PR_PR8 | EXTI_PR_PR9);

	//Enable the EXTI6 to EXTI9 interruption with nvicEnableVector function
	nvicEnableVector(EXTI9_5_IRQn,CORTEX_PRIORITY_MASK(STM32_EXT_EXTI5_9_IRQ_PRIORITY));
	
	//code used when reading with interruptions
	while(1){
		key = 32;
		//Wait for some function enable switchFlag
		while(key == 32);
		LCD_ClearDisplay(); //Clear display
		//Array with characters corresponding to each keyboard key
		char code[16] = {'1', '2', '3', 'A', '4', '5', '6', 'B', '7', '8', '9', 'C', '*', '0', '#', 'D'};
		//Write the character
		LCD_SendChar(code[key]);
	}
}

// code executed when an interruption is called in the previous function
CH_IRQ_HANDLER(EXTI9_5_IRQHandler){
	CH_IRQ_PROLOGUE();
	if ((EXTI-> PR & (EXTI_PR_PR6 | EXTI_PR_PR7 | EXTI_PR_PR8 | EXTI_PR_PR9)) != 0) {
		//Mask the interruptions (set to 0) before exploring the keyboard
		EXTI->IMR = (EXTI->IMR) &~ (EXTI_IMR_MR6 | EXTI_IMR_MR7 | EXTI_IMR_MR8| EXTI_IMR_MR9);
		//Explore the keyboard and save the key
		key = readKeyboard();
		//Erase the EXTI6 to EXTI9 interruption request
		EXTI->PR = (EXTI_PR_PR6 | EXTI_PR_PR7 | EXTI_PR_PR8 | EXTI_PR_PR9);
		(GPIOD->BSRR.H.clear) = KEY_ROW1_BIT | KEY_ROW2_BIT | KEY_ROW3_BIT | KEY_ROW4_BIT;
		//Unmask the interruptions (set a 0) before exploring the keyboard
		EXTI->IMR = (EXTI_IMR_MR6 | EXTI_IMR_MR7 | EXTI_IMR_MR8 | EXTI_IMR_MR9);
	}
	CH_IRQ_EPILOGUE();
}

// Read simultaniously activated keyboard registers
// In case of any key clicked return a 16 bit value with bit in the keys code position set to 1
// In case of none key clicked returns 32
int32_t readMultiKey(void) {
	//set all rows to 1
	(GPIOD->BSRR.H.set) = KEY_ROW1_BIT | KEY_ROW2_BIT | KEY_ROW3_BIT | KEY_ROW4_BIT;
	//auxiliar variable with all its bits set to 0
	uint32_t valor=0;
	int32_t i;
	//iterate rows
	for(i=0; i<=3; i++){
		//set all row bits to 0
		(GPIOD->BSRR.H.clear) = BIT(i);
		DELAY_US(50);
		//iterate columns
		int32_t j = 0;
		int32_t v[] = {KEY_COL1_BIT,KEY_COL2_BIT,KEY_COL3_BIT,KEY_COL4_BIT};
		while(v[j]){
			//if the IDR column bit is set to 0 then put its return bit value to 1
			if((~(KEY_PORT->IDR) & v[j]) == v[j]) valor = valor | BIT(i*4+j);
			j++;
		}
		//set all row bits to 1
		(GPIOD->BSRR.H.set) = BIT(i);
	}
	return valor;
}
