#include "ch.h"
#include "hal.h"
#include <math.h>
#include <main.h>
#include <usbcfg.h>
#include <chprintf.h>
#include <leds.h>

#include <animation.h>







// led quand on va tt droit :



int led_tt_droit(void){

	clear_leads();
	while (1){
		toggle_rgb_led(LED6, GREEN_LED, INTENSITY);
		toggle_rgb_led(LED4, GREEN_LED, INTENSITY);
		//waits 0.25 second
		chThdSleepMilliseconds(250);
		clear_leads();
		toggle_rgb_led(LED3, GREEN_LED, INTENSITY);
		toggle_rgb_led(LED7, GREEN_LED, INTENSITY);
		//waits 0.25 second
		chThdSleepMilliseconds(250);
		clear_leads();
		toggle_rgb_led(LED2, GREEN_LED, INTENSITY);
		toggle_rgb_led(LED8, GREEN_LED, INTENSITY);
		//waits 0.25 second
		chThdSleepMilliseconds(250);
		clear_leads();
	}

}

int led_tourner_droite(void){

	while (1){
		//waits 0.5 second
		chThdSleepMilliseconds(500);
		toggle_rgb_led(LED2, RED_LED, INTENSITY);
		toggle_rgb_led(LED3, RED_LED, INTENSITY);
		toggle_rgb_led(LED4, RED_LED, INTENSITY);
		//waits 0.5 second
		chThdSleepMilliseconds(500);
		clear_leads();
	}
}

int led_tourner_gauche(void){

	while (1){
		clear_leads();
		//waits 0.5 second
		chThdSleepMilliseconds(500);
		toggle_rgb_led(LED6, RED_LED, INTENSITY);
		toggle_rgb_led(LED7, RED_LED, INTENSITY);
		toggle_rgb_led(LED8, RED_LED, INTENSITY);
		//waits 0.5 second
		chThdSleepMilliseconds(500);
	}

}

