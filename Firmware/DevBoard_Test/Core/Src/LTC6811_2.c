#include "LTC6811_2.h"

// precomputed CRC15 Table
const uint16_t crc15Table[256] = {0x0,0xc599, 0xceab, 0xb32, 0xd8cf, 0x1d56, 0x1664, 0xd3fd, 0xf407, 0x319e, 0x3aac,
                               0xff35, 0x2cc8, 0xe951, 0xe263, 0x27fa, 0xad97, 0x680e, 0x633c, 0xa6a5, 0x7558, 0xb0c1,
                               0xbbf3, 0x7e6a, 0x5990, 0x9c09, 0x973b, 0x52a2, 0x815f, 0x44c6, 0x4ff4, 0x8a6d, 0x5b2e,
                               0x9eb7, 0x9585, 0x501c, 0x83e1, 0x4678, 0x4d4a, 0x88d3, 0xaf29, 0x6ab0, 0x6182, 0xa41b,
                               0x77e6, 0xb27f, 0xb94d, 0x7cd4, 0xf6b9, 0x3320, 0x3812, 0xfd8b, 0x2e76, 0xebef, 0xe0dd,
                               0x2544, 0x2be, 0xc727, 0xcc15, 0x98c, 0xda71, 0x1fe8, 0x14da, 0xd143, 0xf3c5, 0x365c,
                               0x3d6e, 0xf8f7,0x2b0a, 0xee93, 0xe5a1, 0x2038, 0x7c2, 0xc25b, 0xc969, 0xcf0, 0xdf0d,
                               0x1a94, 0x11a6, 0xd43f, 0x5e52, 0x9bcb, 0x90f9, 0x5560, 0x869d, 0x4304, 0x4836, 0x8daf,
                               0xaa55, 0x6fcc, 0x64fe, 0xa167, 0x729a, 0xb703, 0xbc31, 0x79a8, 0xa8eb, 0x6d72, 0x6640,
                               0xa3d9, 0x7024, 0xb5bd, 0xbe8f, 0x7b16, 0x5cec, 0x9975, 0x9247, 0x57de, 0x8423, 0x41ba,
                               0x4a88, 0x8f11, 0x57c, 0xc0e5, 0xcbd7, 0xe4e, 0xddb3, 0x182a, 0x1318, 0xd681, 0xf17b,
                               0x34e2, 0x3fd0, 0xfa49, 0x29b4, 0xec2d, 0xe71f, 0x2286, 0xa213, 0x678a, 0x6cb8, 0xa921,
                               0x7adc, 0xbf45, 0xb477, 0x71ee, 0x5614, 0x938d, 0x98bf, 0x5d26, 0x8edb, 0x4b42, 0x4070,
                               0x85e9, 0xf84, 0xca1d, 0xc12f, 0x4b6, 0xd74b, 0x12d2, 0x19e0, 0xdc79, 0xfb83, 0x3e1a, 0x3528,
                               0xf0b1, 0x234c, 0xe6d5, 0xede7, 0x287e, 0xf93d, 0x3ca4, 0x3796, 0xf20f, 0x21f2, 0xe46b, 0xef59,
                               0x2ac0, 0xd3a, 0xc8a3, 0xc391, 0x608, 0xd5f5, 0x106c, 0x1b5e, 0xdec7, 0x54aa, 0x9133, 0x9a01,
                               0x5f98, 0x8c65, 0x49fc, 0x42ce, 0x8757, 0xa0ad, 0x6534, 0x6e06, 0xab9f, 0x7862, 0xbdfb, 0xb6c9,
                               0x7350, 0x51d6, 0x944f, 0x9f7d, 0x5ae4, 0x8919, 0x4c80, 0x47b2, 0x822b, 0xa5d1, 0x6048, 0x6b7a,
                               0xaee3, 0x7d1e, 0xb887, 0xb3b5, 0x762c, 0xfc41, 0x39d8, 0x32ea, 0xf773, 0x248e, 0xe117, 0xea25,
                               0x2fbc, 0x846, 0xcddf, 0xc6ed, 0x374, 0xd089, 0x1510, 0x1e22, 0xdbbb, 0xaf8, 0xcf61, 0xc453,
                               0x1ca, 0xd237, 0x17ae, 0x1c9c, 0xd905, 0xfeff, 0x3b66, 0x3054, 0xf5cd, 0x2630, 0xe3a9, 0xe89b,
                               0x2d02, 0xa76f, 0x62f6, 0x69c4, 0xac5d, 0x7fa0, 0xba39, 0xb10b, 0x7492, 0x5368, 0x96f1, 0x9dc3,
                               0x585a, 0x8ba7, 0x4e3e, 0x450c, 0x8095
                               };

