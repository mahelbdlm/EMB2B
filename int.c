/**************************************************************

 i n t . c

 Interrupt test source file

***************************************************************/

#include "Base.h"     // Basic definitions
#include "int.h"      // This module header file
#include "lcd.h"
#include "accel.h"
volatile int switchFlag;

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

void interruptTest(void)
 {
  RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN; //Activate clock of SYSCFG
  SYSCFG->EXTICR[0] = ( SYSCFG->EXTICR[0] & SYSCFG_EXTICR1_EXTI0) | (SYSCFG_EXTICR1_EXTI0_PA) ; //Connects set first 4 mux to 0 -> PA0  to PA3
  EXTI->IMR |= EXTI_IMR_MR0;     //Interrupt request from line 0 will not be masked
  EXTI->RTSR |= EXTI_RTSR_TR0;   //Enable high flanc
  EXTI->FTSR &= ~EXTI_FTSR_TR0;  //Disable falling edge just in case
    //Select port A on mux EXTI0 to interrupt PA0 usr button

  EXTI->PR = EXTI_PR_PR0;
  nvicEnableVector(EXTI0_IRQn, CORTEX_PRIORITY_MASK(STM32_EXT_EXTI0_IRQ_PRIORITY));
  while(1){
	  switchFlag = 0; //Deletes SwitchFlag
	  while(!switchFlag);  //Waits for switchFlag to take val 1
	  int32_t accelValY = readAccel(0x2B, true);       //Read Y accel
	   char resultAccel[8];
	   itoa(accelValY, resultAccel,  10); //Converts Y num to base 10 num
	   LCD_ClearDisplay(); //Clear LCD display
	   LCD_GotoXY(0, 0);
	   LCD_SendString("Acceleration Y: ");
	   LCD_GotoXY(0, 1);
	   LCD_SendString(resultAccel);

  }
 }


/********************* ISR FUNCTIONS ******************************
Those functions should never be called directly or indirectly from
the main program. They are automatically called by the NVIC
interrupt subsystem.
*******************************************************************/

// EXTI 0 ISR
// Associated to the user button
// Tasks to do:
//    * Erase the pending interrupt flag
//    * Change the green led status
//    * Activate the flag so that the accelerometer Y axis is read
CH_IRQ_HANDLER(EXTI0_IRQHandler)
 {
	CH_IRQ_PROLOGUE();
	// Start of the ISR code
	EXTI->PR = EXTI_PR_PR0; //Write 1 to PR0 to set interruption as handled
	LEDS_PORT->ODR = LEDS_PORT->ODR ^ GREEN_LED_BIT; //Alternate GREEN_LED
	switchFlag = 1; //Set switchFlag to 1 to continue on interruptTest
	// End of the ISR code
	//DELAY_US(100);
	CH_IRQ_EPILOGUE();
 }

