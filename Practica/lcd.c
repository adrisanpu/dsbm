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
// and sets the ouptuts to "0"


static void lcdGPIOInit(void)
{
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

void GPIO_ModePushPull(GPIO_TypeDef *port, int32_t linia) {
	port->MODER = (port->MODER) | (BIT(2*linia)); //assignem sortides parells a 1
	port->MODER = (port->MODER) & ~(BIT(2*linia+1)); //assignem sortides senars a 0,ja que volem que cada parell de bits sigui 01 que és la funcionalitat de sortida
	port->BSRR.H.clear=BIT(linia);
	port->OTYPER = (port->OTYPER) & ~BIT(linia); //assignem sortida otyper a 0(sortida Push-Pull)
	port->OSPEEDR = (port->OSPEEDR) & ~(BIT(linia*2)|BIT(linia*2+1)); //assignem els dos bits del registre OSPEEDR a 0 (2 MHz)
	port->PUPDR = (port->PUPDR) & ~(BIT(linia*2)|BIT(linia*2+1));
}





// Send 4 bits to the LCD and generates an enable pulse
//     nibbleCmd : Bits 3..0 Nibble to send to DB7...DB4
//     RS        : TRUE (RS="1")   FALSE (RS="0")

static void lcdNibble(int32_t nibbleCmd,int32_t RS){
	(LCD_PORT->BSRR.H.clear) = LCD_DB4_BIT | LCD_DB5_BIT | LCD_DB6_BIT | LCD_DB7_BIT;

	/*Abans d’assignar el valor que se’ns passa pel paràmetre nibbleCmd als bits DB4,
	DB5, DB6 i DB7, els posem primer a zero tots per a que no hi hagi errors*/

	(LCD_PORT->BSRR.H.set) = (nibbleCmd << SHIFT_BITS); /*movem 12 bits a l’esquerra de
tal manera que les posicions 0, 1, 2 i 3 de nibbleCmd encaixin amb les posicions 12, 13, 14 i
15 respectivament, que són les que pertanyen als bits DB4, DB5, DB6 i DB7*/

	if (RS==1){
		(LCD_PORT->BSRR.H.set) = LCD_RS_BIT; //si el paràmetre RS val 1,activem el bit
	} else{
		(LCD_PORT->BSRR.H.clear) = LCD_RS_BIT; //si el paràmetre RS val 0,posem el bit a zero
	}

	DELAY_US(10);
	(LCD_PORT->BSRR.H.set) = LCD_E_BIT; //Posem el bit d’enable a 1 i esperem 10 us
	DELAY_US(10);
	(LCD_PORT->BSRR.H.clear) = LCD_E_BIT; //Posem el bit d’enable a 0 per tal de simular la baixada del pols de 10 us
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
	if (on == 0) {
		(LCD_PORT->BSRR.H.clear)=BIT(LCD_BL_PAD); //Si és fals llavors la apaga
	} else {
		(LCD_PORT->BSRR.H.set)=BIT(LCD_BL_PAD); //Si la variable on és certa o 1'encén la llum (funciona com un LED)
	}
}

// Clear the LCD and set the cursor position at 0,0
void LCD_ClearDisplay(void)
{
	lcdNibble(0,0); //0 en binari => 0000
	lcdNibble(1,0); //1 en binari => 0001
	SLEEP_MS(2); //hem d’esperar més de 1,5 ms, per això fiquem 2 ms
}


// Configures the display
//     If Disp is TRUE turn on the display, if not, turns off
//     If Cursor is TRUE show the cursor, if not, hides it
//     If Blink is TRUE turn on blinking, if not, deactivate blinking
void LCD_Config(int32_t Disp,int32_t Cursor,int32_t Blink){
	lcdNibble(0,0); // Primer enviem 0000
	if(Disp) Disp = 1;
	else if(Cursor) Cursor = 1;
	else if(Blink) Blink = 1;
	lcdNibble((1<<3)|(Disp<<2)|(Cursor<<1)|(Blink<<0),0); //enviem pels 4 següents bits un 1 i els valors dels paràmetres Disp, Cursor i Blink
	DELAY_US(40); // esperem 40 microsegons
}


// Set the cursor at the given position
//    col: Columnn (0..LCD_COLUMNS-1)
//    row: Row     (0..LCD_ROWS-1)
void LCD_GotoXY(int32_t col,int32_t row)
{
	if (row == 0){ //si la variable row val 0 el fiquem a la primera fila, sino a la segona
		lcdNibble(8,0); //8 en binari => 1000 (primera fila)
	} else{
	lcdNibble(12,0); //12 en binari => 1100 (segona fila)
	}
	lcdNibble(col,0); //Indiquem la posició de la columna
	DELAY_US(40);


	/*
	if (row == 0){
		lcdNibble(8,0);//8 en binari => 1000 (primera fila)
		lcdNibble(0+col>>4,0);
	}
	else if (row == 1){ //si la variable row val 0 el fiquem a la primera fila, sino a la segona
		lcdNibble(12,0);//8 en binari => 1000 (primera fila)
		lcdNibble(0>>4,0);
	}
	else if (row == 2){ //si la variable row val 0 el fiquem a la primera fila, sino a la segona
		lcdNibble(9,0);//8 en binari => 1000 (primera fila)
		lcdNibble(4>>4,0);
	}
	else if (row == 3){
		lcdNibble(13,0);//8 en binari => 1000 (primera fila)
		lcdNibble(4>>4,0);
	}
	*/

	DELAY_US(40);
}

// Send a character to the LCD at the current position
//     car: Charater to send
void LCD_SendChar(char car)
{
	lcdNibble(car>>4,1); //Accedim a la placa amb els 4 primers bits de l’esquerra.
	//L’operador “>>” desplaça cap a la dreta els bits i al ficar “car>>4” estem desplaçant 4
	//posicions a la dreta els bits de la variable “car”, per tal d’enviar els 4 de l’esquerra.

	lcdNibble(car,1); //Ara enviem els 4 bits de la dreta.
	DELAY_US(40); //Esperem 40 us
}


// Send a string to the LCD at the current position
//     string: String to send
void LCD_SendString(char *string){
	uint32_t i=0;
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






