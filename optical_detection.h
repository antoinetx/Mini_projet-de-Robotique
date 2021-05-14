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
#define GOAL_DISTANCE			10
#define MIN_LINE_WIDTH			40
#define MAX_DISTANCE			180
#define WIDTH_SLOPE				5
#define PXTOCM					1
#define PI_CLOCK				10     // in ms
#define BUTTOM_PICTURE			475

int16_t get_begin(void);
int16_t get_end(void);
int16_t get_error_line(void);
int16_t get_line_width(void);
int16_t get_line_found(void);

void process_image_start(void);

#endif /* MINI_PROJET_ROBOTIQUE_PROJET_OPTICAL_DETECTION_H_ */
