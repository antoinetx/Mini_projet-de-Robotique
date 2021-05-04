/*
 * pi_regulato.h
 *
 *  Created on: 20 avr. 2021
 *      Author: laral
 */

#ifndef MINI_PROJET_ROBOTIQUE_PROJET_PI_REGULATO_H_
#define MINI_PROJET_ROBOTIQUE_PROJET_PI_REGULATO_H_

#define SYS_SPEED 200
#define ERROR_THRESHOLD			5   //[cm] because of the noise of the camera
#define	MAX_SUM_ERROR  (SYS_SPEED/KI)
#define KP						2
#define KI						0.1f
#define KD						2
#define D_T				10   // dt of derivativ term. correspond to PI clock
//#define CORNER_DIST  6 	// 6cm = distance between what epuck see and epuck
//#define NB_MOTOR_STEP	1000 //Nb step for 1 trun of the motor
//#define WHEEL_PERIMETER	 13
#define NB_STEPS_TO_DO      461  //(CORNER_DIST/WHEEL_PERIMETER)*NB_MOTOR_STEP
#define NB_STEPS_TO_TURN  730
#define LINE_WIDTH_TRESH 240

enum side {RIGHT, LEFT};


//start the PI regulator thread
void pid_regulator_start(void);

//test
void corner_approch(void);

#endif /* MINI_PROJET_ROBOTIQUE_PROJET_PI_REGULATO_H_ */

//3.5f
//#define KD						0.01f
