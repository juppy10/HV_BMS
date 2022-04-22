/*
 * current_sense_ADC.h
 *
 *  Created on: 18 Apr. 2022
 *      Author: Will
 */

#ifndef SRC_CURRENT_SENSE_ADC_H_
#define SRC_CURRENT_SENSE_ADC_H_

#include "main.h"
#include "LTC_Structs.h"
#include "stm32f4xx_hal.h"

//ADC Constants
#define ADC_SCALE 4095/5000

//Current Sensors Constants
#define I_SENSE_OFFSET 2500
#define I_SENSE_SCALE_LOW 40
#define I_SENSE_SCALE_HIGH 10

int16_t get_current(ACCUMULATOR *acc);

#endif /* SRC_CURRENT_SENSE_ADC_H_ */