/* Generic function to write 68xx commands. Function calculates PEC for tx_cmd data. */
void cmd_68(uint8_t tx_cmd[2]){
	uint8_t cmd[4];
	uint16_t cmd_pec;
	
	cmd[0] = tx_cmd[0];
	cmd[1] =  tx_cmd[1];
	cmd_pec = LTC6811_pec15_calc(2, cmd);
	cmd[2] = (uint8_t)(cmd_pec >> 8);
	cmd[3] = (uint8_t)(cmd_pec);
	
	SPI_Transmit(&hspi1, cmd, 4);

	/*cs_low(CS_PIN);
	spi_write_array(4,cmd);
	cs_high(CS_PIN);*/
}

/* Generic function to write 68xx commands and write payload data. Function calculates PEC for tx_cmd data and the data to be transmitted.*/
void write_68(uint8_t tx_cmd[2], uint8_t data[BYTES_IN_REG]){
	uint8_t *cmd;
	uint16_t data_pec;
	uint16_t cmd_pec;
	uint8_t cmd_index;
	
	cmd = (uint8_t *)malloc(CMD_LEN*sizeof(uint8_t));
	cmd[0] = tx_cmd[0];
	cmd[1] = tx_cmd[1];
	cmd_pec = LTC6811_pec15_calc(2, cmd);
	cmd[2] = (uint8_t)(cmd_pec >> 8);
	cmd[3] = (uint8_t)(cmd_pec);
	
	cmd_index = 4;

	for (uint8_t current_byte = 0; current_byte < BYTES_IN_REG; current_byte++){	//store the register data in buffer
		cmd[cmd_index] = data[current_byte];
		cmd_index++;
	}
	
	data_pec = (uint16_t)LTC6811_pec15_calc(BYTES_IN_REG, &data[0]);    // Calculating the PEC for each ICs configuration register data
	cmd[cmd_index] = (uint8_t)(data_pec >> 8);
	cmd[cmd_index + 1] = (uint8_t)data_pec;
	cmd_index = cmd_index + 2;

	wakeup_idle();
	
	SPI_Transmit(&hspi1, cmd, CMD_LEN);
	
	free(cmd);
}

int8_t read_68(uint8_t tx_cmd[2], uint8_t *rx_data){
	uint8_t cmd[4];
	uint8_t data[BYTES_IN_REG+2];
	int8_t pec_error = 0;
	uint16_t cmd_pec;
	uint16_t data_pec;
	uint16_t received_pec;
	
	cmd[0] = tx_cmd[0];
	cmd[1] = tx_cmd[1];
	cmd_pec = LTC6811_pec15_calc(2, cmd);
	cmd[2] = (uint8_t)(cmd_pec >> 8);
	cmd[3] = (uint8_t)(cmd_pec);
	
	wakeup_idle();

	SPI_Receive(&hspi1, cmd, 4, data, BYTES_IN_REG+2);

	for (uint8_t current_byte = 0; current_byte < BYTES_IN_REG+2; current_byte++){
		rx_data[current_byte] = data[current_byte];
	}
	
	received_pec = (((uint16_t) rx_data[6])<<8) + rx_data[7];
	data_pec = LTC6811_pec15_calc(6, &rx_data[0]);
	
	if (received_pec != data_pec){
	  pec_error = -1;
	}
	
	return(pec_error);
}

//Wakes up all devices from sleep
void wakeup_sleep(void){
	uint8_t dummyData = 0xFF;
	HAL_GPIO_WritePin(CS1_GPIO_Port, CS1_Pin, 0);
	for (int i =0; i<220; i++){
		HAL_SPI_Transmit(&hspi1, &dummyData, 1, 100);
	}
	HAL_GPIO_WritePin(CS1_GPIO_Port, CS1_Pin, 1);
}

//Wakes up all devices from idle
void wakeup_idle(void){
	uint8_t dummyData = 0xFF;
	HAL_GPIO_WritePin(CS1_GPIO_Port, CS1_Pin, 0);
	HAL_SPI_Transmit(&hspi1, &dummyData, 1, 100);
	HAL_GPIO_WritePin(CS1_GPIO_Port, CS1_Pin, 1);
}

