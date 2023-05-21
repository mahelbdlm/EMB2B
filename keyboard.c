/*
 KEYBOARD.C
 MANAGES KEYBOARD AND ALL THAT STUFF
 */

#include "Base.h"     // Basic definitions
#include "keyboard.h" // LCD definitions
#include "accel.h" //For itoa function
volatile int32_t keyboardCode;

void setOutputOpenDrain(GPIO_TypeDef *port, int32_t numPin) {
	/*
	 Sets pin to output Open drain
	 INPUTS:
	 port: GPIO port
	 pinNum: Number of pin to change
	 */
	//numPin &= 0x0000FFFF; // Makes sure to leave unchanged reserved bits
	(port->MODER) = ((port->MODER) & (~(1 << (2 * numPin + 1))))
			| (1 << (2 * numPin));
	// Sets MODER to 01-> Output
	port->OTYPER |= BIT(numPin);
	// Sets OTYPER to 1 -> OpenDrain
}

void setInputPullUp(GPIO_TypeDef *port, int32_t numPin) {
	/*
	 Sets PIN_NUM of port as Open drain
	 INPUTS:
	 port: GPIO port
	 numPin: Number of pin to change
	 */
	(port->MODER) = (port->MODER) & ~(0x3 << (2 * numPin));
	// Sets MODER to 00 -> Input
	(port->PUPDR) = ((port->PUPDR) & (~(1 << (2 * numPin + 1))))
			| (1 << (2 * numPin));
	// Sets PUPDR to 01 -> Input Pull Up
}

void initKeyboard(void) {
	/*
	 As name indicates, initialises keyboard in un uncanny manner
	 INPUTS: NONE
	 OUTPUTS: NONE
	 CONTEXT:
	 Needs functions setOutputOpenDrain and setInputPullUp
	 defined above
	 */
	setOutputOpenDrain(KEY_PORT, KEY_ROW1_PAD);
	setOutputOpenDrain(KEY_PORT, KEY_ROW2_PAD);
	setOutputOpenDrain(KEY_PORT, KEY_ROW3_PAD);
	setOutputOpenDrain(KEY_PORT, KEY_ROW4_PAD);

	setInputPullUp(KEY_PORT, KEY_COL1_PAD);
	setInputPullUp(KEY_PORT, KEY_COL2_PAD);
	setInputPullUp(KEY_PORT, KEY_COL3_PAD);
	setInputPullUp(KEY_PORT, KEY_COL4_PAD);
}

char convertKeyboardToChar(int32_t keyboardCode) {
	char TECLES[16] = "123A456B789C*0#D";
	if (keyboardCode >= 0 && keyboardCode <= 15) {
		int32_t kbcKode = keyboardCode;
		char tkla = TECLES[keyboardCode];
		return (TECLES[keyboardCode]);
	} else if (keyboardCode == 32) {
		return ('N');
	} else {
		return ('E');
	}
}

int32_t readKeyboard(void) {
	/*
	 Reads the key pressed (or not) by user
	 INPUT: NONE
	 OUTPUT: Code of pressed key
	 */
	KEY_PORT->ODR = (KEY_PORT->ODR) | KEY_ROW1_BIT | KEY_ROW2_BIT | KEY_ROW3_BIT
			| KEY_ROW4_BIT;
	//Sets every ROW to 1
	int32_t indexRow = KEY_ROW1_PAD, indexCol = KEY_COL1_PAD, keyDiscovered = 0;
	while (indexRow <= KEY_ROW4_PAD && keyDiscovered == 0) {
		(KEY_PORT->BSRR.H.clear) = BIT(indexRow);
		DELAY_US(40);
		indexCol = KEY_COL1_PAD;
		while ((indexCol <= KEY_COL4_PAD) && keyDiscovered == 0) {
			if (((KEY_PORT->IDR) & BIT(indexCol))==0){
				keyDiscovered = 1;
			}
			indexCol++;
		}
		(KEY_PORT->BSRR.H.set) = BIT(indexRow);
		indexRow++;
	}

	if (keyDiscovered == 0) {
		return (32);
	} else {
		indexRow -= KEY_ROW1_PAD + 1;
		indexCol -= KEY_COL1_PAD + 1;
		return (indexCol | (indexRow << 2));
	}
}

