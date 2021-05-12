#ifndef MINI_PROJET_ROBOTIQUE_PROJET_PI_REGULATOR_H_
#define MINI_PROJET_ROBOTIQUE_PROJET_PI_REGULATOR_H_

#define PI 							3.14f
#define SYS_SPEED 					400
#define ERROR_THRESHOLD				5   //[cm] because of the noise of the camera
#define	MAX_SUM_ERROR  				(SYS_SPEED/KI)
#define KP_PD						2
#define KD							2
#define D_T							10   // dt of derivative term. correspond to PI clock
#define KP_PI						20
#define KI							0.002
#define ERROR_THRESHOLD_PI			5  // Threshold error in mm for the TOF sensor

#define NB_STEPS_TO_DO				925  //(CORNER_DISTANCE(6cm)/WHEEL_PERIMETER(13cm))*NB_MOTOR_STEP(1000)
#define NB_STEPS_TO_TURN  			325
#define NB_STEPS_TO_TURN_BACK 		660
#define NB_STEPS_TO_GO_STRAIGHT		300
#define LINE_WIDTH_TRESH 			300	//Line width at witch epuck see a corner
#define MIN_COLLISION_DIST 			50	// Min distance collision for the TOF
#define MAX_DETECTION_DIST 			150
#define COEFF_REVERSE 				2
#define NB_SAMPLES 					10	// Nb of sample we take to be sure the is a line
#define TOF_OFFSET					40	// TOF have an offset of 40 mm
#define DETECTION_AMP 				3000	 // Sound amp at witch epuck move


// Different states of epuck
enum side {RIGHT, LEFT, STRAIGHT, TURN_BACK, ARRIVED, OBSTACLE, STOP};

//start the PI regulator thread
void mouvement_start(void);

int8_t get_state(void);

#endif /* MINI_PROJET_ROBOTIQUE_PROJET_PI_REGULATO_H_ */

