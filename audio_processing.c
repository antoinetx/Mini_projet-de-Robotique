#include "ch.h"
#include "hal.h"
#include <main.h>
#include <usbcfg.h>
#include <chprintf.h>

#include <audio/microphone.h>
#include <audio_processing.h>
#include <fft.h>
#include <arm_math.h>


//2 times FFT_SIZE because these arrays contain complex numbers (real + imaginary)
static float micLeft_cmplx_input[2 * FFT_SIZE];
static float micRight_cmplx_input[2 * FFT_SIZE];
static float micFront_cmplx_input[2 * FFT_SIZE];
static float micBack_cmplx_input[2 * FFT_SIZE];

//Arrays containing the computed magnitude of the complex numbers
static float micLeft_output[FFT_SIZE];
static float micRight_output[FFT_SIZE];
static float micFront_output[FFT_SIZE];
static float micBack_output[FFT_SIZE];



static float angle=0., amp=0., freq=0.;

typedef enum {
	MIC_RIGHT_I = 0,
	MIC_LEFT_I,
	MIC_BACK_I,
	MIC_FRONT_I
} MICROPHONE_INDEX;


void epuck_angle(float angle_x,float angle_y){
	if (angle_x>0 && angle_y>0){
		angle=(angle_x+(PI/2-angle_y))/2;
	}
	if (angle_x>0 && angle_y<0){
		angle=PI/2+(-angle_y+(PI/2-angle_x))/2;

	}
	if (angle_x<0 && angle_y<0){
		angle=PI+(-angle_x+(PI/2+angle_y))/2;
		angle = angle-2*PI;
	}
	if (angle_x<0 && angle_y>0){
		angle=3*PI/2+(PI/2+angle_x+angle_y)/2;
		angle = angle-2*PI ;
	}
	if (freq <= 0) angle=0;
}

void triangulation_data(void){

	float max_norm[4] = {MIN_NORM_VALUE,MIN_NORM_VALUE,MIN_NORM_VALUE,MIN_NORM_VALUE};
	int16_t max_norm_index[4] = {-1,-1,-1,-1};

	float phase_shift_right,phase_shift_left,cos_phi,phase_shift_diff_x, phase_shift_front, phase_shift_back, phase_shift_diff_y;
	static float mean_phase_shift_x=0., mean_phase_shift_y=0.;
	static float angle_y=0., angle_x=0.;

	//search for the highest peak
	for(uint16_t i = MIN_FREQ ; i <= MAX_FREQ ; i++){
		if(micRight_output[i] > max_norm[MIC_RIGHT_I]){
			max_norm[MIC_RIGHT_I] = micRight_output[i];
			max_norm_index[MIC_RIGHT_I] = i;
		}
	}
	for(uint16_t i = MIN_FREQ ; i <= MAX_FREQ ; i++){
		if(micLeft_output[i] > max_norm[MIC_LEFT_I]){
			max_norm[MIC_LEFT_I] = micLeft_output[i];
			max_norm_index[MIC_LEFT_I] = i;
		}
	}
	for(uint16_t i = MIN_FREQ ; i <= MAX_FREQ ; i++){
		if(micBack_output[i] > max_norm[MIC_BACK_I]){
			max_norm[MIC_BACK_I] = micBack_output[i];
			max_norm_index[MIC_BACK_I] = i;
		}
	}
	for(uint16_t i = MIN_FREQ ; i <= MAX_FREQ ; i++){
		if(micFront_output[i] > max_norm[MIC_FRONT_I]){
			max_norm[MIC_FRONT_I] = micFront_output[i];
			max_norm_index[MIC_FRONT_I] = i;
		}
	}

	//Find the phase shift. teta = Im(DFT(signal))/Re(DFT(signal).
	if((max_norm_index[MIC_LEFT_I] == max_norm_index[MIC_RIGHT_I]) && (max_norm_index[MIC_LEFT_I] >MIN_FREQ)
			&& (micLeft_output[max_norm_index[MIC_LEFT_I]]>MIN_NORM_VALUE))
	{
		phase_shift_left= atan2f(micLeft_cmplx_input[2*max_norm_index[MIC_LEFT_I]+1], micLeft_cmplx_input[2*max_norm_index[MIC_LEFT_I]]);
		phase_shift_right= atan2f(micRight_cmplx_input[2*max_norm_index[MIC_RIGHT_I]+1], micRight_cmplx_input[2*max_norm_index[MIC_RIGHT_I]]);
		phase_shift_diff_x=(phase_shift_left-phase_shift_right);

		//filter
		if ((-PI < phase_shift_diff_x) && (phase_shift_diff_x<PI)){
			// Mobile mean to avoid to big changes

			mean_phase_shift_x = mean_coeff_A*mean_phase_shift_x+mean_coeff_B*phase_shift_diff_x;
			cos_phi = SIGNAL_CONST*mean_phase_shift_x/(max_norm_index[MIC_LEFT_I]);
			if (cos_phi > 1) cos_phi=1;
			if (cos_phi < -1) cos_phi=-1;
			angle_x = asin(cos_phi);
		}
	}
	if((max_norm_index[MIC_BACK_I] == max_norm_index[MIC_FRONT_I]) && (max_norm_index[MIC_BACK_I] >MIN_FREQ)
		 && (micFront_output[max_norm_index[MIC_FRONT_I]]>MIN_NORM_VALUE))
	{
		phase_shift_front= atan2f(micFront_cmplx_input[2*max_norm_index[MIC_FRONT_I]+1], micFront_cmplx_input[2*max_norm_index[MIC_FRONT_I]]);
		phase_shift_back= atan2f(micBack_cmplx_input[2*max_norm_index[MIC_BACK_I]+1], micBack_cmplx_input[2*max_norm_index[MIC_BACK_I]]);
		phase_shift_diff_y=(phase_shift_front-phase_shift_back);
		if ((-PI < phase_shift_diff_y) && (phase_shift_diff_y<PI)){
			mean_phase_shift_y = mean_coeff_A*mean_phase_shift_y+mean_coeff_B*phase_shift_diff_y;
			cos_phi = SIGNAL_CONST*mean_phase_shift_y/(max_norm_index[MIC_LEFT_I]);
			if (cos_phi > 1) cos_phi=1;
			if (cos_phi < -1) cos_phi=-1;
			angle_y = asin(cos_phi);
		}
	}
	if ((max_norm_index[MIC_LEFT_I] == max_norm_index[MIC_RIGHT_I])&&(max_norm_index[MIC_BACK_I] == max_norm_index[MIC_FRONT_I])){
		freq=max_norm_index[MIC_LEFT_I];
	}
	amp=micFront_output[max_norm_index[MIC_FRONT_I]];
	epuck_angle(angle_x,angle_y);
}


