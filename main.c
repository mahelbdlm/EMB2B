/*************************************************************

   m a i n . c

   Practica 1

 *************************************************************/

#include "Base.h" // Basic definitions
#include "lcd.h"
#include "accel.h"
#include "int.h"
#include "keyboard.h"
#include "analog.h"
#include "encoder.h"
// Function that blinks the green LED
volatile int32_t i = 0;
extern volatile int32_t countEnc;
extern volatile int32_t AHigh;
void ledBlink(void)
{
	while (1)
	{
		i++;
		// Turn on the green LED using the BSRR register
		(LEDS_PORT->BSRR.H.set) = BIT(GREEN_LED_PAD);

		// Wait 200ms
		SLEEP_MS(200);

		// Turn off the green LED using the BSRR register
		(LEDS_PORT->BSRR.H.clear) = BIT(GREEN_LED_PAD);

		// Wait 200ms
		SLEEP_MS(200);
	}
}

void changeLeds(int LED_DISABLE, int LED_ENABLE, int WAIT_BETWEEN_LEDS)
{
	SLEEP_MS(WAIT_BETWEEN_LEDS);
	// Wait between leds

	(LEDS_PORT->BSRR.H.clear) = BIT(LED_DISABLE);
	// Turn off LED_DISABLE using the BSRR register

	SLEEP_MS(WAIT_BETWEEN_LEDS);
	// Wait between leds

	(LEDS_PORT->BSRR.H.set) = BIT(LED_ENABLE);
	// Turn on the green LED using the BSSR register
}

void ledSequence(void)
{
	int leds[4] = {GREEN_LED_PAD, ORANGE_LED_PAD, RED_LED_PAD, BLUE_LED_PAD};
	i = 0;
	(LEDS_PORT->BSRR.H.set) = BIT(leds[0]);
	// Turn on the green LED using the BSSR register
	while (1)
	{
		for (i = 0; i < 3; i++)
		{
			changeLeds(leds[i], leds[i + 1], 500);
		}
		changeLeds(leds[i], leds[0], 500);
	}

	// leds[4]= [XXX_LED_ZZZ, ..., YYY_LED, ZZZ];
}

int main(void)
{
	// Basic initializations
	baseInit();

	//test4threadsAmpl();
	//semaphoreExample();
	//mutexExample();

	// Call the LED blink function
	// This function never returns
	// ledBlink();

	// ledSequence();
	LCD_Init();
	LCD_Backlight(1);
	LCD_ClearDisplay();
	LCD_Config(1, 0, 0);
	//LCD_SendString("Probando: ");


	//initAdc();

	initEncoder();
	initKeyboard();
	while(1){
			char numEncoder[16], numFals[16];
			itoa(countEnc, numEncoder,  10);
			//itoa(AHigh, numFals,  10);
			LCD_ClearDisplay();
			LCD_SendString("Encoder: ");
			LCD_SendString(numEncoder);
			/*LCD_GotoXY(0, 1);
			LCD_SendString("NumFals: ");
			LCD_SendString(numFals);*/

			int32_t keyCode = readKeyboard();
			char txtKeyboard = convertKeyboardToChar(keyCode);
			if(txtKeyboard=='C'){
				countEnc = 0;
			}



			SLEEP_MS(100);
		}
	/*while(1){
			int16_t in8 = readChannel(8);
			char numAnalog[16];
			itoa(in8, numAnalog, 10);
			LCD_ClearDisplay();
			LCD_SendString("Analog: ");
			LCD_SendString(numAnalog);
			SLEEP_MS(300);
		}*/
	/*while(1){
			int16_t in8 = readT();
			char numDigit[3], numDec[3];
			itoa(in8/10, numDigit, 10);
			itoa(in8%10, numDec, 10);
			LCD_ClearDisplay();
			LCD_SendString("TEMP: ");
			LCD_SendString(numDigit);
			LCD_SendChar(',');
			LCD_SendString(numDec);
			SLEEP_MS(300);
		}*/

	/*while(1){
			int16_t tempInt = readVdd();
			char vddRead[6];
			itoa(tempInt, vddRead, 10);
			LCD_ClearDisplay();
			LCD_SendString("VDD: ");
			LCD_SendString(vddRead);
			SLEEP_MS(300);
		}*/
	/*while(1){
		int16_t vddVal = readVdd();
		int32_t calcVdd = (4095*121*1000)/vddVal;
		int16_t tempInt = readTAmpl(calcVdd);
		char numDigit[3], numDec[3];
		itoa(tempInt/10, numDigit, 10);
		itoa(tempInt%10, numDec, 10);
		LCD_ClearDisplay();
		LCD_SendString("TEMP: ");
		LCD_SendString(numDigit);
		LCD_SendChar(',');
		LCD_SendString(numDec);
		SLEEP_MS(300);
	}*/

	//initKeyboard();
	/*while(1){
		char resultLcd[8];
		int32_t keyCode = readKeyboard();
		LCD_ClearDisplay(); //Clear LCD display
	    LCD_SendString("Pressed Key: ");
	    LCD_GotoXY(0, 1);
	    itoa(keyCode, resultLcd,  10);
	    LCD_SendString(resultLcd);
	    LCD_GotoXY(5, 1);
	    char txtKeyboard = convertKeyboardToChar(keyCode);
	    LCD_SendChar(txtKeyboard);
		SLEEP_MS(50);
	}*/

	//SEARCH MULTI
	/*while(1){
		int32_t keyCode = readMultiKeyboard();
		LCD_ClearDisplay(); //Clear LCD display
		LCD_SendString("Pressed Key: ");
		LCD_GotoXY(0, 1);
	    //itoa(keyCode, resultLcd,  10);
		//LCD_SendString(resultLcd);
		char txtKeyboard[16];
		convertMultiToStr(keyCode, txtKeyboard);
		LCD_SendString(txtKeyboard);
		SLEEP_MS(50);
	}*/

	//KEYBOARD INTERRUPT
	//intConfigKeyboard();
	//intConfigCalculator();

	//initAccel();
	//int32_t whoAmI = readAccel(0x0F, true);
	//char resultLcd[8];
	/*int32_t accelX = readAccel(0x2D, true),
	sensi = ((321454)*(1<<15)/(accelX<<15));*/

	//itoa(whoAmI, resultLcd,  16);
	//itoa(sensi, resultSens,  16);
	//LCD_SendString(resultLcd);
	//LCD_GotoXY(0, 1);
	//LCD_SendString(resultSens);
	//SLEEP_MS(500);
	//LCD_ClearDisplay();

	//interruptTest();

	//accelToLCD();

	/*while(1){
		int32_t valX = readAccel(0x29, true);
		itoa(valX, resultValX,  10);
		LCD_ClearDisplay();
		LCD_GotoXY(5, 0);
		LCD_SendString(resultValX);
		SLEEP_MS(300);
	}*/

	// Return so that the compiler doesn't complain
	// It is not really needed as ledBlink never returns

	return 0;
}
