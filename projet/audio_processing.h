/*
 * audio_processing.h
 *
 *  Created on: Apr 15, 2021
 *      Author: antoine
 */

#ifndef PROJET_AUDIO_PROCESSING_H_
#define PROJET_AUDIO_PROCESSING_H_

#define FFT_SIZE 	1024

typedef enum {
	//2 times FFT_SIZE because these arrays contain complex numbers (real + imaginary)
	LEFT_CMPLX_INPUT = 0,
	RIGHT_CMPLX_INPUT,
	FRONT_CMPLX_INPUT,
	BACK_CMPLX_INPUT,
	//Arrays containing the computed magnitude of the complex numbers
	LEFT_OUTPUT,
	RIGHT_OUTPUT,
	FRONT_OUTPUT,
	BACK_OUTPUT
} BUFFER_NAME_t;


void processAudioData(int16_t *data, uint16_t num_samples);

/*
*	put the invoking thread into sleep until it can process the audio datas
*/
void wait_triangulation_data(void);

float get_angle(void);

float get_freq(void);

float get_amp(void);

#endif /* PROJET_AUDIO_PROCESSING_H_ */
