#ifndef PROJET_AUDIO_PROCESSING_H_
#define PROJET_AUDIO_PROCESSING_H_


#define FFT_SIZE 	1024

#define MIN_VALUE_THRESHOLD 10000

// Garde pour tests
#define MIN_FREQ 10 //we don’t analyze before this index to not use resources for nothing
#define MAX_FREQ 30 //we don’t analyze after this index to not use resources for nothing

//minimal intensity so that we don't compute the noise
#define MIN_NORM_VALUE 10000

// Mobile mean parameters : angle = a*angle +b*angle_buf
#define mean_coeff_A					0.7f
#define mean_coeff_B					0.3f

// SIGNAL_CONST=v/(2*PI*lx), lx distance between 2 microphones. lx=6cm
// SIGNAL_CONST=340/(2*PI*lx)
#define SIGNAL_CONST			128.112


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

float get_angle(void);

float get_freq(void);

float get_amp(void);

void mouvement(void);

#endif /* PROJET_AUDIO_PROCESSING_H_ */
