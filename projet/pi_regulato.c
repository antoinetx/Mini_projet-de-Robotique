#include "ch.h"
#include "hal.h"
#include <math.h>
#include <usbcfg.h>
#include <chprintf.h>
#include <motors.h>


#include <main.h>
#include <pi_regulato.h>
#include <optical_detection.h>

/*
//simple PI regulator implementation
int16_t pi_regulator(float distance, float goal){

	float error = 0;
	float speed = 0;

	static float sum_error = 0;

	error = distance - goal;

	//due to the noisy camera we set a minimal error for movement
	if(fabs(error) < ERROR_THRESHOLD){
		return 0;
	}

	sum_error += error;

	//we set a maximum and a minimum for the sum to avoid an uncontrolled growth
	if(sum_error > MAX_SUM_ERROR){
		sum_error = MAX_SUM_ERROR;
	}else if(sum_error < -MAX_SUM_ERROR){
		sum_error = -MAX_SUM_ERROR;
	}

	speed = KP * error + KI * sum_error;

	return (int16_t)speed;
}
*/

// When epuck approch a corner he avance to it with continuous comande because the camera see too far away.
void corner_approch(void){

	int16_t steps_traveled = 0;
	left_motor_set_pos(0);
	right_motor_set_pos(0);

	while(steps_traveled<NB_STEPS_TO_DO){
		// go on slower than before when approching
		if(steps_traveled > NB_STEPS_TO_DO/2){
			right_motor_set_speed(SYS_SPEED*((steps_traveled/NB_STEPS_TO_DO)+0.3));
			left_motor_set_speed(SYS_SPEED*((steps_traveled/NB_STEPS_TO_DO)+0.3));
		}
		else{
			right_motor_set_speed(SYS_SPEED);
			left_motor_set_speed(SYS_SPEED);
		}
		steps_traveled = left_motor_get_pos();
	}
	right_motor_set_speed(0);
	left_motor_set_speed(0);
}

void motor_turn(int16_t speed_left, int16_t speed_right){

	int16_t steps_traveled_left = 0;
	int16_t steps_traveled_right = 0;
	left_motor_set_pos(0);
	right_motor_set_pos(0);

	while((steps_traveled_left != NB_STEPS_TO_TURN) || (steps_traveled_right != NB_STEPS_TO_TURN)){

		right_motor_set_speed(speed_right);
		left_motor_set_speed(speed_left);

		steps_traveled_left = left_motor_get_pos();
		steps_traveled_right = right_motor_get_pos();
	}
	right_motor_set_speed(0);
	left_motor_set_speed(0);
}


void corner_turn(int16_t side){
	switch (side)
	{
	case RIGHT:
		motor_turn(SYS_SPEED, 0);
		break;
	case LEFT:
		motor_turn(0, SYS_SPEED);
		break;

	}
}

// PID regulator for the rotational line follower
int16_t pid_regulator(float error){

	float rot_speed = 0;
	static float sum_error = 0, old_error;

	//due to the noisy camera we set a minimal error for movement
	if(fabs(error) < ERROR_THRESHOLD){
		return 0;
	}

	sum_error += error;

	//we set a maximum and a minimum for the sum to avoid an uncontrolled growth
	if(sum_error > MAX_SUM_ERROR){
		sum_error = MAX_SUM_ERROR;
	}else if(sum_error < -MAX_SUM_ERROR){
		sum_error = -MAX_SUM_ERROR;
	}

	//rot_speed = KP * error + KI * sum_error +  KD * ((error - old_error)/D_T);
	rot_speed = KP * error + KD * ((error - old_error)/D_T);
	 //chprintf((BaseSequentialStream *)&SDU1, " \n error %d", get_error_line());

	old_error = error;
	return (int16_t)rot_speed;
}


static THD_WORKING_AREA(waPidRegulator, 256);
static THD_FUNCTION(PidRegulator, arg) {

	chRegSetThreadName(__FUNCTION__);
	(void)arg;

	systime_t time;

	int16_t rotation_speed = 0;

	while(1){
		time = chVTGetSystemTime();

		//computes the speed to give to the motors
		//distance_cm is modified by the image processing thread

		rotation_speed = pid_regulator(get_error_line());
		//computes a correction factor to let the robot rotate to be in front of the line


		//applies the speed from the PI regulator and the correction for the rotation
		right_motor_set_speed(SYS_SPEED - rotation_speed);
		left_motor_set_speed(SYS_SPEED + rotation_speed);

		//100Hz
		chThdSleepUntilWindowed(time, time + MS2ST(10));
	}
}


void pid_regulator_start(void){
	chThdCreateStatic(waPidRegulator, sizeof(waPidRegulator), NORMALPRIO, PidRegulator, NULL);
}





