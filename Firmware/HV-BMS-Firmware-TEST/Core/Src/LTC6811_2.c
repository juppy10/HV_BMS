#include <LTC6811_2.h>

/* Generic function to write 68xx commands. Function calculates PEC for tx_cmd data. */
void cmd_68(uint8_t tx_cmd[2]){
	uint8_t cmd[4];
	uint16_t cmd_pec;
	uint8_t md_bits;
	
	cmd[0] = tx_cmd[0];
	cmd[1] =  tx_cmd[1];
	cmd_pec = LTC6811_pec15_calc(2, cmd);
	cmd[2] = (uint8_t)(cmd_pec >> 8);
	cmd[3] = (uint8_t)(cmd_pec);
	
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
	
	/*cs_low(CS_PIN);
	spi_write_array(CMD_LEN, cmd);
	cs_high(CS_PIN);*/
	
	free(cmd);
}

int8_t read_68(uint8_t tx_cmd[2], uint8_t *rx_data){
	uint8_t cmd[4];
	uint8_t data[BYTES_IN_REG];
	int8_t pec_error = 0;
	uint16_t cmd_pec;
	uint16_t data_pec;
	uint16_t received_pec;
	
	cmd[0] = tx_cmd[0];
	cmd[1] = tx_cmd[1];
	cmd_pec = LTC6811_pec15_calc(2, cmd);
	cmd[2] = (uint8_t)(cmd_pec >> 8);
	cmd[3] = (uint8_t)(cmd_pec);
	
	/*cs_low(CS_PIN);
	spi_write_read(cmd, 4, data, (BYTES_IN_REG));         //Transmits the command and reads the configuration data of all ICs on the daisy chain into rx_data[] array
	cs_high(CS_PIN);*/                                         

	for (uint8_t current_byte = 0; current_byte < BYTES_IN_REG; current_byte++){
		rx_data[current_byte] = data[current_byte];
	}
	
	received_pec = (rx_data[6]<<8) + rx_data[7];
	data_pec = LTC6811_pec15_calc(6, &rx_data[0]);
	
	if (received_pec != data_pec){
	  pec_error = -1;
	}
	
	return(pec_error);
}

//Wakes up all devices - NEED TO VERIFY THAT THE CS_LOW WILL FUNCTION (and write functions)
void wakeup_sleep(void){
   isoSPI_cs_low(CS_PIN);
   HAL_Delay_uS(400); 	// Guarantees the LTC6811 will be in standby
   isoSPI_cs_low(CS_PIN);
   HAL_Delay_uS(10);
}

//turns on all reference voltage
void refup_state(LTC6811_2_IC *ic){
   if(LTC6811_rdcfg(&ic)){
   	//return some fault?
   }
   ic->CFGR[0] = ic->CFGR[0] | 0x04;		//enable REFON bit
	LTC6811_wrcfg(ic);
}