char convertMultiKeyboardToChar(int32_t keyboardCode) {
	char TECLES[16] = "123A456B789C*0#D";
	if (keyboardCode >= 0 && keyboardCode <= 15) {
		return (TECLES[keyboardCode]);
	} else if (keyboardCode == 16) {
		return ('N');
	} else {
		return ('E');
	}
}
void convertMultiToStr(int32_t multiCode, char *strToDisp) {
	int32_t i = 0, j = 0;
	for (i = 0; i <= 31; i++) {
		if ((multiCode & (1 << i)) != 0) {
			strToDisp[j] = convertMultiKeyboardToChar(i);
			j++;
		}
	}
	strToDisp[j] = '\0';
}

int32_t readMultiKeyboard(void) {
	/*
	 Reads various key pressed (or not) by user
	 INPUT: NONE
	 OUTPUT: Code of pressed keys
	 */
	KEY_PORT->ODR = (KEY_PORT->ODR) | KEY_ROW1_BIT | KEY_ROW2_BIT | KEY_ROW3_BIT
			| KEY_ROW4_BIT;
	//Sets every ROW to 1
	int32_t indexRow = KEY_ROW1_PAD, indexCol = KEY_COL1_PAD, keyDiscovered = 0,
			resultKeys = 0;
	while ((indexRow <= KEY_ROW4_PAD)) {
		(KEY_PORT->BSRR.H.clear) = BIT(indexRow);
		DELAY_US(40);
		indexCol = KEY_COL1_PAD;
		while ((indexCol <= KEY_COL4_PAD)) {
			if (((KEY_PORT->IDR) & BIT(indexCol))==0){
				keyDiscovered = 1;
				resultKeys |=
						BIT(((indexCol-KEY_COL1_PAD)|((indexRow-KEY_ROW1_PAD)<<2)));
			}
			indexCol++;
		}
		(KEY_PORT->BSRR.H.set) = BIT(indexRow);
		indexRow++;
	}

	if (keyDiscovered == 0) {
		return (BIT(16));
	} else {
		return (resultKeys);
	}
}

void intConfigKeyboard(void) {
	KEY_PORT->ODR = (KEY_PORT->ODR) & ~KEY_ROW1_BIT & ~KEY_ROW2_BIT
			& ~KEY_ROW3_BIT & ~KEY_ROW4_BIT;
	//Sets all rows to 0
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN; //Activate clock of SYSCFG

	//Configuration of SYSCFG MUX to point to port D
	SYSCFG->EXTICR[1] = (SYSCFG->EXTICR[1] & ~SYSCFG_EXTICR2_EXTI6
			& ~SYSCFG_EXTICR2_EXTI7) | SYSCFG_EXTICR2_EXTI6_PD
			| SYSCFG_EXTICR2_EXTI7_PD;
	SYSCFG->EXTICR[2] = (SYSCFG->EXTICR[2] & ~SYSCFG_EXTICR3_EXTI8
			& ~SYSCFG_EXTICR3_EXTI9) | SYSCFG_EXTICR3_EXTI8_PD
			| SYSCFG_EXTICR3_EXTI9_PD;

	//Enable interrupt for ports 6 to 9
	EXTI->IMR = EXTI->IMR | EXTI_IMR_MR6 | EXTI_IMR_MR7 | EXTI_IMR_MR8
			| EXTI_IMR_MR9;

	EXTI->RTSR = EXTI->RTSR & ~EXTI_RTSR_TR6 & ~EXTI_RTSR_TR7 & ~EXTI_RTSR_TR8
			& ~EXTI_RTSR_TR9; //Disable rising edge for ports 6 to 9 just in case
	EXTI->FTSR = EXTI->FTSR | EXTI_FTSR_TR6 | EXTI_FTSR_TR7 | EXTI_FTSR_TR8
			| EXTI_FTSR_TR9;  //Enable falling edge for ports 6 to 9

	EXTI->PR = EXTI_PR_PR6 | EXTI_PR_PR7 | EXTI_PR_PR8 | EXTI_PR_PR9;
	//Erase all possible pending interruption for ports 6 to 9

	nvicEnableVector(EXTI9_5_IRQn,
			CORTEX_PRIORITY_MASK(STM32_EXT_EXTI5_9_IRQ_PRIORITY));

	while (1) {
		keyboardCode = 32; //Deletes SwitchFlag
		while (keyboardCode == 32)
			;  //Waits for switchFlag to take val 1
		LCD_ClearDisplay(); //Clear LCD display
		LCD_SendString("Pressed Key: ");
		LCD_GotoXY(0, 1);
		char txtKeyboard = convertKeyboardToChar(keyboardCode);
		LCD_SendChar(txtKeyboard);

	}
}

