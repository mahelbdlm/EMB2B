/*
 * analog.c
 *
 *  Created on: Apr 20, 2023
 *      Author: mahel.berthod
 */
#include "Base.h"     // Basic definitions
#include "analog.h" // ANALOG definitions

#define uVAvg_Slope 2500 //uV
#define uV25 760000 //uV
#define uVddVal 300000 //mV

void initAdc(void){
	RCC->APB2ENR |= RCC_APB2ENR_ADC1EN; // Enables ADC clock
	ADC->CCR = (ADC->CCR & ~ADC_CCR_ADCPRE_1) | ADC_CCR_ADCPRE_0;
		//Set ADC local clock to 1/4 = 21 MHz
	ADC1->SMPR2 = (ADC1->SMPR2 & ~ADC_SMPR2_SMP8) | ADC_SMPR2_SMP8_0; //Sample time for SMP8 is set to 001 -> 15 cycles

    (POT_PORT->PUPDR) &= (~POT_PAD); // Disable pull-up and pull-down of PB0
	(POT_PORT->MODER) = ((POT_PORT->MODER) | GPIO_MODER_MODER0 | GPIO_MODER_MODER0_1); //Set 11 (INPUT ANALOG) of POT_PORT PB0

	//Set up TEMPERATURE
	ADC1->SMPR1 = (ADC1->SMPR1 | ADC_SMPR1_SMP16); //Sample time for SMP16 is set to 111 -> 480 cycles
	ADC->CCR |= ADC_CCR_TSVREFE; //Enable temp sensor OR reference voltage for readVdd

	//Set up readVdd
	ADC1->SMPR1 = (ADC1->SMPR1 | ADC_SMPR1_SMP17); //Sample time for SMP16 is set to 111 -> 480 cycles

	ADC1->CR2 |= ADC_CR2_ADON; //Enable ADC
}

int16_t readIN8(void){
	ADC1->SQR1 = ADC1->SQR1 & ~ADC_SQR1_L; //Only 1 step -> length = 1 -> 0000
	ADC1->SQR3 = (ADC1->SQR3 & ~ADC_SQR3_SQ1) | 8;
		// Set channel 8 to first pos in sequence
	ADC1->CR2 |= ADC_CR2_SWSTART; //Start conversion

	while((ADC1->SR & ADC_SR_EOC) == 0) ; //Wait for conversion to finish

	return(ADC1->DR);//TO MODIFY LATER ON
}


int32_t readChannel(int32_t channelNum){
	/*
	 * Reads channel with ADC
	 * INPUTS: channelNum (int) : Channel number to convert
	 */
	ADC1->SQR1 = ADC1->SQR1 & ~ADC_SQR1_L; //Only 1 step -> length = 1 -> 0000

	ADC1->SQR3 = (ADC1->SQR3 & ~ADC_SQR3_SQ1) | channelNum;

	ADC1->CR2 |= ADC_CR2_SWSTART; //Start conversion

	while((ADC1->SR & ADC_SR_EOC) == 0) ; //Wait for conversion to finish

	return(ADC1->DR);//TO MODIFY LATER ON
}

int32_t readT(void){
	ADC1->SQR1 = ADC1->SQR1 & ~ADC_SQR1_L; //Only 1 step -> length = 1 -> 0000
	ADC1->SQR3 = (ADC1->SQR3 & ~ADC_SQR3_SQ1) | 16;
	ADC1->CR2 |= ADC_CR2_SWSTART; //Start conversion

	while((ADC1->SR & ADC_SR_EOC) == 0) ; //Wait for conversion to finish

	int32_t uVtemp = (uVddVal*ADC1->DR)/(1<<12);
	uVtemp = 25000+(((uVtemp*10-uV25)*1000)/uVAvg_Slope);
	return(uVtemp/100);//Temperature
}


int32_t readVdd(void){
	ADC1->SQR1 = ADC1->SQR1 & ~ADC_SQR1_L; //Only 1 step -> length = 1 -> 0000
	ADC1->SQR3 = (ADC1->SQR3 & ~ADC_SQR3_SQ1) | 17;
	ADC1->CR2 |= ADC_CR2_SWSTART; //Start conversion

	while((ADC1->SR & ADC_SR_EOC) == 0) ; //Wait for conversion to finish

	return(ADC1->DR);//TO MODIFY LATER ON
}

int32_t readTAmpl(int32_t vddVal){
	ADC1->SQR1 = ADC1->SQR1 & ~ADC_SQR1_L; //Only 1 step -> length = 1 -> 0000
	ADC1->SQR3 = (ADC1->SQR3 & ~ADC_SQR3_SQ1) | 16;
	ADC1->CR2 |= ADC_CR2_SWSTART; //Start conversion

	while((ADC1->SR & ADC_SR_EOC) == 0) ; //Wait for conversion to finish

	int32_t uVtemp = (vddVal*ADC1->DR)/(1<<12);
	uVtemp = 25000+(((uVtemp*10-uV25)*1000)/uVAvg_Slope);
	return(uVtemp/100);//Temperature
}
