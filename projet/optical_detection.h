/*
 * optical_detection.h
 *
 *  Created on: 15 avr. 2021
 *      Author: laral
 */

#ifndef MINI_PROJET_ROBOTIQUE_PROJET_OPTICAL_DETECTION_H_
#define MINI_PROJET_ROBOTIQUE_PROJET_OPTICAL_DETECTION_H_


//constants for the differents parts of the project
#define IMAGE_BUFFER_SIZE		640
#define IMAGE_BUFER_MIDDLE		320
// les prochains en dessous sont juste mis au pif, a compléter avec des vrai valeurs
#define GOAL_DISTANCE			10
#define ROTATION_THRESHOLD		10
#define ROTATION_COEFF			10
#define KP						800
#define KI						3.5f
#define KD						50
#define MIN_LINE_WIDTH			40
#define MAX_DISTANCE			180
#define WIDTH_SLOPE				5
#define PXTOCM					1
#define ERROR_THRESHOLD			0.8f   //[cm] because of the noise of the camera
#define MAX_SUM_ERROR 		    (400/KI)
#define PI_CLOCK				10     // in ms



void process_image_start(void);
//void suivre_ligne(void);


#endif /* MINI_PROJET_ROBOTIQUE_PROJET_OPTICAL_DETECTION_H_ */