// EXTI 6 ISR
CH_IRQ_HANDLER( EXTI9_5_IRQHandler) {
	CH_IRQ_PROLOGUE();
	// Start of the ISR code
	keyboardCode = 32;
	int32_t colNum = KEY_COL1_PAD, colFound = 0, rowNum = KEY_ROW1_PAD,
			keyDiscovered = 0;
	const int imrMaskEnable = EXTI_IMR_MR6 | EXTI_IMR_MR7 | EXTI_IMR_MR8
			| EXTI_IMR_MR9;

	//Disable interrupt for ports 5 to 9. Port 5 is only for security purposes
	EXTI->IMR &= ~imrMaskEnable;

	if ((EXTI->PR & KEY_COL1_BIT)!=0
	|| (EXTI->PR & KEY_COL2_BIT)!=0
	|| (EXTI->PR & KEY_COL3_BIT)!=0
	|| (EXTI->PR & KEY_COL4_BIT)!=0){while(colNum<=KEY_COL4_PAD && colFound==0) {
		if((EXTI->PR & 1<<colNum)!=0) {
			colFound=1;
		}
		colNum++;
	}
	colNum--;
	if(colFound==0) {
		keyboardCode = 0x10; //Error code
	}
	//colNum now has the value of the column which triggered
	//We will now change the values of the row and check
	KEY_PORT->ODR = ((KEY_PORT->ODR) & ~KEY_ROW1_BIT) | KEY_ROW2_BIT | KEY_ROW3_BIT | KEY_ROW4_BIT;
	//Sets ROW1 to 0 and rows 2 to 4 to 1
	while(rowNum<=KEY_ROW4_PAD && keyDiscovered==0) {
		(KEY_PORT->BSRR.H.clear) = BIT(rowNum);
		DELAY_US(40); //Delay because CPU is super fast
		if(((KEY_PORT->IDR) & BIT(colNum))==0) {
			//That means that setting row to 1 makes the column be 1
			keyDiscovered = 1;
			keyboardCode = (colNum-KEY_COL1_PAD)|((rowNum-KEY_ROW1_PAD)<<2);
		}
		(KEY_PORT->BSRR.H.set) = BIT(rowNum);
		rowNum++;
	}
	if(keyDiscovered==0) {
		keyboardCode = 32;
	}
}
else {
	//Port 5 was triggered
	keyboardCode = 0x10;//Error code
}

	//Set all rows to 0
	KEY_PORT->ODR = (KEY_PORT->ODR) & ~KEY_ROW1_BIT & ~KEY_ROW2_BIT
			& ~KEY_ROW3_BIT & ~KEY_ROW4_BIT;

	DELAY_US(100); //Delay to avoid button bounce

	//Erase all possible pending interruption for ports 6 to 9
	EXTI->PR = EXTI_PR_PR5 | EXTI_PR_PR6 | EXTI_PR_PR7 | EXTI_PR_PR8
			| EXTI_PR_PR9;

	//Enable interrupt for ports 6 to 9
	EXTI->IMR |= imrMaskEnable;
	// End of the ISR code
	CH_IRQ_EPILOGUE();
}

