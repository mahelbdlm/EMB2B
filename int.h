/**************************************************************

 i n t . h

 Interrupt test source file

***************************************************************/

#ifndef _INT_H   // Definitions so that the
#define _INT_H   // file is included only once

/****** Public function prototypes *******/

void interruptTest(void);

CH_IRQ_HANDLER(EXTI0_IRQHandler);

#endif   // int.h

