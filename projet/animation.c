#include "ch.h"
#include "hal.h"
#include <math.h>
#include <main.h>
#include <usbcfg.h>
#include <chprintf.h>
#include <leds.h>
#include <animation.h>

#include "audio/audio_thread.h"
#include "audio/play_melody.h"
#include "audio/play_sound_file.h"
#include "audio/microphone.h"



static int8_t i = 0;

// a enlever d ici apres
static uint8_t states = 5;


static THD_WORKING_AREA(waLedAnimation, 1024);
static THD_FUNCTION(LedAnimation, arg) {

	chRegSetThreadName(__FUNCTION__);
	(void)arg;

	//ajouter la ligne en fonction du nom de la fonction ici :
	// uint8_t states = get_states();

	while (1){
		switch (states)
		{
			// activate the right turn signal
			case RIGHT:
				if (i == ETEIND){
					set_rgb_led(LED2,INTENSITY_MOY,INTENSITY,0);
					set_rgb_led(LED4,INTENSITY_MOY,INTENSITY,0);
					set_led(LED3,ON);
					i = ETEIND_BIS;
				}
				else if (i == ETEIND_BIS){
					i = ALLUMEE;
				}
				else if (i == ALLUMEE){
					clear_leds();
					i = ALLUMEE_BIS;
				}
				else {
					clear_leds();
					i = ETEIND;
				}
			break;
			// activate the left turn signal
			case LEFT:
				if (i == ETEIND){
					set_rgb_led(LED6,INTENSITY_MOY,INTENSITY,0);
					set_rgb_led(LED8,INTENSITY_MOY,INTENSITY,0);
					set_led(LED7,ON);
					i = ETEIND_BIS;
				}
				else if (i == ETEIND_BIS){
					i = ALLUMEE;
				}
				else if (i == ALLUMEE){
					clear_leds();
					i = ALLUMEE_BIS;
				}
				else {
					clear_leds();
					i = ETEIND;
				}
			break;
			// activate the high beams
			case STRAIGHT:
				if (i == ETEIND){
					clear_leds();
					set_rgb_led(LED4,INTENSITY,0,INTENSITY);
					set_rgb_led(LED6,INTENSITY,0,INTENSITY);
					i = ETEIND_BIS;
				}
				else if (i == ETEIND_BIS){
					clear_leds();
					set_led(LED3,ON);
					set_led(LED7,ON);
					i = ALLUMEE;
				}
				else if (i == ALLUMEE){
					clear_leds();
					set_rgb_led(LED2,INTENSITY,0,INTENSITY);
					set_rgb_led(LED8,INTENSITY,0,INTENSITY);
					i = ALLUMEE_BIS;
				}
				else {
					clear_leds();
					i = ETEIND;
				}
			break;
			// activate the turn back signals
			case TURN_BACK:
				if (i == ETEIND){
					clear_leds();
					set_rgb_led(LED2,INTENSITY,0,INTENSITY);
					set_rgb_led(LED6,INTENSITY,0,INTENSITY);
					i = ETEIND_BIS;
				}
				else if (i == ETEIND_BIS){
					clear_leds();
					set_led(LED3,ON);
					set_led(LED7,ON);
					i = ALLUMEE;
				}
				else if (i == ALLUMEE){
					clear_leds();
					set_rgb_led(LED8,INTENSITY,0,INTENSITY);
					set_rgb_led(LED4,INTENSITY,0,INTENSITY);
					i = ALLUMEE_BIS;
				}
				else {
					clear_leds();
					set_led(LED1,ON);
					set_led(LED5,ON);
					i = ETEIND;
				}
			break;
			// activate the arrival lights
			case STOP:
				if (i == ETEIND){
					clear_leds();
					set_rgb_led(LED2,INTENSITY_MOY,0,INTENSITY);
					set_rgb_led(LED6,INTENSITY_MOY,0,INTENSITY);
					set_rgb_led(LED8,INTENSITY_MOY,0,INTENSITY);
					set_rgb_led(LED4,INTENSITY_MOY,0,INTENSITY);
					i = ALLUMEE;
				}
				else {
					clear_leds();
					set_led(LED3,ON);
					set_led(LED7,ON);
					set_led(LED1,ON);
					set_led(LED5,ON);
					i = ETEIND;
				}
			break;
			//activate the hazard warning lights
			case OBSTACLE:
				if (i == ETEIND){
					set_rgb_led(LED2,INTENSITY,0,INTENSITY);
					set_rgb_led(LED4,INTENSITY,0,INTENSITY);
					set_rgb_led(LED6,INTENSITY,0,INTENSITY);
					set_rgb_led(LED8,INTENSITY,0,INTENSITY);
					set_led(LED3,ON);
					set_led(LED7,ON);
					i = ETEIND_BIS;
				}
				else if (i == ETEIND_BIS){
					i = ALLUMEE;
				}
				else if (i == ALLUMEE){
					clear_leds();
					i = ALLUMEE_BIS;
				}
				else {
					clear_leds();
					i = ETEIND;
				}
			break;
		}




		//chprintf((BaseSequentialStream *)&SDU1, " \n AVANCE");

		//do some stuff and sleep for 250ms
		chThdSleepMilliseconds(250);

	}




}


static THD_WORKING_AREA(waSoundAnimation, 1024);
static THD_FUNCTION(SoundAnimation, arg) {

	chRegSetThreadName(__FUNCTION__);
	(void)arg;

	//ajouter la ligne en fonction du nom de la fonction ici :
	// uint8_t states = get_states();

	while (1){
		switch (states)
		{
			// activate the straight way music
			case STRAIGHT:
				playMelody(SANDSTORMS,ML_FORCE_CHANGE,NULL);
			break;
			// activate the turn back music
			case TURN_BACK:
				playMelody(STARWARS,ML_FORCE_CHANGE,NULL);
			break;
			// activate the arrival music
			case STOP:
				playMelody(MARIO_FLAG,ML_FORCE_CHANGE,NULL);
			break;
			// activate the obstacle way music
			case OBSTACLE:
				playMelody(SEVEN_NATION_ARMY,ML_FORCE_CHANGE,NULL);
			break;
		}



		//do some stuff and sleep for 4s
		chThdSleepMilliseconds(4000);

	}
}





void led_animation_start(void){
	chThdCreateStatic(waLedAnimation, sizeof(waLedAnimation), NORMALPRIO, LedAnimation, NULL);
}

void sound_animation_start(void){
	chThdCreateStatic(waSoundAnimation, sizeof(waSoundAnimation), NORMALPRIO, SoundAnimation, NULL);
}
