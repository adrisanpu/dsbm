/**************************************************************

 p r o c e s s . c

 Introduction to processes source file

***************************************************************/

#include "Base.h"      // Basic definitions
#include "lcd.h"       // LCD module header file
#include "process.h"   // This module header file

/************** TWO THREAD SIMPLE EXAMPLE *********************/

// Working area for the child thread
static WORKING_AREA(waChild,128);
//static WORKING_AREA(waChildOrange,128);
//static WORKING_AREA(waChildGreen,128);
//static WORKING_AREA(waChildRed,128);

// Function prototype for the child thread
// Needed because thChild is referenced before its definition
// It is not included in process.h because it's a static function
static msg_t thChild(void *arg);
//static msg_t thChildOrange(void *arg);
//static msg_t thChildGreen(void *arg);
//static msg_t thChildRed(void *arg);

// Test for simple two thread operation
// This function:
//     - Initializes the system
//     - Creates a child thread that blinks the orange LED
//     - Blinks the blue LED
void test2threads(void)
 {
 // Basic system initialization
 baseInit();

 // Child thread creation
 chThdCreateStatic(waChild, sizeof(waChild), NORMALPRIO, thChild, NULL);
 //chThdCreateStatic(waChildOrange, sizeof(waChildOrange), NORMALPRIO, thChildOrange, NULL);
 //chThdCreateStatic(waChildGreen, sizeof(waChildGreen), NORMALPRIO, thChildGreen, NULL);
 //chThdCreateStatic(waChildRed, sizeof(waChildRed), NORMALPRIO, thChildRed, NULL);

 while (TRUE)
     {
	 // Turn on blue LED using BSRR
	 (LEDS_PORT->BSRR.H.set)=BLUE_LED_BIT;

	 // Pause
  	 // busyWait(3);
	 SLEEP_MS(300);

 	 // Turn off blue LED using BSRR
 	 (LEDS_PORT->BSRR.H.clear)=BLUE_LED_BIT;

 	 // Pausa
     // busyWait(3);
 	 SLEEP_MS(300);
     }
 }

//AMPLIACIÓN
static msg_t thChild(void *arg)
 {
	int32_t temps_ms;
 while (TRUE)
    {
	 if (arg == ORANGE_LED_BIT){
		 temps_ms = 500;
	 } else if (arg == GREEN_LED_BIT){
		 temps_ms = 400;
	 } else temps_ms = 700;

	 // Turn on orange LED using BSRR
	(LEDS_PORT->BSRR.H.set)=arg;

	// Pausa
	// busyWait(5);
	SLEEP_MS(temps_ms);

	// Turn off orange LED using BSRR
	(LEDS_PORT->BSRR.H.clear)=arg;

	// Pausa
	// busyWait(5);
	SLEEP_MS(temps_ms);

    }
 return 0;
 }

/*
// Child thread that bliks the ORANGE LED
static msg_t thChildOrange(void *arg)
 {
 while (TRUE)
    {
	 // Turn on orange LED using BSRR
	(LEDS_PORT->BSRR.H.set)=ORANGE_LED_BIT;

	// Pausa
	// busyWait(5);
	SLEEP_MS(500);

	// Turn off orange LED using BSRR
	(LEDS_PORT->BSRR.H.clear)=ORANGE_LED_BIT;

	// Pausa
	// busyWait(5);
	SLEEP_MS(500);

    }
 return 0;
 }

// Child thread that bliks the GREEN LED
static msg_t thChildGreen(void *arg)
 {
 while (TRUE)
    {
	 // Turn on orange LED using BSRR
	(LEDS_PORT->BSRR.H.set)=GREEN_LED_BIT;

	// Pausa
	// busyWait(4);
	SLEEP_MS(400);

	// Turn off orange LED using BSRR
	(LEDS_PORT->BSRR.H.clear)=GREEN_LED_BIT;

	// Pausa
	// busyWait(4);
	SLEEP_MS(400);

    }
 return 0;
 }

// Child thread that bliks the RED LED
static msg_t thChildRed(void *arg)
 {
 while (TRUE)
    {
	 // Turn on orange LED using BSRR
	(LEDS_PORT->BSRR.H.set)=RED_LED_BIT;

	// Pausa
	// busyWait(7);
	SLEEP_MS(700);

	// Turn off orange LED using BSRR
	(LEDS_PORT->BSRR.H.clear)=RED_LED_BIT;

	// Pausa
	// busyWait(7);
	SLEEP_MS(700);

    }
 return 0;
 }
*/
// Busy waits in a thread doing some operations
// The greater n, the longer the wait
void busyWait(uint32_t n)
 {
 uint32_t i;
 volatile uint32_t x=0;
 for(i=0;i<n*1000000;i++)
	    x=(x+2)/3;
 }


/************ TWO THREAD SIMPLE EXAMPLE ENDS *******************/



