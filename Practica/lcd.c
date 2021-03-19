/****************************************************

 l c d . c

 P2 LCD Functions

*****************************************************/

#include "Base.h"  // Basic definitions
#include "lcd.h"   // LCD definitions

// Some of the function need to be completed
// as is requested on the manual

/***************** PRIVATE FUNCTIONS TO BE IMPLEMENTED *************
 In C, all functions are public by default.
 Those functions, as they are not needed outside of lcd.c
 won't have prototypes on lcd.h.
 They will also be declared static so that the compiler can
 optimize their usage knowing they are not used outside this file.
****************************************************************/
#define SHIFT_BITS	12

// Initializes DB7..DB4, RS, E i BL in Push-Pull output mode
// and sets the outputs to "0"

static void lcdGPIOInit(void)
{
	//loop referencing all port (GPIOE) lines that must be modified
	int32_t i = 0;
	int32_t v[] = {LCD_BL_PAD,LCD_E_PAD,LCD_RS_PAD,LCD_DB4_PAD,LCD_DB5_PAD,LCD_DB6_PAD,LCD_DB7_PAD};
		while (v[i]){
			GPIO_ModePushPull(LCD_PORT, v[i]);
			i++;
		}
		//for (i=0; i < 6; i++) { //Ports de la LCD del 9 al 15
		//	GPIO_ModePushPull(LCD_PORT, v(i)); //Crida a la funció GPIO_ModePushPull per cada port
		//}
}

// Send 4 bits to the LCD and generates an enable pulse
//     port : port name
//     line : port line
void GPIO_ModePushPull(GPIO_TypeDef *port, int32_t linia) {
	//outpt register mode bits set to 01
	//even bits set to 1
	port->MODER = (port->MODER) | (BIT(2*linia));
	//odd bits set to 0
	port->MODER = (port->MODER) & ~(BIT(2*linia+1)); 
	//set register line bit to 0 for push-pull output
	port->OTYPER = (port->OTYPER) & ~BIT(linia);
	//bits set to 0 for minimum speed (2MHz) to avoid dangerous behaviours
	port->OSPEEDR = (port->OSPEEDR) & ~(BIT(linia*2)|BIT(linia*2+1));
	//set register bits to 01
	port->PUPDR = (port->PUPDR) & ~(BIT(linia*2)| BIT(linia*2+1));
}

// Send 4 bits to the LCD and generates an enable pulse
//     nibbleCmd : Bits 3..0 Nibble to send to DB7...DB4
//     RS        : TRUE (RS="1")   FALSE (RS="0")

static void lcdNibble(int32_t nibbleCmd,int32_t RS){
	//initialize to 0 the data bits
	(LCD_PORT->BSRR.H.clear) = LCD_DB4_BIT | LCD_DB5_BIT | LCD_DB6_BIT | LCD_DB7_BIT;
	//shift the bits to the positions corresponding to data (DB4, DB5, DB6 i DB7)
	(LCD_PORT->BSRR.H.set) = (nibbleCmd << SHIFT_BITS); 
	//set RS register bit to the function argument
	if (RS==1) (LCD_PORT->BSRR.H.set) = LCD_RS_BIT; 
	else (LCD_PORT->BSRR.H.clear) = LCD_RS_BIT; 
	DELAY_US(10);
	//set to 1 register enable bit
	(LCD_PORT->BSRR.H.set) = LCD_E_BIT; 
	DELAY_US(10);
	//simulate a 10 us pulse
	(LCD_PORT->BSRR.H.clear) = LCD_E_BIT;
	DELAY_US(10);
}


/********** PUBLIC FUNCTIONS TO IMPLEMENT ************************
 Those functions will be used outside of lcd.c so there will
 be prototypes for them in lcd.h.
******************************************************************/

// Backlight configuration
//     on evaluates to TRUE   Turn on backlight
//     on evaluates to FALSE  Turn off backlight
void LCD_Backlight(int32_t on){
	//set bit to 0 to switch off the backlight
	if (on == 0) (LCD_PORT->BSRR.H.clear)=BIT(LCD_BL_PAD);
	//set bit to 1 to switch on the backlight
	else (LCD_PORT->BSRR.H.set)=BIT(LCD_BL_PAD);
}

