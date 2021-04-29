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

// les prochains en dessous sont juste mis au pif, a compl�ter avec des vrai valeurs
#define GOAL_DISTANCE			10
#define MIN_LINE_WIDTH			40
#define MAX_DISTANCE			180
#define WIDTH_SLOPE				5
#define PXTOCM					1
#define PI_CLOCK				10     // in ms



// return the begining and end position of the black line in pixels

int16_t get_begin(void);
int16_t get_end(void);
int16_t get_error_line(void);

void process_image_start(void);
//void suivre_ligne(void);


#endif /* MINI_PROJET_ROBOTIQUE_PROJET_OPTICAL_DETECTION_H_ */
