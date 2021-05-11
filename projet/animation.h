/*
 * animation.h
 *
 *  Created on: 5 mai 2021
 *      Author: laral
 */

#ifndef MINI_PROJET_ROBOTIQUE_PROJET_ANIMATION_H_
#define MINI_PROJET_ROBOTIQUE_PROJET_ANIMATION_H_



#define INTENSITY			255
#define INTENSITY_MOY		125
#define ON					1
#define OFF					0



enum etat {ETEIND, ETEIND_BIS, ALLUMEE, ALLUMEE_BIS};




void led_animation_start(void);

void sound_animation_start(void);

#endif /* MINI_PROJET_ROBOTIQUE_PROJET_ANIMATION_H_ */
