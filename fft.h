/*
 * fft.h
 *
 *  Created on: Apr 15, 2021
 *      Author: antoine
 */

#ifndef PROJET_FFT_H_
#define PROJET_FFT_H_



typedef struct complex_float{
	float real;
	float imag;
}complex_float;

void doFFT_optimized(uint16_t size, float* complex_buffer);

void doFFT_c(uint16_t size, complex_float* complex_buffer);




#endif /* PROJET_FFT_H_ */