// Clear the LCD and set the cursor position at 0,0
void LCD_ClearDisplay(void){
	//0 in binary => 0000
	lcdNibble(0,0);
	//1 in binary => 0001
	lcdNibble(1,0); //1 en binari => 0001
	//wait at least 1,5 ms
	SLEEP_MS(2); 
}

// Configures the display
//     If Disp is TRUE turn on the display, if not, turns off
//     If Cursor is TRUE show the cursor, if not, hides it
//     If Blink is TRUE turn on blinking, if not, deactivate blinking
void LCD_Config(int32_t Disp,int32_t Cursor,int32_t Blink){
	//0 in binary => 0000
	lcdNibble(0,0);
	//make sure the arguments equals 0 (False) or 1 (True)
	if(Disp) Disp = 1;
	else if(Cursor) Cursor = 1;
	else if(Blink) Blink = 1;
	//send next 4 bits to 1 ans the arguments values
	lcdNibble((1<<3)|(Disp<<2)|(Cursor<<1)|(Blink<<0),0);
	DELAY_US(40);
}


// Set the cursor at the given position
//    col: Columnn (0..LCD_COLUMNS-1)
//    row: Row     (0..LCD_ROWS-1)
void LCD_GotoXY(int32_t col,int32_t row){
	//sets the cursor in the first column of the first row, 8 en binari => 1000 
	if (row == 0) lcdNibble(8,0);
	//sets the cursor in the first column of the second row, 12 en binari => 1100 
	else lcdNibble(12,0);
	//indicates the column position
	lcdNibble(col,0);
	DELAY_US(40);

}

// Send a character to the LCD at the current position
//     car: Charater to send
void LCD_SendChar(char car){
	//send 4 first bits on the left
	lcdNibble(car>>4,1);
	//send 4 bits on the right
	lcdNibble(car,1); 
	DELAY_US(40);
}

// Send a string to the LCD at the current position
//     string: String to send
void LCD_SendString(char *string){
	uint32_t i=0;
	//send all string characters until there is the escape sequence (end of string)
	while(string[i] != '\0'){
		LCD_SendChar(string[i]);
		i++;
	}
}

/********** PUBLIC FUNCTIONS ALREADY IMPLEMENTED ***************
 Those functions are provided to reduce the codding effort
 for the LCD module
****************************************************************/

// Init the LCD in 4 bits mode
// This function depends on the implementation of the
// private functions lcdSendNibble and lcdGPIOInit
void LCD_Init(void)
 {
 // Initializes the LCD GPIOs
 lcdGPIOInit();

 // Wait 50ms for vdd to stabilize
 SLEEP_MS(50);

 // Init command 1
 lcdNibble(0x03,0);
 SLEEP_MS(10); // Wait > 4.1ms

 // Repeat command
 lcdNibble(0x03,0);

 // Wait > 100us
 DELAY_US(500);

 // Repeat command
 lcdNibble(0x03,0);

 // Init command 2
 lcdNibble(0x02,0);

 // Mode 4 bits 2 lines 5x8 chars
 lcdNibble(0x08,0);
 lcdNibble(0x02,0);
 DELAY_US(50);

 // Display on blink on and cursor on
 lcdNibble(0x00,0);
 lcdNibble(0x0E,0);
 DELAY_US(50);

 // Clear Display
 lcdNibble(0x00,0);
 lcdNibble(0x01,0);
 SLEEP_MS(5);

 // Cursor set direction
 lcdNibble(0x00,0);
 lcdNibble(0x06,0);
 DELAY_US(50);

 // Set ram address
 lcdNibble(0x08,0);
 lcdNibble(0x00,0);
 DELAY_US(50);

 // Send OK
 lcdNibble(0x04,1);
 lcdNibble(0x0F,1);
 DELAY_US(50);
 lcdNibble(0x04,1);
 lcdNibble(0x0B,1);
 DELAY_US(50);
 }






