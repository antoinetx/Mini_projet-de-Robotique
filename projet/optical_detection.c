#include "ch.h"
#include "hal.h"
#include <main.h>
#include <usbcfg.h>
#include <chprintf.h>
#include <optical_detection.h>

#include <camera/po8030.h>
#include "memory_protection.h"

//semaphore
static BSEMAPHORE_DECL(image_ready_sem, TRUE);

static uint16_t begin = 0, end = 0;
static int16_t last_err_pos = 0, line_found = 0;


/*
* Returns the error between the center of the line and the current position, extracted from the image buffer given
* Returns 0 if line not found
*/
int16_t extract_error_line_position(uint8_t *buffer){

	uint16_t i = 0;
	uint8_t stop = 0, wrong_line = 0;
	int16_t new_err_pos = 0;
	uint32_t mean = 0;
	uint8_t line_not_found = 0;

	//performs an average
	for(uint16_t j = 0 ; j < IMAGE_BUFFER_SIZE ; j++){
		mean += buffer[j];
	}
	mean /= IMAGE_BUFFER_SIZE;

	do{
		wrong_line = 0;

		//search for a begin
		while(stop == 0 && i < (IMAGE_BUFFER_SIZE - WIDTH_SLOPE))
		{
			//the slope must at least be WIDTH_SLOPE wide and is compared
			//to the mean of the image
			if(buffer[i] > mean && buffer[i+WIDTH_SLOPE] < mean)
			{
				begin = i;
				stop = 1;
			}
			i++;
		}
		//if a begin was found, search for an end
		if (i < (IMAGE_BUFFER_SIZE - WIDTH_SLOPE) && begin)
		{
			stop = 0;

			while(stop == 0 && i < IMAGE_BUFFER_SIZE)
			{
				if(buffer[i] > mean && buffer[i-WIDTH_SLOPE] < mean)
				{
					end = i;
					stop = 1;
				}
				i++;
			}
			//if an end was not found
			if (i > IMAGE_BUFFER_SIZE || !end)
			{
				line_not_found = 1;
			}
		}
		else//if no begin was found
		{
			line_not_found = 1;
		}

		//if a line too small has been detected, continues the search
		if(!line_not_found && (end-begin) < MIN_LINE_WIDTH){

			i = end;
			begin = 0;
			end = 0;
			stop = 0;
			wrong_line = 1;
		}
	}while(wrong_line);

	//return the error of the position, or return the last error if no line was found
	if(line_not_found){
		begin = 0;
		end = 0;
		new_err_pos = last_err_pos;
	}else{
		new_err_pos = last_err_pos = (begin + end)/2 - IMAGE_BUFER_MIDDLE; // gives the error from the center of the picture
	}
	line_found = !line_not_found;
	return new_err_pos;
}


//search for a line in the image and gets its width in pixels
static THD_WORKING_AREA(waCaptureImage, 256);
static THD_FUNCTION(CaptureImage, arg) {

    chRegSetThreadName(__FUNCTION__);
    (void)arg;

	//Takes pixels 0 to IMAGE_BUFFER_SIZE of the line 10 + 11 (minimum 2 lines because reasons)
	po8030_advanced_config(FORMAT_RGB565, 0, 475, IMAGE_BUFFER_SIZE, 2, SUBSAMPLING_X1, SUBSAMPLING_X1);

	dcmi_enable_double_buffering();
	dcmi_set_capture_mode(CAPTURE_ONE_SHOT);
	dcmi_prepare();

    while(1){
        //starts a capture
		dcmi_capture_start();
		//waits for the capture to be done
		wait_image_ready();
		//signals an image has been captured
		chBSemSignal(&image_ready_sem);

    }
}


static THD_WORKING_AREA(waProcessImage, 1024);
static THD_FUNCTION(ProcessImage, arg) {

    chRegSetThreadName(__FUNCTION__);
    (void)arg;

	uint8_t *img_buff_ptr;
	uint8_t image[IMAGE_BUFFER_SIZE] = {0};

    while(1){
    	//waits until an image has been captured
        chBSemWait(&image_ready_sem);

		//gets the pointer to the array filled with the last image in RGB565
		img_buff_ptr = dcmi_get_last_image_ptr();

		//Extracts only the red pixels
		for(uint16_t i = 0 ; i < (2 * IMAGE_BUFFER_SIZE) ; i+=2){

		//takes nothing from the second byte
		image[i/2] = (uint8_t)img_buff_ptr[i]&0xF8;
		}

	    last_err_pos = extract_error_line_position(image);

    }
}


void process_image_start(void){
	chThdCreateStatic(waProcessImage, sizeof(waProcessImage), NORMALPRIO, ProcessImage, NULL);
	chThdCreateStatic(waCaptureImage, sizeof(waCaptureImage), NORMALPRIO, CaptureImage, NULL);
}


//Allow other folder to have access to those informations
int16_t get_error_line(void){
	return last_err_pos;
}
int16_t get_begin(void){
	return begin;
}
int16_t get_end(void){
	return end;
}
int16_t get_line_width(void){
	return (end - begin);
}
int16_t get_line_found(void){
	return (line_found);
}
