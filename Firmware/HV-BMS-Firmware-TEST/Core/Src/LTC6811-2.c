#include <LTC6811-2.h>


/* Start ADC Conversion for Status 
Inputs---
	MD - ADC Mode
	CHST - Stat Channels to be measured
*/
void LTC681x_adstat(uint8_t MD, uint8_t CHST){
	uint8_t cmd[4];
	uint8_t md_bits;
	
	md_bits = (MD & 0x02) >> 1;
	cmd[0] = md_bits + 0x04;
	md_bits = (MD & 0x01) << 7;
	cmd[1] = md_bits + 0x68 + CHST ;
	
	cmd_68(cmd);
}

/*Reads and parses the LTC6811 cell voltage registers.
Inputs---
	reg - Controls which cell voltage register is read back.
	segmaSlave - Array of the parsed cell codes
*/
//CURRENTLY READS ALL ADC CELL VOLTAGES (EVEN IF NOT CONNECTED AS THEY ARE IN 8R AND 8L)
void LTC6811_rdcv(uint8_t reg, segmaSlave *ic){
	uint8_t *cell_data;
	uint8_t numCells;
	cell_data = (uint8_t *) malloc(24*sizeof(uint8_t));		//2 bytes per cell (max 12 cells)

	uint8_t ic_Addrss = ic.segmaID*3;

	for(uint8_t i=0; i<3; i++){				//loop through each IC within a segment (required for address)
		for (uint8_t cell_reg = 1; cell_reg < NUM_CV_REG + 1; cell_reg++){	//loop through all cell regs

			LTC6811_rdcllV(cell_reg, ic_Addrss+i, cell_data);	//read cell voltages

			if(i==0){	//12 cells
				for(int j = 0; j < 12; j += 2){
					ic->cell_V[j] = (cell_data+j);		//stores lower byte
					ic->cell_V[j+1] = (cell_data+j+1);	//stores upper byte
				}
			}
			else if(i==1){		//8 cells
				for(int j = 12; j < 20; j += 2){
					ic->cell_V[j] = (cell_data+j);		//stores lower byte
					ic->cell_V[j+1] = (cell_data+j+1);	//stores upper byte
				}
			}
			else if(i==2){		//8 cells
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