void processAudioData(int16_t *data, uint16_t num_samples){
/*
*
* We get 160 samples per mic every 10ms
* So we fill the samples buffers to reach
* 1024 samples, then we compute the FFTs.
*
*/

	static uint16_t nb_samples = 0;
	//loop to fill the buffers
	for(uint16_t i = 0 ; i < num_samples ; i+=4){

	//construct an array of complex numbers. Put 0 to the imaginary part
		micRight_cmplx_input[nb_samples] = (float)data[i + MIC_RIGHT];
		micLeft_cmplx_input[nb_samples] = (float)data[i + MIC_LEFT];
		micBack_cmplx_input[nb_samples] = (float)data[i + MIC_BACK];
		micFront_cmplx_input[nb_samples] = (float)data[i + MIC_FRONT];

		nb_samples++;

		micRight_cmplx_input[nb_samples] = 0;
		micLeft_cmplx_input[nb_samples] = 0;
		micBack_cmplx_input[nb_samples] = 0;
		micFront_cmplx_input[nb_samples] = 0;

		nb_samples++;

		//stop when buffer is full
		if(nb_samples >= (2 * FFT_SIZE)){
			break;
		}
	}

	if(nb_samples >= (2 * FFT_SIZE)){

		/* FFT proccessing
		*
		* This FFT function stores the results in the input buffer given.
		* This is an "In Place" function.
		*/
		doFFT_optimized(FFT_SIZE, micRight_cmplx_input);
		doFFT_optimized(FFT_SIZE, micLeft_cmplx_input);
		doFFT_optimized(FFT_SIZE, micFront_cmplx_input);
		doFFT_optimized(FFT_SIZE, micBack_cmplx_input);

		/* Magnitude processing
		*
		* Computes the magnitude of the complex numbers and
		* stores them in a buffer of FFT_SIZE because it only contains
		* real numbers.
		*
		*/
		arm_cmplx_mag_f32(micRight_cmplx_input, micRight_output, FFT_SIZE);
		arm_cmplx_mag_f32(micLeft_cmplx_input, micLeft_output, FFT_SIZE);
		arm_cmplx_mag_f32(micFront_cmplx_input, micFront_output, FFT_SIZE);
		arm_cmplx_mag_f32(micBack_cmplx_input, micBack_output, FFT_SIZE);

		nb_samples = 0;
		triangulation_data();

	}
}

float get_angle(void){
	return angle;
}

float get_freq(void){
	return freq;
}

float get_amp(void){
	return amp;
}

