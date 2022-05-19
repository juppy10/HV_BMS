/*
 * current_sense_ADC.c
 *
 *  Created on: 18 Apr. 2022
 *      Author: Will
 */
#include "current_sense_ADC.h"

extern ADC_HandleTypeDef hadc1;


void update_Current(ACCUMULATOR *acc){				//TODO!!!!: write readADC function
	int16_t packCurrent;
	uint32_t ADC_sum=0;
	for(int ii=0; ii<NUM_ADC_SAMPLES; ii++){
		HAL_ADC_Start(&hadc1);
		HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);
		ADC_sum += HAL_ADC_GetValue(&hadc1)*ADC_SCALE_3V3;
	}
	//uint32_t tim_Val = __HAL_TIM_GetCounter(&htim13);
	packCurrent = ((float) ADC_sum/NUM_ADC_SAMPLES-2500)*25;
	if(abs(packCurrent)<min_Pack_Current) packCurrent=0;
	/*int str_len;
	char cell_I[18];
	str_len = snprintf(cell_I, 8, "%d\r\n",packCurrent);
	HAL_UART_Transmit(&huart2, (int *)cell_I, str_len, 100);*/
	acc->pack_Current = packCurrent;

	/*int16_t high_I = 0, low_I = 0;

	low_I = ((readADC()*ADC_SCALE)-I_SENSE_OFFSET)/I_SENSE_SCALE_LOW;		//convert ADC to voltage - then interpret as current
	if(low_I >= 50){		//if lower current channel is maxxed out - use high
		high_I = ((readADC()*ADC_SCALE)-I_SENSE_OFFSET)/I_SENSE_SCALE_HIGH;		//convert ADC to voltage - then interpret as current
		return high_I;		//return current in mA
	}
	return low_I;			//return current in mA*/
}

