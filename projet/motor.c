#include "ch.h"
#include "hal.h"
#include <main.h>
#include <usbcfg.h>
#include <chprintf.h>
#include <motors.h>


void go_forward(int speed){
		left_motor_set_speed(speed);
		right_motor_set_speed(speed);
}

void turn(int speed_right, int speed_left){
		left_motor_set_speed(speed_left);
		right_motor_set_speed(speed_right);
}

void go_backward(int speed){
		left_motor_set_speed(-speed);
		right_motor_set_speed(-speed);
}

