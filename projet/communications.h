/*
 * communications.h
 *
 *  Created on: Apr 15, 2021
 *      Author: antoine
 */
#ifndef PROJET_COMMUNICATIONS_H_
#define PROJET_COMMUNICATIONS_H_

void SendFloatToComputer(BaseSequentialStream* out, float* data, uint16_t size);

uint16_t ReceiveInt16FromComputer(BaseSequentialStream* in, float* data, uint16_t size);


#endif /* PROJET_COMMUNICATIONS_H_ */
