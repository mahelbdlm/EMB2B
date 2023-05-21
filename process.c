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
static WORKING_AREA(waChildGreen,128);
static WORKING_AREA(waChildRed,128);

// Function prototype for the child thread
// Needed because thChild is referenced before its definition
// It is not included in process.h because it's a static function
static msg_t thChild(void *arg);

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
 chThdCreateStatic(waChild, sizeof(waChild), NORMALPRIO+1, thChild, NULL);

 while (TRUE)
     {
	 // Turn on blue LED using BSRR
	 (LEDS_PORT->BSRR.H.set)=BLUE_LED_BIT;

	 // Pause
 	 SLEEP_MS(300);

 	 // Turn off blue LED using BSRR
 	 (LEDS_PORT->BSRR.H.clear)=BLUE_LED_BIT;

 	 // Pausa
 	SLEEP_MS(300);
     }
 }


// Child thread that bliks the orange LED
static msg_t thChild(void *arg)
 {
 while (TRUE)
    {
	 // Turn on orange LED using BSRR
	(LEDS_PORT->BSRR.H.set)=ORANGE_LED_BIT;

	// Pausa
	SLEEP_MS(500);

	// Turn off orange LED using BSRR
	(LEDS_PORT->BSRR.H.clear)=ORANGE_LED_BIT;

	// Pausa
	SLEEP_MS(500);
    }
 return 0;
 }

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

/**** FOUR THREADS OVER ENGINEERED DISCO MODE BEGINS ****/

//Definition of 'Power Ranger' childs:
static msg_t thChildOrange(void *arg);
static msg_t thChildGreen(void *arg);
static msg_t thChildRed(void *arg);

// Test for extensive four thread operation
// This function:
//     - Initializes the system
//     - Creates three child thread that blinks their respective leds
//     - Blinks the blue LED
void test4threads(void)
 {
 // Basic system initialization
 baseInit();

 // Child thread PW Orange
 chThdCreateStatic(waChild, sizeof(waChild), NORMALPRIO, thChildOrange, NULL);
 chThdCreateStatic(waChild, sizeof(waChild), NORMALPRIO, thChildGreen, NULL);
 chThdCreateStatic(waChild, sizeof(waChild), NORMALPRIO, thChildRed, NULL);

 while (TRUE)
     {
	 // Turn on blue LED using BSRR
	 (LEDS_PORT->BSRR.H.set)=BLUE_LED_BIT;

	 // Pause
 	SLEEP_MS(300);

 	 // Turn off blue LED using BSRR
 	 (LEDS_PORT->BSRR.H.clear)=BLUE_LED_BIT;

 	 // Pausa
     SLEEP_MS(300);
     }
 }


// Child thread that bliks the orange LED
static msg_t thChildOrange(void *arg)
 {
 while (TRUE)
    {
	 // Turn on orange LED using BSRR
	(LEDS_PORT->BSRR.H.set)=ORANGE_LED_BIT;

	// Pausa
	SLEEP_MS(500);

	// Turn off orange LED using BSRR
	(LEDS_PORT->BSRR.H.clear)=ORANGE_LED_BIT;

	// Pausa
	SLEEP_MS(500);
    }
 return 0;
 }

// Child thread that bliks the green LED
static msg_t thChildGreen(void *arg)
 {
 while (TRUE)
    {
	 // Turn on orange LED using BSRR
	(LEDS_PORT->BSRR.H.set)=GREEN_LED_BIT;

	// Pausa
	SLEEP_MS(400);

	// Turn off orange LED using BSRR
	(LEDS_PORT->BSRR.H.clear)=GREEN_LED_BIT;

	// Pausa
	SLEEP_MS(400);
    }
 return 0;
 }

// Child thread that bliks the green LED
static msg_t thChildRed(void *arg)
 {
 while (TRUE)
    {
	 // Turn on orange LED using BSRR
	(LEDS_PORT->BSRR.H.set)=RED_LED_BIT;

	// Pausa
	SLEEP_MS(700);

	// Turn off orange LED using BSRR
	(LEDS_PORT->BSRR.H.clear)=RED_LED_BIT;

	// Pausa
	SLEEP_MS(700);
    }
 return 0;
 }

/******** Process same thing, but awesom(ed) :) ********/
// Test for extensive four thread operation
// This function:
//     - Initializes the system
//     - Creates only one child thread that blinks leds
//     - Blinks the blue LED
//	   - Makes us happy by getting us all points
static msg_t thChildMixt(void *arg);
void test4threadsAmpl(void)
 {
 // Basic system initialization
 baseInit();

 // Child thread PW Orange
 chThdCreateStatic(waChild, sizeof(waChild), NORMALPRIO+1, thChildMixt, (void *)1);
 chThdCreateStatic(waChildGreen, sizeof(waChildGreen), NORMALPRIO+1, thChildMixt, (void *)2);
 chThdCreateStatic(waChildRed, sizeof(waChildRed), NORMALPRIO+1, thChildMixt, (void *)3);
 thChildMixt((void *)0);
 }

static msg_t thChildMixt(void *arg)
 {
	int32_t sleepTime[4] = {300, 500, 400, 700};
	int32_t ledBit[4] = {BLUE_LED_BIT, ORANGE_LED_BIT, GREEN_LED_BIT, RED_LED_BIT};
 while (TRUE)
    {
	 // Turn on orange LED using BSRR
	(LEDS_PORT->BSRR.H.set)=(ledBit[(int)arg]);
	// Pausa
	SLEEP_MS(sleepTime[(int)arg]);

	// Turn off orange LED using BSRR
	(LEDS_PORT->BSRR.H.clear)=(ledBit[(int)arg]);

	// Pausa
	SLEEP_MS(sleepTime[(int)arg]);
    }
 return 0;
 }

/***** FOUR THREADS OVER ENGINEERED DISCO MODE ENDS *****/


