/*
 * encoder.h
 *
 *  Created on: May 8, 2023
 *      Author: mahel.berthod
 */

#ifndef ENCODER_H_
#define ENCODER_H_

void initEncoder(void);
CH_IRQ_HANDLER(EXTI1_IRQHandler);
CH_IRQ_HANDLER(EXTI3_IRQHandler);


#endif /* ENCODER_H_ */
