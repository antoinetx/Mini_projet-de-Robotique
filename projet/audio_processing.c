#include "ch.h"
#include "hal.h"
#include <main.h>
#include <usbcfg.h>
#include <chprintf.h>
#include <motors.h>     // Garde pour tests
#include <audio/microphone.h>
#include <audio_processing.h>
#include <communications.h>
#include <fft.h>
#include <arm_math.h>
#include <leds.h>

//semaphore
static BSEMAPHORE_DECL(triangulation, TRUE);

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

#define MIN_VALUE_THRESHOLD 10000

// Garde pour tests
#define MIN_FREQ 10 //we don’t analyze before this index to not use resources for nothing
#define FREQ_FORWARD 13 //250Hz
#define FREQ_LEFT 19 //296Hz
#define FREQ_RIGHT 23 //359HZ
#define FREQ_BACKWARD 26 //406Hz
#define MAX_FREQ 30 //we don’t analyze after this index to not use resources for nothing


//minimal intensity so that we don't compute the noise
#define MIN_NORM_VALUE 10000

// Mobile mean parameters : angle = a*angle +b*angle_buf
#define mean_coeff_A					0.7f
#define mean_coeff_B					0.3f

// SIGNAL_CONST=v/(2*PI*lx), lx distance between 2 microphones. lx=6cm
// SIGNAL_CONST=340/(2*PI*lx)
#define SIGNAL_CONST			128.112 // 59.21 avec résolution audio

//PID
#define ERROR_THRESHOLD  0.5
#define MAX_SUM_ERROR  1
#define KP 800
#define KI 3.5
#define ROTATION_THRESHOLD 0
#define COEFF_SPEED 0.4
static float speed = 0;

//------

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
	}
	if (angle_x<0 && angle_y>0){
		angle=3*PI/2+(PI/2+angle_x+angle_y)/2;
	}
	//angle = angle*180/PI;
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
			cos_phi = SIGNAL_CONST	*mean_phase_shift_y/(max_norm_index[MIC_LEFT_I]);
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


/*
* Callback called when the demodulation of the four microphones is done.
* We get 160 samples per mic every 10ms (16kHz)
*
* params :
* int16_t *data Buffer containing 4 times 160 samples. the samples are sorted by micro
* so we have [micRight1, micLeft1, micBack1, micFront1, micRight2, etc...]
* uint16_t num_samples Tells how many data we get in total (should always be 640)
*/

void controller(void){
		float error = 0;

		static float sum_error = 0;
		int16_t goal = 0;
		error = angle - goal;

		//disables the PI regulator if the error is to small
		//this avoids to always move as we cannot exactly be where we want and
		//the camera is a bit noisy

		sum_error += error;

		//we set a maximum and a minimum for the sum to avoid an uncontrolled growth
		if(sum_error > MAX_SUM_ERROR){
			sum_error = MAX_SUM_ERROR;
		}else if(sum_error < -MAX_SUM_ERROR){
			sum_error = -MAX_SUM_ERROR;
		}

		speed = KP * error + KI * sum_error;


		if(fabs(error) < ERROR_THRESHOLD){
			speed = 0;
		}

		if(error<PI){
			speed = COEFF_SPEED*speed;
		}
		if(error>PI){
			speed = -COEFF_SPEED*speed;
		}
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
		chBSemSignal(&triangulation);
		triangulation_data();
		controller();
		mouvement();
		//sound_remote(micLeft_output);

	}

}


//------------------ mouvement------------------


void mouvement(void){

	right_motor_set_speed(speed);
	left_motor_set_speed(-speed);
    		/*
		if( (0<angle) & (angle<1)){
			right_motor_set_speed(0);
			left_motor_set_speed(0);
		}

		if(angle>PI){
			//applies the speed from the PI regulator and the correction for the rotation
        		//right_motor_set_speed(speed - ROTATION_COEFF * speed_correction);
        		//left_motor_set_speed(speed + ROTATION_COEFF * speed_correction);
			right_motor_set_speed(-speed);
			left_motor_set_speed(speed);
		}
		if(angle<PI){
        			//applies the speed from the PI regulator and the correction for the rotation
        			right_motor_set_speed(speed);
        			left_motor_set_speed(-speed);
        	}
        	*/

}


//---------------------------------- fin mouvement


void set_tracking_leds(float angle){
	clear_leds();

	if ((angle<2*PI/8)  || (angle>14*PI/8))   	set_led(LED1,1);
	if ((PI/8<angle)    && (3*PI/8>angle))		set_rgb_led(LED8,255,0,0);
	if ((2*PI/8<angle)  && (6*PI/8>angle))   	set_led(LED7,1);
	if ((5*PI/8<angle)  && (7*PI/8>angle))		set_rgb_led(LED6,255,0,0);
	if ((6*PI/8<angle)  && (10*PI/8>angle))		set_led(LED5,1);
	if ((9*PI/8<angle)  && (11*PI/8>angle))		set_rgb_led(LED4,255,0,0);
	if ((10*PI/8<angle) && (14*PI/8>angle)) 	set_led(LED3,1);
	if ((13*PI/8<angle) && (15*PI/8>angle))		set_rgb_led(LED2,255,0,0);

	set_front_led(1);
}


void wait_triangulation_data(void){
	chBSemWait(&triangulation);
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
