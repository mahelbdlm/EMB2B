/*
 * analog.h
 *
 *  Created on: Apr 21, 2023
 *      Author: mahel.berthod
 */

#ifndef ANALOG_H_
#define ANALOG_H_

void initAdc(void);
int16_t readIN8(void);
int32_t readChannel(int32_t channelNum);
int32_t readT(void);
int32_t readVdd(void);
int32_t readTAmpl(int32_t vddVal);




#endif /* ANALOG_H_ */
