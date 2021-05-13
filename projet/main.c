// Standard includes
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "ch.h"
#include "hal.h"
#include <main.h>

#include <usbcfg.h>
#include <camera/po8030.h>
#include <motors.h>
#include <leds.h>
#include <audio/microphone.h>
#include <sensors/VL53L0X/VL53L0X.h>
#include <spi_comm.h>

#include "audio/audio_thread.h"
#include "audio/play_melody.h"
#include "audio/play_sound_file.h"
#include "memory_protection.h"

#include <pi_regulator.h>
#include <optical_detection.h>
#include <audio_processing.h>
#include <fft.h>
#include <animation.h>

#include <arm_math.h>
#include <chprintf.h>

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

	 //Sensor initialization
     dcmi_start();
     po8030_start();
     serial_start();
     timer12_start();
	 usb_start();
	 motors_init();
	 VL53L0X_start();
	 spi_comm_start();

	 // Threads start
	 mic_start(&processAudioData);
	 playMelodyStart();
	 dac_start();
	 led_animation_start();
	 sound_animation_start();
     process_image_start();
     mouvement_start();

    while (1) {
    	//waits 1 second
        chThdSleepMilliseconds(1000);
    }

}


#define STACK_CHK_GUARD 0xe2dee396
uintptr_t __stack_chk_guard = STACK_CHK_GUARD;

void __stack_chk_fail(void)
{
    chSysHalt("Stack smashing detected");
}
