#include <BMS.h>

const uint16_t piecewise_SoC_OCV[][];

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
	ic.num_Cells = 15;
	ic->CFGR[0] = CFGR0_DEFAULT;
	ic->num_balanced_cells=0;
	//set UV & OV limits
	cell_undervoltage = (CELL_UV/(16))-1;
	cell_overvoltage = (CELL_OV/(16));
	ic->CFGR[1] = (uint8_t)cell_undervoltage;
	ic->CFGR[2] = (uint8_t)((cell_undervoltage>>8) | (cell_overvoltage << 4));
	ic->CFGR[3] = (uint8_t)(cell_overvoltage>>4);

	ic->CFGR[4] = CFGR4_DEFAULT;
	ic->CFGR[5] = CFGR5_DEFAULT;

	uint8_t cmd[]={0x00,0x01};

	wakeup_sleep();
	write_68(cmd, ic->CFGR);			//broadcast configuration to all ICs on bus

	uint8_t MD=0x02, DCP=0x00, CH=0x00;

	LTC6811_ADC_start(MD, DCP, CH);
	delay_us(3000);						//allow ADC to finish conversion
	LTC6811_rdcv(ic);
}

void get_init_SoC(ACCUMULATOR *acc){
	//find the region in piecewise linear model
	uint16_t avg_Cell_V = (acc->pack_V)/TOTAL_CELLS;
	if(avg_Cell_V >= 40000){															//greater than 4V
		acc->pack_SoC = avg_Cell_V*piecewise_SoC_OCV[0][0]*piecewise_SoC_OCV[0][1];
	}else if(avg_Cell_V < 40000 && avg_Cell_V >= 37000 ){								//between 4V and 3.7V
		acc->pack_SoC = avg_Cell_V*piecewise_SoC_OCV[1][0]*piecewise_SoC_OCV[1][1];
	}else if(avg_Cell_V < 37000 && avg_Cell_V >= 35000 ){								//between 3.7V and 3.5V
		acc->pack_SoC = avg_Cell_V*piecewise_SoC_OCV[2][0]*piecewise_SoC_OCV[2][1];
	}else if(avg_Cell_V < 35000){														//less than 3.5V
		acc->pack_SoC = avg_Cell_V*piecewise_SoC_OCV[3][0]*piecewise_SoC_OCV[3][1];
	}
}

void SoC_Update(ACCUMULATOR *acc){		//call me once every 100ms!
	float current = get_current();		//get the pack current (in mA)
	acc->coulomb_count_mAh -= current/SOC_UPDATE_TIME;
	acc->pack_SoC = (coulomb_count / PACK_CAPACITY_mAh)*100;	//SoC as %
}

void updateSegmentVoltages(LTC6811_2_IC *ic){
	//uint8_t MD=0x10, DCP=0x01, CH=0x00;	//ADC mode, discharge permit and cell selection	-CHANGE LATER TO VARIABLE OR DEFINE
	uint8_t MD=0x02, DCP=0x00, CH=0x00;		//ADC mode, discharge NOT permit and cell selection	-CHANGE LATER TO VARIABLE OR DEFINE

	wakeup_idle();						//wakeup the isoSPI interface
	LTC6811_ADC_start(MD, DCP, CH);		//start the ADC conversion for all cells - CHANGE THIS FUNCTION TO UPDATE ENTIRE ACCUMULATOR CELL VOLTAGE

	delay_us(3000);						//allow 3ms to pass for ADC conversion to finish
	LTC6811_rdcv(ic);				//read the cell voltages from registers and update the segment structure

}

void updateSegmentVoltages_And_Temp(LTC6811_2_IC *ic){
	updateSegmentVoltages(ic);

	uint8_t MD=0x02, CHG=0x00;			//ADC mode, and ADC selection	-CHANGE LATER TO VARIABLE OR DEFINE

	wakeup_idle();						//wakeup the isoSPI interface
	LTC6811_ADAX_start(MD, CHG);		//start the ADC conversion for all cells - CHANGE THIS FUNCTION TO UPDATE ENTIRE ACCUMULATOR CELL VOLTAGE

	delay_us(4000);						//allow 4ms to pass for ADC conversion to finish
	LTC6811_rdADC(ic);					//read the cell voltages from registers and update the segment structure
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

	//check for either UV or OV
	if(ic->STBR[2] | ic->STBR[3] | ic->STBR[4]){			//this is all that is needed for the ltc6811-2!
		return 1;			//an overvoltage or undervoltage flag has been set by the BMS
	} else if (ic->AUXD[4] & 0b00111111){
		return 1;
	} else return 0;

}