//turns on all reference voltage
void refup_state(LTC6811_2_IC *ic){
   if(LTC6811_RDCFGA(ic)){
   	//return some fault?
   }
   ic->CFGR[0] = ic->CFGR[0] | 0x04;		//enable REFON bit
	LTC6811_WRCFGA(*ic);
}

//Start cell voltage ADC Conversion for ALL devices
void LTC6811_ADC_start(uint8_t MD, uint8_t DCP, uint8_t CH){
	uint8_t cmd[2];
	uint8_t md_bits;
	
	md_bits = (MD & 0x02) >> 1;
	cmd[0] = md_bits + 0x02;
	md_bits = (MD & 0x01) << 7;
	cmd[1] =  md_bits + 0x60 + (DCP<<4) + CH;

	cmd_68(cmd);
}

/* Start cell voltage ADC Conversion for ALL devices */
void LTC6811_ADAX_start(uint8_t MD, uint8_t CHG){
	uint8_t cmd[2];
	uint8_t md_bits;

	md_bits = (MD & 0x02) >> 1;
	cmd[0] = md_bits + 0x04;
	md_bits = (MD & 0x01) << 7;
	cmd[1] =  md_bits + 0x60 + CHG;

	cmd_68(cmd);
}

/* Start open wire detection ADC Conversion for ALL devices */
void LTC6811_Open_Wire_ADC_start(uint8_t MD, uint8_t PUP, uint8_t DCP, uint8_t CH){
	uint8_t cmd[4];
	uint8_t md_bits;
	
	md_bits = (MD & 0x02) >> 1;
	cmd[0] = md_bits + 0x02;
	md_bits = (MD & 0x01) << 7;
	cmd[1] =  md_bits + 0x28 + (PUP<<6) + CH + (DCP<<4);
	
	cmd_68(cmd);
}

/* Writes register groups*/
//Config_A
void LTC6811_WRCFGA(LTC6811_2_IC ic){
	uint8_t cmd[2];
	cmd[0] = 0x00;			//CONFIGRES TO ONLY WRITE TO SPECIFIC ADDRESS -- CHECK BEFORE USE!!
	cmd[1] = 0X01;

	write_68(cmd, ic.CFGR);
}

//Config_B
void LTC6811_WRCFGB(LTC6811_2_IC ic){		//NOT REQUIRED FOR THE LTC6811
	uint8_t cmd[2];
	cmd[0] = 0x00;			//BROADCASTS CONFIG!!
	cmd[1] = 0b00100100;

	write_68(cmd, ic.CFGRB);
}

/* Reads register groups*/
//Config_A
int8_t LTC6811_RDCFGA(LTC6811_2_IC *ic){
	uint8_t cmd[2];
	cmd[0] = ic->address;
	cmd[1] = 0X02;

	if(read_68(cmd, ic->CFGR) == -1){
		//return some error? idk maybe try again??
	}
	return 0;
}

//Config_B
int8_t LTC6811_RDCFGB(LTC6811_2_IC *ic){		//NOT REQUIRED FOR THE LTC6811
	uint8_t cmd[2];
	cmd[0] = ic->address;			//CONFIGRES TO ONLY READ TO SPECIFIC ADDRESS -- CHECK BEFORE USE!!
	cmd[1] = 0b00100110;

	if(read_68(cmd, ic->CFGRB) == -1){
		//return some error? idk maybe try again??
	}
	return 0;
}

//Status_A
int8_t LTC6811_RDSTATA(LTC6811_2_IC *ic){
	uint8_t cmd[2];
	cmd[0] = ic->address;
	cmd[1] = 0X10;

	if(read_68(cmd, ic->STAR) == -1){
		//return some error? idk maybe try again??
	}
	return 0;
}

//Status_B
int8_t LTC6811_RDSTATB(LTC6811_2_IC *ic){
	uint8_t cmd[2];
	cmd[0] = ic->address;
	cmd[1] = 0b00010010;

	if(read_68(cmd, ic->STBR) == -1){
		//return some error? idk maybe try again??
	}
	return 0;
}

//Aux_D
int8_t LTC6811_RDAUXD(LTC6811_2_IC *ic){
	uint8_t cmd[2];
	cmd[0] = ic->address;
	cmd[1] = 0x0F;

	if(read_68(cmd, ic->AUXD) == -1){
		//return some error? idk maybe try again??
	}
	return 0;
}

