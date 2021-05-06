#include "ch.h"
#include "hal.h"
#include <math.h>
#include <usbcfg.h>
#include <chprintf.h>
#include <motors.h>


#include <main.h>
#include <pi_regulato.h>
#include <optical_detection.h>
#include <audio_processing.h>
#include <sensors/VL53L0X/VL53L0X.h>


// Make a choice to turn
int8_t turn_choice(void){
	float angle = get_angle();
	if( angle>PI/4 && angle<(3*PI/4)) return LEFT;
	if( angle>(3*PI/4) || angle<-(3*PI/4)) return TURN_BACK;
	if( angle< (-PI/4) && angle > (-3*PI/4)) return RIGHT;
	if( angle<PI/4 && angle>-(PI/4)) return STRAIGHT;
	else return STOP;
}

// When epuck approch a corner he avance to it with continuous comande because the camera see too far away.
void corner_approch(void){

	int16_t steps_traveled = 0;
	left_motor_set_pos(0);
	right_motor_set_pos(0);

	while(steps_traveled<NB_STEPS_TO_DO){
		right_motor_set_speed(SYS_SPEED);
		left_motor_set_speed(SYS_SPEED);
		steps_traveled = (left_motor_get_pos()+right_motor_get_pos())/2;
	}
	right_motor_set_speed(0);
	left_motor_set_speed(0);
}

void motor_turn(int16_t speed_left, int16_t speed_right, int16_t nb_steps_to_do){

	int16_t steps_traveled_left = 0;
	int16_t steps_traveled_right = 0;
	left_motor_set_pos(0);
	right_motor_set_pos(0);

	while((steps_traveled_left < nb_steps_to_do) && (steps_traveled_right < nb_steps_to_do)){

		right_motor_set_speed(speed_right);
		left_motor_set_speed(speed_left);

		steps_traveled_left = left_motor_get_pos();
		steps_traveled_right = right_motor_get_pos();
	}
	right_motor_set_speed(0);
	left_motor_set_speed(0);
}


void corner_turn(int8_t side){
	int16_t nb_steps_to_do = 0;
	switch (side)
	{
	case RIGHT:
		nb_steps_to_do = NB_STEPS_TO_TURN;
		motor_turn(SYS_SPEED, 50, nb_steps_to_do);
		break;
	case LEFT:
		nb_steps_to_do = NB_STEPS_TO_TURN;
		motor_turn(50, SYS_SPEED, nb_steps_to_do);
		break;
	case STRAIGHT:
		nb_steps_to_do = NB_STEPS_TO_GO_STRAIGHT;
		motor_turn(SYS_SPEED, -SYS_SPEED, nb_steps_to_do);
		break;
	case TURN_BACK:
		nb_steps_to_do = NB_STEPS_TO_TURN_BACK;
		motor_turn(SYS_SPEED, 0, nb_steps_to_do);
		break;
	case STOP:
		nb_steps_to_do = 0;
		motor_turn(0, 0, nb_steps_to_do);
		break;

	}
}

// PI regulator implementation for the distance collision sensor
int16_t pi_collision_regulator(int16_t distance){

	float speed = 0, error = 0;

	static float sum_error = 0;

	error = (distance) - MIN_COLLISION_DIST;		//possible car distance jamais < que 10

	//due to the noisy camera we set a minimal error for movement
	if(fabs(error) < ERROR_THRESHOLD_PID){
			return 0;
	}
	if(fabs(error) > MAX_DETECTION_DIST){
			return 0;
	}

	sum_error += error;

	//we set a maximum and a minimum for the sum to avoid an uncontrolled growth
	if(sum_error > MAX_SUM_ERROR){
		sum_error = MAX_SUM_ERROR;
	}else if(sum_error < -MAX_SUM_ERROR){
		sum_error = -MAX_SUM_ERROR;
	}

	speed = KP_PID * (error) + KI * (sum_error) ;

	if(speed < 0) speed *= COEFF_REVERSE;
	if(speed > SYS_SPEED) speed = SYS_SPEED;

	return speed;
}



// PD regulator for the rotational line follower
int16_t pd_regulator(float error){

	float rot_speed = 0;
	static float old_error;

	//due to the noisy camera we set a minimal error for movement
	if(fabs(error) < ERROR_THRESHOLD){
		return 0;
	}

	rot_speed = KP_PD * error + KD * ((error - old_error)/D_T);

	old_error = error;
	return (int16_t)rot_speed;
}

static THD_WORKING_AREA(waPidRegulator, 256);
static THD_FUNCTION(PidRegulator, arg) {

	chRegSetThreadName(__FUNCTION__);
	(void)arg;

	systime_t time;
	int16_t rotation_speed = 0, dist_colision = 0;


	while(1){
		time = chVTGetSystemTime();

		// CALCUL DES VITESSES

		//computes the speed to give to the motors
		//distance_cm is modified by the image processing thread
		rotation_speed = pd_regulator(get_error_line());

		// Récupère la distance avec l'obstacle.
		dist_colision = VL53L0X_get_dist_mm() - TOF_OFFSET;

		// CONDITIONS DE MVNT

		//Vérifie si il y a un corner
		if(get_line_width() > LINE_WIDTH_TRESH ){
			corner_approch();
			while (get_amp() < DETECTION_AMP) corner_turn(STOP);
			if (turn_choice() == LEFT) corner_turn(LEFT);
			if (turn_choice() == RIGHT) corner_turn(RIGHT);
			if (turn_choice() == TURN_BACK) corner_turn(TURN_BACK);
			if (turn_choice() == STRAIGHT) corner_turn(STRAIGHT);


			//if (get_angle()>0 && (get_amp() > DETECTION_AMP)) corner_turn(LEFT);
			//if (get_angle()<0 && (get_amp() > DETECTION_AMP)) corner_turn(RIGHT);
		}
		else{
			//applies the speed from the PI regulator and the correction for the rotation
			if(dist_colision < MAX_DETECTION_DIST){
				right_motor_set_speed(pi_collision_regulator(dist_colision));
				left_motor_set_speed(pi_collision_regulator(dist_colision));
			}
			else{
				right_motor_set_speed(SYS_SPEED - rotation_speed);
				left_motor_set_speed(SYS_SPEED + rotation_speed);
			}
		}
		//100Hz
		chThdSleepUntilWindowed(time, time + MS2ST(10));
	}
}


void pid_regulator_start(void){
	chThdCreateStatic(waPidRegulator, sizeof(waPidRegulator), NORMALPRIO, PidRegulator, NULL);
}

