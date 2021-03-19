/*************************************************************

   m a i n . c

   Practica 1

 *************************************************************/

#include "Base.h"     // Basic definitions
#include "lcd.h"



int main(void)
 {
 // Basic initializations
 //baseInit();

	baseInit(); // Basic initialization
	LCD_Init(); // Initializes the LCD

	LCD_Config(1,1,1);

	LCD_SendString("ADRIA SANCHEZ");
	LCD_GotoXY(3,1);
	LCD_SendString("SARA MARTINEZ");

	while (1);



 // Return so that the compiler doesn't complain
 // It is not really needed as ledBlink never returns
 return 0;
 }