//Clear Status register groups
void LTC6811_CLRSTAT(void){
	uint8_t cmd[2];
	cmd[0] = 0b00000111;			//BROADCASTS CLEAR
	cmd[1] = 0b00010011;

	cmd_68(cmd);
}

/*Reads and parses the LTC6811 cell voltage registers.
Inputs---
	segmaSlave - Array of the parsed cell codes
*/
//CURRENTLY READS ONE SEGMENTS VOLTAGE
void LTC6811_rdcv(LTC6811_2_IC *ic){
	uint8_t *cell_data;
	cell_data = (uint8_t *) malloc(15*sizeof(uint8_t));		//2 bytes per cell (max 12 cells)

	//for(uint8_t i=0; i<3; i++){				//loop through each IC within a segment (required for address)
	for (uint8_t cell_reg = 0; cell_reg < NUM_CV_REG+1; cell_reg++){	//loop through all cell regs
		LTC6811_rdcllV(cell_reg, cell_data);	//read cell voltages

		for(int j = 0; j < 3; j++){
			ic->cell_V[j+(3*cell_reg)] =  (((uint16_t)(cell_data[2*j+1]))<<8) | (cell_data[2*j]);
		}

			/*if(i==0){			//8R cells
				for(int j = 0; j < 8; j++){
					segma->cell_V[j] =  (((uint16_t)(cell_data[j+1]))<<8) | (cell_data[j]);
				}
			}
			else if(i==1){		//12 cells
				for(int j = 8; j < 20; j += 2){
					segma->cell_V[j] =  (((uint16_t)(cell_data[j+1]))<<8) | (cell_data[j]);
				}
			}
			else if(i==2){		//8L cells
				for(int j = 20; j < 28; j += 2){
					segma->cell_V[j] =  (((uint16_t)(cell_data[j+1]))<<8) | (cell_data[j]);
				}
			}*/
	}	
	//LTC6811_check_pec(total_ic,CELL,ic);			//WORRY ABOUT PEC LATER
	free(cell_data);
}

/*Reads and parses the LTC6811 cell voltage registers.
Inputs---
	segmaSlave - Array of the parsed cell codes
*/
//CURRENTLY READS ONE SEGMENTS VOLTAGE
void LTC6811_rdcv_new(LTC6811_2_IC *ic){
	uint8_t *cell_data;
	cell_data = (uint8_t *) malloc(15*sizeof(uint8_t));		//2 bytes per cell (max 12 cells)

	//for(uint8_t i=0; i<3; i++){				//loop through each IC within a segment (required for address)
	for (uint8_t cell_reg = 0; cell_reg < NUM_CV_REG; cell_reg++){	//loop through all cell regs
		LTC6811_rdcllV_new(cell_reg, cell_data,ic->address);	//read cell voltages

		for(int j = 0; j < 3; j++){
			ic->cell_V[j+(3*cell_reg)] =  (((uint16_t)(cell_data[2*j+1]))<<8) | (cell_data[2*j]);

		}

			/*if(i==0){			//8R cells
				for(int j = 0; j < 8; j++){
					segma->cell_V[j] =  (((uint16_t)(cell_data[j+1]))<<8) | (cell_data[j]);
				}
			}
			else if(i==1){		//12 cells
				for(int j = 8; j < 20; j += 2){
					segma->cell_V[j] =  (((uint16_t)(cell_data[j+1]))<<8) | (cell_data[j]);
				}
			}
			else if(i==2){		//8L cells
				for(int j = 20; j < 28; j += 2){
					segma->cell_V[j] =  (((uint16_t)(cell_data[j+1]))<<8) | (cell_data[j]);
				}
			}*/
	}
	if(ic->num_Cells == 8){
		for(int jj=0;jj<4;jj++){						//adjust for only 8 cells
			ic->cell_V[4+jj]=ic->cell_V[6+jj];
		}
		ic->cell_V[8]=0; ic->cell_V[9]=0;
	}
	//LTC6811_check_pec(total_ic,CELL,ic);			//WORRY ABOUT PEC LATER
	free(cell_data);
}