void intConfigCalculator(void) {
	KEY_PORT->ODR = (KEY_PORT->ODR) & ~KEY_ROW1_BIT & ~KEY_ROW2_BIT
			& ~KEY_ROW3_BIT & ~KEY_ROW4_BIT;
	//Sets all rows to 0
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN; //Activate clock of SYSCFG

	//Configuration of SYSCFG MUX to point to port D
	SYSCFG->EXTICR[1] = (SYSCFG->EXTICR[1] & ~SYSCFG_EXTICR2_EXTI6
			& ~SYSCFG_EXTICR2_EXTI7) | SYSCFG_EXTICR2_EXTI6_PD
			| SYSCFG_EXTICR2_EXTI7_PD;
	SYSCFG->EXTICR[2] = (SYSCFG->EXTICR[2] & ~SYSCFG_EXTICR3_EXTI8
			& ~SYSCFG_EXTICR3_EXTI9) | SYSCFG_EXTICR3_EXTI8_PD
			| SYSCFG_EXTICR3_EXTI9_PD;

	//Enable interrupt for ports 6 to 9
	EXTI->IMR = EXTI->IMR | EXTI_IMR_MR6 | EXTI_IMR_MR7 | EXTI_IMR_MR8
			| EXTI_IMR_MR9;

	EXTI->RTSR = EXTI->RTSR & ~EXTI_RTSR_TR6 & ~EXTI_RTSR_TR7 & ~EXTI_RTSR_TR8
			& ~EXTI_RTSR_TR9; //Disable rising edge for ports 6 to 9 just in case
	EXTI->FTSR = EXTI->FTSR | EXTI_FTSR_TR6 | EXTI_FTSR_TR7 | EXTI_FTSR_TR8
			| EXTI_FTSR_TR9;  //Enable falling edge for ports 6 to 9

	EXTI->PR = EXTI_PR_PR6 | EXTI_PR_PR7 | EXTI_PR_PR8 | EXTI_PR_PR9;
	//Erase all possible pending interruption for ports 6 to 9

	nvicEnableVector(EXTI9_5_IRQn,
			CORTEX_PRIORITY_MASK(STM32_EXT_EXTI5_9_IRQ_PRIORITY));

	int32_t maxNumLength = 15;
	int32_t numLength = 0, posOp = 0;
	char num[16] = "\0";

	while (1) {
		keyboardCode = 32; //Deletes SwitchFlag
		while (keyboardCode == 32)
			;  //Waits for switchFlag to take val 1
		char txtKeyboard = convertKeyboardToChar(keyboardCode);
		if (txtKeyboard >= '1' && txtKeyboard <= '9' && numLength <= maxNumLength) {
			num[numLength] = txtKeyboard;
			numLength++;
		}
		switch (txtKeyboard) {
		case ('*'):
			num[numLength] = txtKeyboard;
			posOp = numLength;
			numLength++;
			break;
		case ('#'):
			num[numLength] = '/';
			posOp = numLength;
			numLength++;
			break;
		case ('A'):
			num[numLength] = '+';
			posOp = numLength;
			numLength++;
			break;
		case ('B'):
			num[numLength] = '-';
			posOp = numLength;
			numLength++;
			break;

		}
		if (txtKeyboard == 'D') {
			//num1[numLength] = '\0';
			int32_t i = 0, num1=0, num2=0, result=0;
			for(i=0; i<posOp; i++){
				int32_t j = 0, power=1;
				for(j=0; j<posOp-i-1; j++){
					power = power * 10;
				}
				num1 += (num[i] - '0') * power;
			}

			for(i=posOp+1; i<numLength; i++){
				int32_t j = 0, power=1;
				for(j=0; j<numLength-i-1; j++){
					power = power * 10;
				}
				num2 += (num[i] - '0') * power;
			}

			if(num[posOp]=='*'){
				result = num1*num2;
			}
			if(num[posOp]=='/'){
				result = num1/num2;
			}
			if(num[posOp]=='+'){
				result = num1+num2;
			}
			if(num[posOp]=='-'){
				result = num1-num2;
			}
			char resultLcd[8];
			itoa(result, resultLcd,  10);
			LCD_GotoXY(0, 1);
			LCD_SendString(resultLcd);
		}
		else if (txtKeyboard == 'C') {
			int32_t j = 0;
			for(j=0; j<16; j++){
				num[j]='\0';
			}
			numLength = 0;
			posOp = 0;
			LCD_ClearDisplay(); //Clear LCD display
		}
		else{
			LCD_ClearDisplay(); //Clear LCD display
			//LCD_SendString("Pressed Key: ");
			//LCD_GotoXY(0, 1);
			LCD_SendString(num);
		}

	}
}

