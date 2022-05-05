#include <ch.h>
#include <hal.h>
#include <stdlib.h>
#include <math.h>
#include <chprintf.h>
#include <leds.h>
#include <sensors/proximity.h>
#include <motors.h>
#include "main.h"

#include "proximity_detection.h"

//----------------------------------------------------- INTERNAL FUNCTIONS ------------------------------------------------------------------------------

static THD_WORKING_AREA(waProximityDetection, 128);
static THD_FUNCTION(ProximityDetection, arg){

	chRegSetThreadName(__FUNCTION__);
	(void)arg;

	int proximity3_value = 0;

	while(1){

    	proximity3_value = conv_prox_mm(get_calibrated_prox(2));

    	if(proximity3_value < GOAL_PROX_VALUE){
    		set_body_led(1);
    	}else{
    		set_body_led(0);
    	}

		chThdSleepMilliseconds(100);
	}
}

//Regulation des moteurs en fonction des valeurs des capteurs de proximité / thread utilisé pour test, à effacer/déplacer

static THD_WORKING_AREA(waProximityMotors, 128);
static THD_FUNCTION(ProximityMotors, arg){

	chRegSetThreadName(__FUNCTION__);
	(void)arg;

	systime_t time = 0;

	while(1){

		time = chVTGetSystemTime();

		if(get_prox(2) > 210){
			right_motor_set_speed(INITIAL_SPEED + 200);
			left_motor_set_speed(INITIAL_SPEED);
		}else if(get_prox(2) < 190){
			right_motor_set_speed(INITIAL_SPEED);
			left_motor_set_speed(INITIAL_SPEED + 200);
		}else{
			right_motor_set_speed(INITIAL_SPEED);
			left_motor_set_speed(INITIAL_SPEED);
		}

		/*if(get_prox(2) > 100 || get_prox(1) > 100 || get_prox(3) > 100){

			if(get_prox(1) > get_prox(3)){
				right_motor_set_speed(INITIAL_SPEED);
				left_motor_set_speed(INITIAL_SPEED - 300);
			}

			if(get_prox(1) < get_prox(3)){
				left_motor_set_speed(INITIAL_SPEED);
				right_motor_set_speed(INITIAL_SPEED - 300);
			}

		}else{
			left_motor_set_speed(INITIAL_SPEED);
			right_motor_set_speed(INITIAL_SPEED);
		}*/

		chThdSleepUntilWindowed(time, time + MS2ST(10));

	}

}

//----------------------------------------------------- EXTERNAL FUNCTIONS ------------------------------------------------------------------------------

// sensor value in mm conversion
float conv_prox_mm(int error){
	return 60 - error * PROX_MM_FACTOR;
}

void start_proximity_detection(void){
	chThdCreateStatic(waProximityDetection, sizeof(waProximityDetection), NORMALPRIO, ProximityDetection, NULL);
}

void start_proximity_motors(void){
	chThdCreateStatic(waProximityMotors, sizeof(waProximityMotors), NORMALPRIO, ProximityMotors, NULL);
}
