/*************************************************************

   m a i n . c

   Practica 1

 *************************************************************/

#include "Base.h"     // Basic definitions
#include "lcd.h"
#include "accel.h"
#include "int.h"



int main(void)
 {
 // Basic initializations
 //baseInit();

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


	baseInit(); // Basic initialization
	LCD_Init(); // Initializes the LCD
	LCD_Config(1,0,0);
	initAccel();
	int32_t inx = readAccel(0x29,1);
	int32_t iny = readAccel(0x2b,1);

	while(1){
		//Actualised values
		int32_t valx = readAccel(0x29,1);
		int32_t valy = readAccel(0x2b,1);
		//Column for the character = 256 positions lecture value / 16 columns for each row
		valx = round(((valx-inx)+33)/4.7);
		valy = round(((valy-iny)+33)/4.7);
		//Write the character in the column previously obtained
		if (valx>15) valx=15;
		if (valx<0) valx=0;
		if (valy>15) valy=15;
		if (valy<0) valy=0;
		LCD_ClearDisplay();
		LCD_GotoXY(valx,0);
		LCD_SendChar('*');
		LCD_GotoXY(valy,1);
		LCD_SendChar('*');
		SLEEP_MS(200);
	}


	baseInit(); // Basic initialization
	LCD_Init(); // Initializes the LCD
	LCD_Config(1,0,0);
	initAccel();
	int32_t inx = readAccel(0x29,1);
	int32_t iny = readAccel(0x2b,1);
	int32_t N = 5;
	int32_t m[2][N];
	int32_t j,mx,my;

	while(1){
		for (j=0;j<N-1;j++){
			//Actualised values
			m[0][j]= readAccel(0x29,1);
			m[1][j]= readAccel(0x2b,1);
			SLEEP_MS(200);
		}
		for (j=0;j<N-1;j++){
			mx = mx + m[0][j];
			my = my + m[1][j];
		}
		mx=mx/N;
		my=my/N;

		//Column for the character = 256 positions lecture value / 16 columns for each row
		int32_t valx = round(((mx-inx)+33)/4.7);
		int32_t valy = round(((my-iny)+33)/4.7);
		//Write the character in the column previously obtained
		if (valx>15) valx=15;
		if (valx<0) valx=0;
		if (valy>15) valy=15;
		if (valy<0) valy=0;
		LCD_ClearDisplay();
		LCD_GotoXY(valx,0);
		LCD_SendChar('*');
		LCD_GotoXY(valy,1);
		LCD_SendChar('*');
	}
*/
     baseInit(); // Basic initialization
	LCD_Init(); // Initializes the LCD
	initAccel();
	interruptTest();

 // Return so that the compiler doesn't complain
 // It is not really needed as ledBlink never returns
 return 0;
 }

