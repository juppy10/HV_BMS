/*
 * current_sense_ADC.h
 *
 *  Created on: 18 Apr. 2022
 *      Author: Will
 */

#ifndef SRC_CURRENT_SENSE_ADC_H_
#define SRC_CURRENT_SENSE_ADC_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "stm32f4xx_hal.h"
#include "main.h"
#include "LTC_Structs.h"
#include "BMS.h"

//ADC Constants
#define ADC_SCALE 			4095/5000	//idk what this is
#define ADC_SCALE_3V3		3300/4095	//ADC scale (3V3)
#define min_Pack_Current 	50 			//minimum registered pack current in mA
#define NUM_ADC_SAMPLES 	50			//number of samples for basic avg filter

//Current Sensors Constants
#define I_SENSE_OFFSET 2500
#define I_SENSE_SCALE_LOW 40
#define I_SENSE_SCALE_HIGH 10

void update_Current(ACCUMULATOR *acc);

#endif /* SRC_CURRENT_SENSE_ADC_H_ */
