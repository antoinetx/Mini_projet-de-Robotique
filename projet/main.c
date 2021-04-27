#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <motors.h>

#include "ch.h"
#include "hal.h"
#include <main.h>
#include <usbcfg.h>
#include <chprintf.h>

#include <camera/po8030.h>


#include "ch.h"
#include "hal.h"
#include "memory_protection.h"


#include <main.h>

#include <camera/po8030.h>

void SendUint8ToComputer(uint8_t* data, uint16_t size)
{
	chSequentialStreamWrite((BaseSequentialStream *)&SD3, (uint8_t*)"START", 5);
	chSequentialStreamWrite((BaseSequentialStream *)&SD3, (uint8_t*)&size, sizeof(uint16_t));
	chSequentialStreamWrite((BaseSequentialStream *)&SD3, (uint8_t*)data, size);
}


static void serial_start(void)
{
	static SerialConfig ser_cfg = {
	    115200,
	    0,
	    0,
	    0,
	};

	sdStart(&SD3, &ser_cfg); // UART3.
}


int main(void)
{

    halInit();
    chSysInit();
    mpu_init();

    //starts the camera
    dcmi_start();
    po8030_start();


    //starts the serial communication
    serial_start();
    //start the USB communication
    usb_start();

   //inits the motors
   motors_init();
	//starts ToF sensor
	VL53L0X_start();


   //stars the threads for the pi regulator and the processing of the image

   process_image_start();


    /* Infinite loop. */
    while (1) {
    	//waits 1 second
        chThdSleepMilliseconds(1000);
        chprintf((BaseSequentialStream *)&SDU1, " \n erreur ligne : %d" , get_extract_error_line_position());
        chprintf((BaseSequentialStream *)&SDU1, " \n begin ligne : %d" , get_begin());
        chprintf((BaseSequentialStream *)&SDU1, " \n end ligne : %d" , get_end());

    }

}



#define STACK_CHK_GUARD 0xe2dee396
uintptr_t __stack_chk_guard = STACK_CHK_GUARD;

void __stack_chk_fail(void)
{
    chSysHalt("Stack smashing detected");
}
