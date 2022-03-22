#include <BMS.h>

HAL_StatusTypeDef SPI_Transmit(SPI_HandleTypeDef *hspi, uint8_t *data, uint16_t size){
	HAL_StatusTypeDef status;
	HAL_GPIO_WritePin(CS1_GPIO_Port, CS1_Pin, 0);
	status = HAL_SPI_Transmit(hspi, data, size, 100);
	HAL_GPIO_WritePin(CS1_GPIO_Port, CS1_Pin, 1);

	return status;
}

HAL_StatusTypeDef SPI_Receive(SPI_HandleTypeDef *hspi, uint8_t *tx_buf, uint16_t tx_size, uint8_t *rx_buf, uint16_t rx_size){
	HAL_StatusTypeDef status;
	HAL_GPIO_WritePin(CS1_GPIO_Port, CS1_Pin, 0);
	status = HAL_SPI_Transmit(hspi, tx_buf, tx_size, 100);
	status = HAL_SPI_Receive(hspi, rx_buf, rx_size, 100);
	HAL_GPIO_WritePin(CS1_GPIO_Port, CS1_Pin, 1);

	return status;
}

void LTC6811_startup(LTC6811_2_IC *ic){
	uint16_t cell_undervoltage, cell_overvoltage;
	ic->CFGR[0] = CFGR0_DEFAULT;

	cell_undervoltage = (CELL_UV/(16))-1;
	cell_overvoltage = (CELL_OV/(16));
	ic->CFGR[1] = (uint8_t)cell_undervoltage;
	ic->CFGR[2] = (uint8_t)((cell_undervoltage>>8) | (cell_overvoltage << 4));
	ic->CFGR[3] = (uint8_t)(cell_overvoltage>>4);												//CHECK WITH ONLINE COMPILER IS THIS ALL CHECKS OUT!!

	ic->CFGR[4] = CFGR4_DEFAULT;
	ic->CFGR[5] = CFGR5_DEFAULT;

	uint8_t cmd[]={0x00,0x01};

	wakeup_sleep();
	write_68(cmd, ic->CFGR);			//broadcast configuration to all ICs on bus

	uint8_t MD=0x10, DCP=0x00, CH=0x00;

	LTC6811_ADC_start(MD, DCP, CH);
	delay_us(3000);
	LTC6811_rdcv(ic);
}

void updateSegmentVoltages(LTC6811_2_IC *ic){
	uint8_t MD=0x10, DCP=0x01, CH=0x00;	//ADC mode, discharge permit and cell selection	-CHANGE LATER TO VARIABLE OR DEFINE

	wakeup_idle();						//wakeup the isoSPI interface
	LTC6811_ADC_start(MD, DCP, CH);		//start the ADC conversion for all cells - CHANGE THIS FUNCTION TO UPDATE ENTIRE ACCUMULATOR CELL VOLTAGE

	delay_us(3000);						//allow 3ms to pass for ADC conversion to finish
	LTC6811_rdcv(ic);				//read the cell voltages from registers and update the segment structure

}

void TEST_dischargeCell(LTC6811_2_IC *ic){
	wakeup_idle();
	if(ic->CFGR[4] != 0x07){			//toggle discharge
		ic->CFGR[4] = 0x07;
	} else ic->CFGR[4] = 0x00;
	//uint8_t cmd[2]={((ic.address << 3) | 0x80), 0x01};		FOR USE WITH ADDRESSABLE ICS

	uint8_t cmd[2]={0x00,0x01};		//write configA
	write_68(cmd, ic->CFGR);
}

void TEST_dischargeCell2(LTC6811_2_IC *ic){
	wakeup_idle();
	if(ic->CFGR[4] == 0x01){			//toggle discharge
		ic->CFGR[4] = 0x02;
	} else if(ic->CFGR[4] == 0x02){
		ic->CFGR[4] = 0x01;
	}
	//uint8_t cmd[2]={((ic.address << 3) | 0x80), 0x01};		FOR USE WITH ADDRESSABLE ICS

	uint8_t cmd[2]={0x00,0x01};		//write configA
	write_68(cmd, ic->CFGR);
}

uint8_t check_UV_OV_flags(LTC6811_2_IC *ic){
	LTC6811_RDAUXD(ic);		//read aux D register
	LTC6811_RDSTATB(ic);	//read status B register

	/*if((ic->STBR[2] & 0x01)==0x01){
		return 1;
	}
	else return 0;*/

	//check for either UV or OV
	if(ic->STBR[2] | ic->STBR[3] | ic->STBR[4]){			//this is all that is needed for the ltc6811-2!
		return 1;			//an overvoltage or undervoltage flag has been set by the BMS
	} else if (ic->AUXD[4] & 0b00111111){
		return 1;
	} else return 0;

}

void print_Cell_Voltages(uint16_t *cell_V){
	int str_len;
	char cellV[18];
	for(int i=0; i<15;i++){
		str_len = snprintf(cellV, 6, "%d",cell_V[i]);

		HAL_UART_Transmit(&huart2, (uint8_t *)cellV, str_len, 100);
	}
	str_len = sprintf(cellV, "\r\n");
	HAL_UART_Transmit(&huart2, (uint8_t *)cellV, str_len, 100);
}

void balance(LTC6811_2_IC *ic){
	//assume that at least one cell voltage is greater than STRT_BALANCE_THRES

	uint16_t minCellVoltage = 0xFFFF;
	for(uint8_t i=0; i < TOTAL_SEG_CELLS; i++){			//loop through all cells to determine if minimum cell voltage
		if(ic->cell_V[i]<minCellVoltage){
			minCellVoltage = ic->cell_V[i];
		}
	}
	for(uint8_t i=0; i < TOTAL_SEG_CELLS; i++){			//loop through all cells to determine if they need to be balanced
		if(ic->cell_V[i] - minCellVoltage >= dV_BAL_THRES){
			if(i > 7){		//7 IS FOR 1 IC - CURRENTLY UNSURE AS TO TREAT BALANCING ON A IC BASE OR SEGMENT/ACCUMULATOR

			}
		}
	}
}


