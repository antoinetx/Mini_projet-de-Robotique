#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "ch.h"
#include "hal.h"
#include "memory_protection.h"
#include <main.h>


#include <usbcfg.h>
#include <chprintf.h>
#include <audio/microphone.h>

#include <audio_processing.h>
#include <fft.h>
#include <communications.h>
#include <arm_math.h>


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

static void timer12_start(void){
    //General Purpose Timer configuration
    //timer 12 is a 16 bit timer so we can measure time
    //to about 65ms with a 1Mhz counter
    static const GPTConfig gpt12cfg = {
        1000000,        /* 1MHz timer clock in order to measure uS.*/
        NULL,           /* Timer callback.*/
        0,
        0
    };

    gptStart(&GPTD12, &gpt12cfg);
    //let the timer count to max value
    gptStartContinuous(&GPTD12, 0xFFFF);
}

int main(void)
{

	 halInit();
	 chSysInit();
	 mpu_init();

	 //starts the serial communication / the USB communication / timer 12 / inits the motors
	 serial_start();
	 usb_start();
	 timer12_start();
	 motors_init();


	 //starts the microphones processing thread.
	 //it calls the callback given in parameter when samples are ready

	 mic_start(&processAudioData);

    /* Infinite loop. */
    while (1) {
    	//waits 0.1 second
        chThdSleepMilliseconds(1000);

        //waits until a result must be sent to the computer
        //wait_triangulation_data();

        chprintf((BaseSequentialStream *)&SDU1, " \n angle : %f" , get_angle());
        chprintf((BaseSequentialStream *)&SDU1, " \n fréquence : %f" , get_freq());
    }
}

#define STACK_CHK_GUARD 0xe2dee396
uintptr_t __stack_chk_guard = STACK_CHK_GUARD;

void __stack_chk_fail(void)
{
    chSysHalt("Stack smashing detected");
}
