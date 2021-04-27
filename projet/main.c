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
#include "memory_protection.h"
#include <main.h>
#include <audio/microphone.h>

#include <audio_processing.h>
#include <fft.h>
#include <communications.h>
#include <arm_math.h>

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


	 //starts the serial communication
	 serial_start();
	 //start the USB communication
	 usb_start();
	 timer12_start();
	 motors_init();

    //starts the camera
    dcmi_start();
    po8030_start();

	//starts ToF sensor
	VL53L0X_start();


   //stars the threads for the pi regulator and the processing of the image

   process_image_start();


	 //starts the microphones processing thread.
	 //it calls the callback given in parameter when samples are ready
	 mic_start(&processAudioData);

    /* Infinite loop. */
    while (1) {
    	//waits 0.1 second
        chThdSleepMilliseconds(1000);
        chprintf((BaseSequentialStream *)&SDU1, " \n angle : %f" , get_angle());
    }

}



#define STACK_CHK_GUARD 0xe2dee396
uintptr_t __stack_chk_guard = STACK_CHK_GUARD;

void __stack_chk_fail(void)
{
    chSysHalt("Stack smashing detected");
}