/* Start cell voltage ADC Conversion for ALL devices */
void LTC6811_ADC_start(uint8_t MD, uint8_t DCP, uint8_t CH){
	uint8_t cmd[4];
	uint8_t md_bits;
	
	md_bits = (MD & 0x02) >> 1;
	cmd[0] = md_bits + 0x02;
	md_bits = (MD & 0x01) << 7;
	cmd[1] =  md_bits + 0x60 + (DCP<<4) + CH;
	
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

/* Writes the configuration register group*/
void LTC6811_WRCFGA(LTC6811_2_IC ic){
	uint8_t CMD[2];
	CMD[0] = ((ic->address << 3) | 0x80);			//CONFIGRES TO ONLY WRITE TO SPECIFIC ADDRESS -- CHECK BEFORE USE!!
	CMD[1] = 0X01;

	write_68(cmd, ic.CFGR);
}

/* Reads the configuration register group*/
int8_t LTC6811_RDCFGA(LTC6811_2_IC *ic){
	uint8_t CMD[2];
	CMD[0] = ((ic->address << 3) | 0x80);			//CONFIGRES TO ONLY READ TO SPECIFIC ADDRESS -- CHECK BEFORE USE!!
	CMD[1] = 0X02;

	if(read_68(cmd, ic->CFGR) == -1){
		//return some error? idk maybe try again??
	}
	return 0;
}

int8_t LTC6811_RDSTATA(LTC6811_2_IC *ic){
	uint8_t CMD[2];
	CMD[0] = ((ic->address << 3) | 0x80);			//CONFIGRES TO ONLY WRITE TO SPECIFIC ADDRESS -- CHECK BEFORE USE!!
	CMD[1] = 0X10;

	if(read_68(cmd, ic->STAR) == -1){
		//return some error? idk maybe try again??
	}
	return 0;
}

int8_t LTC6811_RDSTATB(LTC6811_2_IC *ic){
	uint8_t CMD[2];
	CMD[0] = ((ic->address << 3) | 0x80);			//CONFIGRES TO ONLY WRITE TO SPECIFIC ADDRESS -- CHECK BEFORE USE!!
	CMD[1] = 0X12;

	if(read_68(cmd, ic->STBR) == -1){
		//return some error? idk maybe try again??
	}
	return 0;
}

/*Reads and parses the LTC6811 cell voltage registers.
Inputs---
	segmaSlave - Array of the parsed cell codes
*/
//CURRENTLY READS ONE SEGMENTS VOLTAGE
void LTC6811_rdcv(segmaSlave *ic){
	uint8_t *cell_data;
	uint8_t numCells;
	cell_data = (uint8_t *) malloc(24*sizeof(uint8_t));		//2 bytes per cell (max 12 cells)

	uint8_t ic_Addrss = ic.segmaID*3;

	for(uint8_t i=0; i<3; i++){				//loop through each IC within a segment (required for address)
		for (uint8_t cell_reg = 1; cell_reg < NUM_CV_REG + 1; cell_reg++){	//loop through all cell regs

			LTC6811_rdcllV(cell_reg, ic_Addrss+i, cell_data);	//read cell voltages

			if(i==0){	//8R cells
				for(int j = 0; j < 8; j += 2){
					ic->cell_V[j] = (cell_data+j);		//stores lower byte
					ic->cell_V[j+1] = (cell_data+j+1);	//stores upper byte
				}
			}
			else if(i==1){		//12 cells
				for(int j = 8; j < 20; j += 2){
					ic->cell_V[j] = (cell_data+j);		//stores lower byte
					ic->cell_V[j+1] = (cell_data+j+1);	//stores upper byte
				}
			}
			else if(i==2){		//8L cells
				for(int j = 20; j < 28; j += 2){
					ic->cell_V[j] = (cell_data+j);		//stores lower byte
					ic->cell_V[j+1] = (cell_data+j+1);	//stores upper byte
				}
			}
		}
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
void LTC6811_rdcllV(uint8_t reg, uint8_t ic, uint8_t *data){
	uint8_t cmd[4];
	uint16_t cmd_pec;
	cmd[0] = ic;

	if(reg==1){			//1: RDCVA
		cmd[1] = 0x04;
	}
	else if(reg==2){ 	//2: RDCVB
		cmd[1] = 0x06;
	}
	else if(reg==3){ 	//3: RDCVC
		cmd[1] = 0x08;
	}
	else if(reg==4){ 	//4: RDCVD
		cmd[1] = 0x0A;
	else if(reg==5){ 	//4: RDCVE
		cmd[1] = 0x09;
	}
	else if(reg==6){ 	//4: RDCVF
		cmd[1] = 0x0B;
	}

	cmd_pec = LTC6811_rdcllV_pec15_calc(2, cmd);
	cmd[2] = (uint8_t)(cmd_pec >> 8);
	cmd[3] = (uint8_t)(cmd_pec);

	/*cs_low(CS_PIN);
	SPI WRITE DATA spi_write_read(cmd,4,data,(REG_LEN*TOTAL_IC));
	cs_high(CS_PIN);*/
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

void LTC6811_startup(LTC6811_2_IC *ic){
	uint8_t CFGR[BYTES_IN_REG];
	uint16_t cell_undervoltage, cell_overvoltage;
	CFGR[0] = CFGR0_DEFAULT;
	
	cell_undervoltage = (CELL_UV/(16))-1;
	cell_overvoltage = (CELL_OV/(16));
	CFGR[1] = (uint8_t)cell_undervoltage;
	CFGR[2] = (uint8_t)((cell_undervoltage>>8) & (cell_overvoltage << 4));
	CFGR[3] = (uint8_t)(cell_overvoltage>>4);												//CHECK WITH ONLINE COMPILER IS THIS ALL CHECKS OUT!! 

	CFGR[4] = CFGR4_DEFAULT;
	CFGR[5] = CFGR5_DEFAULT;

	uint8_t CMD[]={0x00,0x01};
	write_68(cmd, ic.CFGR);			//broadcast configuration to all ICs on bus
}