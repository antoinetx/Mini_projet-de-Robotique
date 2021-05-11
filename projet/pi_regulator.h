/*
 * pi_regulator.h
 *
 *  Created on: 20 avr. 2021
 *      Author: laral
 */

#ifndef MINI_PROJET_ROBOTIQUE_PROJET_PI_REGULATOR_H_
#define MINI_PROJET_ROBOTIQUE_PROJET_PI_REGULATOR_H_

#define PI 3.14
#define SYS_SPEED 400
#define ERROR_THRESHOLD			5   //[cm] because of the noise of the camera
#define	MAX_SUM_ERROR  (SYS_SPEED/KI)	//(SYS_SPEED/KI)
#define KP_PD						2
#define KD						2
#define D_T				10   // dt of derivativ term. correspond to PI clock

#define KP_PID						20     //		11  -->  0.5
#define KI						0.002		//	0.003f	-->  0.0018
#define ERROR_THRESHOLD_PID			5

//#define CORNER_DIST  6 	// 6cm = distance between what epuck see and epuck
//#define NB_MOTOR_STEP	1000 //Nb step for 1 trun of the motor
//#define WHEEL_PERIMETER	 13
#define NB_STEPS_TO_DO      925  //(CORNER_DIST/WHEEL_PERIMETER)*NB_MOTOR_STEP
#define NB_STEPS_TO_TURN  325
#define NB_STEPS_TO_TURN_BACK 660
#define NB_STEPS_TO_GO_STRAIGHT 300
#define LINE_WIDTH_TRESH 300
#define MIN_COLLISION_DIST 	50
#define MAX_DETECTION_DIST 	150
#define COEFF_REVERSE 2
#define NB_SAMPLES 10


#define TOF_OFFSET	40

#define DETECTION_AMP 3000

enum side {RIGHT, LEFT, STRAIGHT, TURN_BACK, ARRIVED, OBSTACLE, STOP};


//start the PI regulator thread
void mouvment_start(void);

int8_t get_state(void);


#endif /* MINI_PROJET_ROBOTIQUE_PROJET_PI_REGULATO_H_ */

//3.5f
//#define KD						0.01f
