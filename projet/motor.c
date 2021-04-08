#include "ch.h"
#include "hal.h"
#include <main.h>
#include <usbcfg.h>
#include <chprintf.h>
#include <motors.h>


void go_forward(void){
		left_motor_set_speed(600);
		right_motor_set_speed(600);
}

void turn(void){
		left_motor_set_speed(-600);
		right_motor_set_speed(600);
}

void go_backward(void){
		left_motor_set_speed(-600);
		right_motor_set_speed(-600);
}