/* Writes the command and reads the raw cell voltage register data 
Inputs---
	reg  - which cell voltage reg to read
	ic   - address of the IC to be read
	data - Array of unpared cell voltages
*/
void LTC6811_rdcllV(uint8_t reg, uint8_t *data){
	uint8_t cmd[4];
	uint16_t cmd_pec;
	cmd[0] = 0x00;					//changed to 0x00 from address for testing

	if(reg==0){			//1: RDCVA
		cmd[1] = 0x04;
	}
	else if(reg==1){ 	//2: RDCVB
		cmd[1] = 0x06;
	}
	else if(reg==2){ 	//3: RDCVC
		cmd[1] = 0x08;
	}
	else if(reg==3){ 	//4: RDCVD
		cmd[1] = 0x0A;
	}
	else if(reg==4){ 	//4: RDCVE
		cmd[1] = 0x09;
	}

	cmd_pec = LTC6811_pec15_calc(2, cmd);
	cmd[2] = (uint8_t)(cmd_pec >> 8);
	cmd[3] = (uint8_t)(cmd_pec);
	SPI_Receive(&hspi1, cmd, 4, data, BYTES_IN_REG+2);
}

/* Writes the command and reads the raw cell voltage register data
Inputs---
	reg  - which cell voltage reg to read
	ic   - address of the IC to be read
	data - Array of unpared cell voltages
*/
void LTC6811_rdcllV_new(uint8_t reg, uint8_t *data, uint8_t address){
	uint8_t cmd[4];
	uint16_t cmd_pec;
	//cmd[0] = 0x00;					//changed to 0x00 from address for testing
	cmd[0] = address;				//address of IC on segment PCB

	if(reg==0){			//1: RDCVA
		cmd[1] = 0x04;
	}
	else if(reg==1){ 	//2: RDCVB
		cmd[1] = 0x06;
	}
	else if(reg==2){ 	//3: RDCVC
		cmd[1] = 0x08;
	}
	else if(reg==3){ 	//4: RDCVD
		cmd[1] = 0x0A;
	}
	else if(reg==4){ 	//4: RDCVE
		cmd[1] = 0x09;
	}

	cmd_pec = LTC6811_pec15_calc(2, cmd);
	cmd[2] = (uint8_t)(cmd_pec >> 8);
	cmd[3] = (uint8_t)(cmd_pec);
	SPI_Receive(&hspi1, cmd, 4, data, BYTES_IN_REG+2);
}

void LTC6811_rdADC(LTC6811_2_IC *ic){
	uint8_t cmd[4];
	uint16_t cmd_pec;
	uint8_t data[8];

	cmd[0] = ic->address;					//changed to 0b11010000 from address for testing

	//read GPIO ADC 1,2
	cmd[1] = 0x0C;					//RDAUXA
	cmd_pec = LTC6811_pec15_calc(2, cmd);
	cmd[2] = (uint8_t)(cmd_pec >> 8);
	cmd[3] = (uint8_t)(cmd_pec);
	SPI_Receive(&hspi1, cmd, 4, data, BYTES_IN_REG+2);
	ic->cell_temp[0] = (data[1] << 8) | data[0];
	ic->cell_temp[1] = (data[3] << 8) | data[2];

	//read GPIO ADC 6,7
	cmd[1] = 0x0D;		//RDAUXC
	cmd_pec = LTC6811_pec15_calc(2, cmd);
	cmd[2] = (uint8_t)(cmd_pec >> 8);
	cmd[3] = (uint8_t)(cmd_pec);
	SPI_Receive(&hspi1, cmd, 4, data, BYTES_IN_REG+2);
	ic->cell_temp[2] = (data[1] << 8) | data[0];
	ic->cell_temp[3] = (data[3] << 8) | data[2];

}

void LTC6811_MUTE(){
	uint8_t cmd[2] = {0b00000000, 0b00101000};
	cmd_68(cmd);
}

void LTC6811_UNMUTE(){
	uint8_t cmd[2] = {0b00000000, 0b00101001};
	cmd_68(cmd);
}


/*Calculates  and returns the CRC15 
Inputs ----
	len - Number of bytes that will be used to calculate a PEC
	data - Array of data that will be used to calculate  a PEC
*/
uint16_t LTC6811_pec15_calc(uint8_t len, uint8_t *data){
	uint16_t remainder,addr;
	remainder = 16;//initialize the PEC
	
	for (uint8_t i = 0; i<len; i++){ // loops for each byte in data array
		addr = ((remainder>>7)^data[i])&0xff;//calculate PEC table address
		remainder = (remainder<<8)^crc15Table[addr];		
	}
	
	return(remainder*2);//The CRC15 has a 0 in the LSB so the remainder must be multiplied by 2
}



