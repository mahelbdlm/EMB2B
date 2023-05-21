/****************************************************

 k e y b o a r d . h

 P2 KEYBOARD Header file

*****************************************************/

#ifndef _KEYBOARD_H   // Definition so that the file
#define _KEYBOARD_H   // is only included once

void setOutputOpenDrain(GPIO_TypeDef *port, int32_t numPin);
void setInputPullUp(GPIO_TypeDef *port, int32_t numPin);
void initKeyboard(void);
char convertKeyboardToChar(int32_t keyboardCode);
char convertMultiKeyboardToChar(int32_t keyboardCode);
int32_t readKeyboard(void);
void convertMultiToStr(int32_t multiCode, char *strToDisp);
int32_t readMultiKeyboard(void);
void intConfigKeyboard(void);
CH_IRQ_HANDLER(EXTI9_5_IRQHandler);
void intConfigCalculator(void);


#endif //_KEYBOARD_H