void print_Cell_Voltages(LTC6811_2_IC *ic){
	int str_len;
	char cellV[18];
	char cellT[18];
	char *charBuf = "0";
	for(int i=0; i<15;i++){
		str_len = snprintf(cellV, 6, "%d",ic->cell_V[i]);
		HAL_UART_Transmit(&huart2, (uint8_t *)cellV, str_len, 100);
	}
	for(int i=0; i<4;i++){
		str_len = snprintf(cellT, 6, "%d",ic->cell_temp[i]);
		for(uint8_t ii = str_len; ii<5 ; ii++){
			HAL_UART_Transmit(&huart2, (uint8_t *)charBuf, 1, 100);
		}
		HAL_UART_Transmit(&huart2, (uint8_t *)cellT, str_len, 100);
	}

	str_len = sprintf(cellV, "\r\n");
	HAL_UART_Transmit(&huart2, (uint8_t *)cellV, str_len, 100);
}

uint8_t balanceThres(LTC6811_2_IC *ic){
	for(uint8_t i=0; i < ic->num_Cells; i++){		//loop through all cells to determine if any cells are greater than threshold
		if(ic->cell_V[i] >= STRT_BALANCE_THRES) return 1;
	}
	return 0;
}

void chargeMODE(LTC6811_2_IC *ic){
	//if(balanceThres(ic)){		//determine if pack voltage is high enough for balancing
	if(ic->num_balanced_cells == 0){		//cells aren't being balanced or resting
		if(need_balance(ic)){				//determine if pack is in balance
			LTC6811_WRCFGA(*ic);			//write reg configs to IC - this starts balancing
			LTC6811_WRCFGB(*ic);

			HAL_TIM_Base_Start_IT(&htim12);		//start discharge timer


			/*TESTING CODE
			 * int str_len;
			char cellV[18];
			for(int i=0; i<15;i++){
				str_len = snprintf(cellV, 10, "%d %i ",cellArry[i][0],cellArry[i][1]);

				HAL_UART_Transmit(&huart2, (uint8_t *)cellV, str_len, 100);
			}
			str_len = sprintf(cellV, "\r\n");
			HAL_UART_Transmit(&huart2, (uint8_t *)cellV, str_len, 100);
			 */
			/*print_Cell_Voltages(ic->cell_V);
			int str_len;
			char cellV[18];
			str_len = snprintf(cellV, 20, "Pos: %i\r\n",cellThresPOS);
			HAL_UART_Transmit(&huart2, (uint8_t *)cellV, str_len, 100);*/

		}
	}
}

/*
returns 0 if cells are in balance
returns 1 if cells require balancing
*/
uint8_t need_balance(LTC6811_2_IC *ic){
	uint16_t cellArry[ic->num_Cells][2];
	int8_t cellThresPOS = -1;
	//updateSegmentVoltages(ic);
	LTC6811_RDCFGA(ic);
	LTC6811_RDCFGB(ic);
	ic->CFGR[4] = 0x00;
	ic->CFGR[5] = 0x00;
	ic->CFGRB[0] &= (0b10001111);						//clear discharge resistors
	//ic -> num_balanced_cells=0;							//reset number of cells to be balanced

	for (uint8_t i =0; i < ic->num_Cells; i++){
		cellArry[i][0] = ic->cell_V[i];
		cellArry[i][1] = i;
	}
	insertion_sort(cellArry,ic->num_Cells);		//sort from lowest to highest


	for (uint8_t j = 1; j < ic->num_Cells; j++){	//find lowest cell voltage that is above delta cell voltage balance threshold
		if(cellArry[j][0] - cellArry[0][0] > dV_BAL_THRES){
			cellThresPOS = j;
			break;
		}
	}
	if(cellThresPOS == -1) return 0;				//if within balance threshold no balacing to be done

	for(uint8_t ii = cellThresPOS; ii < ic->num_Cells; ii++){
		if(cellArry[ii][1] < 8){					//for register things - specific to LTC6812-1
			ic->CFGR[4] += (0x01 << cellArry[ii][1]);
			ic->num_balanced_cells++;
		}else if(cellArry[ii][1] > 7 && cellArry[ii][1] < 12){
			ic->CFGR[5] += (0x01 << (cellArry[ii][1]-8));
			ic->num_balanced_cells++;
		}else{
			ic->CFGRB[0] += (0x01 << (cellArry[ii][1]-12+4));
			ic->num_balanced_cells++;
		}
	}

	return 1;


	/*//assume that at least one cell voltage is greater than STRT_BALANCE_THRES

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
	}*/
}

void insertion_sort(uint16_t cellArry[15][2], uint8_t numCells){		//fix to dynamically allocate mrmy
	for(uint8_t ii = 1; ii < numCells; ii++) {
		uint16_t temp = cellArry[ii][0];
		uint8_t temp_cell = cellArry[ii][1];
		int8_t jj = ii - 1;
		while(jj >= 0 && temp <= cellArry[jj][0]) {
			cellArry[jj + 1][0] = cellArry[jj][0];
			cellArry[jj + 1][1] = cellArry[jj][1];
			jj--;
		}
		cellArry[jj + 1][0] = temp;
		cellArry[jj + 1][1] = temp_cell;
	}

}
