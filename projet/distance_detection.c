#include "ch.h"
#include "hal.h"
#include <math.h>
#include <main.h>
#include <usbcfg.h>
#include <chprintf.h>
#include <distance_detection.h>
#include <sensors/VL53L0X/VL53L0X.h>


float distance = 0;

static THD_WORKING_AREA(waCapteurToF, 1024);
static THD_FUNCTION(CapteurToF, arg) {

    chRegSetThreadName(__FUNCTION__);
    (void)arg;

    distance = (float)VL53L0X_get_dist_mm();

}



float get_distance(void){
	return distance;
}
