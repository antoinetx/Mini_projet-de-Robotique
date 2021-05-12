#include "ch.h"
#include "hal.h"
#include <math.h>
#include <usbcfg.h>
#include <chprintf.h>
#include <leds.h>
#include <animation.h>

#include "audio/audio_thread.h"
#include "audio/play_melody.h"
#include "audio/play_sound_file.h"
#include "audio/microphone.h"
#include <pi_regulator.h>


static int8_t i = 0;


static THD_WORKING_AREA(waLedAnimation, 1024);
static THD_FUNCTION(LedAnimation, arg) {

	chRegSetThreadName(__FUNCTION__);
	(void)arg;

	uint8_t states = get_state();

	while (1){
		states = get_state();
		switch (states)
		{
			// activate the right turn signal : the tree left leds
			case RIGHT:
				if (i == ETEIND){
					set_rgb_led(LED2,INTENSITY_MOY,INTENSITY,INTENSITY_NUL);
					set_rgb_led(LED4,INTENSITY_MOY,INTENSITY,INTENSITY_NUL);
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
			// activate the left turn signal : the tree right leds
			case LEFT:
				if (i == ETEIND){
					set_rgb_led(LED6,INTENSITY_MOY,INTENSITY,INTENSITY_NUL);
					set_rgb_led(LED8,INTENSITY_MOY,INTENSITY,INTENSITY_NUL);
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
			// activate the high beams : activate the leds from the back to the front
			case STRAIGHT:
				if (i == ETEIND){
					clear_leds();
					set_rgb_led(LED4,INTENSITY,INTENSITY_NUL,INTENSITY);
					set_rgb_led(LED6,INTENSITY,INTENSITY_NUL,INTENSITY);
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
			// activate the turn back signals : the leds make a round
			case TURN_BACK:
				if (i == ETEIND){
					clear_leds();
					set_rgb_led(LED2,INTENSITY,INTENSITY_NUL,INTENSITY);
					set_rgb_led(LED6,INTENSITY,INTENSITY_NUL,INTENSITY);
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
					set_rgb_led(LED8,INTENSITY,INTENSITY_NUL,INTENSITY);
					set_rgb_led(LED4,INTENSITY,INTENSITY_NUL,INTENSITY);
					i = ALLUMEE_BIS;
				}
				else {
					clear_leds();
					set_led(LED1,ON);
					set_led(LED5,ON);
					i = ETEIND;
				}
			break;
			// activate the arrival lights : sparkeling of the leds
			case ARRIVED:
				if (i == ETEIND){
					clear_leds();
					set_rgb_led(LED2,INTENSITY_MOY,INTENSITY_NUL,INTENSITY);
					set_rgb_led(LED6,INTENSITY_MOY,INTENSITY_NUL,INTENSITY);
					set_rgb_led(LED8,INTENSITY_MOY,INTENSITY_NUL,INTENSITY);
					set_rgb_led(LED4,INTENSITY_MOY,INTENSITY_NUL,INTENSITY);
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
			//activate the hazard warning lights : blinking of the front and back leds
			case OBSTACLE:
				if (i == ETEIND){
					set_rgb_led(LED2,INTENSITY_MOY,INTENSITY,INTENSITY_NUL);
					set_rgb_led(LED4,INTENSITY_MOY,INTENSITY,INTENSITY_NUL);
					set_rgb_led(LED6,INTENSITY_MOY,INTENSITY,INTENSITY_NUL);
					set_rgb_led(LED8,INTENSITY_MOY,INTENSITY,INTENSITY_NUL);
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

		//leave the actual led state for 250ms
		chThdSleepMilliseconds(250);

	}
}


static THD_WORKING_AREA(waSoundAnimation, 1024);
static THD_FUNCTION(SoundAnimation, arg) {

	chRegSetThreadName(__FUNCTION__);
	(void)arg;

	uint8_t states = get_state();


	while (1){
		switch (states)
		{
			// activate the arrival music
			case ARRIVED:
				//playMelody(MARIO_FLAG,ML_FORCE_CHANGE,NULL);
			break;
			// activate the obstacle way music
			case OBSTACLE:
				//playMelody(MARIO_START,ML_FORCE_CHANGE,NULL);
			break;
		}

		//run the music for 2s
		chThdSleepMilliseconds(2000);

	}
}




void led_animation_start(void){
	chThdCreateStatic(waLedAnimation, sizeof(waLedAnimation), NORMALPRIO, LedAnimation, NULL);
}

void sound_animation_start(void){
	chThdCreateStatic(waSoundAnimation, sizeof(waSoundAnimation), NORMALPRIO, SoundAnimation, NULL);
}
