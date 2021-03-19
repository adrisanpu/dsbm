/*************************************************************

   m a i n . c

   Practica 1

 *************************************************************/

#include "Base.h"     // Basic definitions
#include "lcd.h"
#include "accel.h"



int main(void)
 {
	/*
 	baseInit(); // Basic initialization
 	LCD_Init(); // Initializes the LCD
 	LCD_Config(1,1,1);
 	LCD_SendString("ADRIA SANCHEZ");
 	LCD_GotoXY(3,1);
 	LCD_SendString("SARA MARTINEZ");

 	while (1);
	*/
	
	/*
	baseInit(); // Basic initialization
	LCD_Init(); // Initializes the LCD
	initAccel();
	int32_t val = readAccel(0x0F,0);
	char lectura[10];
	itoa(val,lectura,16);
	LCD_ClearDisplay();
	LCD_GotoXY(0,0);
	LCD_SendString(lectura);
	while (1);
	*/


	baseInit(); // Basic initialization
	LCD_Init(); // Initializes the LCD
	initAccel();
	char lectura[10];
	while(1){
		int32_t val = readAccel(0x2b,1);
		itoa(val,lectura,10);
		LCD_ClearDisplay();
		LCD_GotoXY(0,0);
		LCD_SendString(lectura);
		SLEEP_MS(200);
	}


 // Return so that the compiler doesn't complain
 // It is not really needed as ledBlink never returns
 return 0;
 }

