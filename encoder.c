/*
 * encoder.c
 *
 *  Created on: May 8, 2023
 *      Author: mahel.berthod
 */
#include "Base.h"     // Basic definitions
#include "keyboard.h" //For function setInputPullUp
#include "encoder.h"

volatile int32_t countEnc=0;
volatile int32_t AHigh=0;
void initEncoder(void){
	setInputPullUp(ENC_PORT, ENC_A_PAD);
	setInputPullUp(ENC_PORT, ENC_B_PAD);

	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN; //Activate clock of SYSCFG
	SYSCFG->EXTICR[0] = ((SYSCFG->EXTICR[0] & ~SYSCFG_EXTICR1_EXTI1 & ~SYSCFG_EXTICR1_EXTI3)| SYSCFG_EXTICR1_EXTI1_PA | SYSCFG_EXTICR1_EXTI3_PA);
		//Connects PA1 and PA3 to interruption register
	EXTI->RTSR = EXTI->RTSR | EXTI_RTSR_TR1 | EXTI_RTSR_TR3;  //Enable high edge
	EXTI->FTSR = EXTI->FTSR | EXTI_FTSR_TR1 | EXTI_FTSR_TR3;  //Enable falling edge just in case
	EXTI->PR = EXTI_PR_PR1 | EXTI_PR_PR3; //Delete any flag to ensure correct initialization
	EXTI->IMR = EXTI->IMR | EXTI_IMR_MR1; //Enable interruptions on PA1
	nvicEnableVector(EXTI1_IRQn, CORTEX_PRIORITY_MASK(STM32_EXT_EXTI1_IRQ_PRIORITY));
		//Creates nested vector for ENC_A
	nvicEnableVector(EXTI3_IRQn, CORTEX_PRIORITY_MASK(STM32_EXT_EXTI3_IRQ_PRIORITY));
		//Creates nested vector for ENC_B

}

/*
CH_IRQ_HANDLER(EXTI1_IRQHandler)
 {
	CH_IRQ_PROLOGUE();
	// Start of the ISR code
	EXTI->IMR = EXTI->IMR & ~EXTI_IMR_MR1;
	EXTI->PR = EXTI_PR_PR1; //Write 1 to PR1 to set interruption as handled

	if (((ENC_PORT->IDR) & ENC_A_BIT)==0){
		//Bit PA1 is LOW
		if(((ENC_PORT->IDR) & ENC_B_BIT)==0){
			countEnc--;
		}
		else{
			countEnc++;
		}
	}

	EXTI->IMR = EXTI->IMR | EXTI_IMR_MR3;
	EXTI->PR = EXTI_PR_PR3;

	// End of the ISR code
	CH_IRQ_EPILOGUE();
 }

CH_IRQ_HANDLER(EXTI3_IRQHandler)
 {
	CH_IRQ_PROLOGUE();
	// Start of the ISR code
	EXTI->IMR = EXTI->IMR & ~EXTI_IMR_MR3;

	EXTI->PR = EXTI_PR_PR3; //Write 1 to PR3 to set interruption as handled


	EXTI->IMR = EXTI->IMR | EXTI_IMR_MR1;
	EXTI->PR = EXTI_PR_PR1;

	// End of the ISR code
	CH_IRQ_EPILOGUE();
 }
*/
//Ampliacio:

CH_IRQ_HANDLER(EXTI1_IRQHandler)
 {
	CH_IRQ_PROLOGUE();
	// Start of the ISR code
	EXTI->IMR = EXTI->IMR & ~EXTI_IMR_MR1;
	EXTI->PR = EXTI_PR_PR1; //Write 1 to PR0 to set interruption as handled

	if (((ENC_PORT->IDR) & ENC_A_BIT)==0){
		//Falling edge
		if(((ENC_PORT->IDR) & ENC_B_BIT)==0){
			//FA CLOSE
			countEnc--;
		}
		else{
			//FA OPEN
			countEnc++;
		}

	}
	else{
		//High edge
		if(((ENC_PORT->IDR) & ENC_B_BIT)==0){
			//FB CLOSE
			countEnc++;
		}
		else{
			//FB OPEN
			countEnc--;
		}
	}

	EXTI->IMR = EXTI->IMR | EXTI_IMR_MR3;
	EXTI->PR = EXTI_PR_PR3;

	// End of the ISR code
	CH_IRQ_EPILOGUE();
 }

CH_IRQ_HANDLER(EXTI3_IRQHandler)
 {
	CH_IRQ_PROLOGUE();
	// Start of the ISR code
	EXTI->IMR = EXTI->IMR & ~EXTI_IMR_MR3;
	EXTI->PR = EXTI_PR_PR3; //Write 1 to PR0 to set interruption as handled


	EXTI->IMR = EXTI->IMR | EXTI_IMR_MR1;
	EXTI->PR = EXTI_PR_PR1;

	// End of the ISR code
	CH_IRQ_EPILOGUE();
 }

