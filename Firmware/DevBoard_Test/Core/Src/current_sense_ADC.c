/*
 * current_sense_ADC.c
 *
 *  Created on: 18 Apr. 2022
 *      Author: Will
 */


int16_t get_current(){				//TODO!!!!: write readADC function
	int16_t high_I = 0, low_I = 0;

	low_I = ((readADC()*ADC_SCALE)-I_SENSE_OFFSET)/I_SENSE_SCALE_LOW;		//convert ADC to voltage - then interpret as current
	if(low_I >= 50){		//if lower current channel is maxxed out - use high
		high_I = ((readADC()*ADC_SCALE)-I_SENSE_OFFSET)/I_SENSE_SCALE_HIGH;		//convert ADC to voltage - then interpret as current
		return high_I;		//return current in mA
	}
	return low_I;			//return current in mA
}
