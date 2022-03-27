/*
 * LTC_Structs.h
 *
 *  Created on: 22 Mar. 2022
 *      Author: Will
 */

#ifndef INC_LTC_STRUCTS_H_
#define INC_LTC_STRUCTS_H_

//physical constants
#define NUM_RX_BYT 	    8   //8
#define NUM_CV_REG 	    4 	//number of cell voltage registers per IC
#define TOTAL_IC	    3  	//number of ICs in the accumulator (currently set to 3 for testing purposes)
#define SEG_IC		    3 	//number of ICs per segment
#define TOTAL_SEG_CELLS	15	//number of cells in a segment ------------------ CHANGE THIS BACK!
#define TOTAL_CELLS 	144	//number of cells in an accumulator

//cell limits
#define CELL_UV         30000    //under cell voltage limit (E-4 V)
#define CELL_OV         42100    //over cell voltage limit (E-4 V)

#define dV_BAL_THRES 			100		//maximum allowed delta voltage, 10mV
#define STRT_BALANCE_THRES		40000	//voltage threshold (lower) for balancing
#define END_BALANCE_THRES		42000	//voltage threshold (upper) for balancing

//register constants
#define BYTES_IN_REG        6      //number of bytes in register groups
#define CMD_LEN             12     //number of bytes in an single read/write command

#define CFGR0_DEFAULT   0b11111100
#define CFGR4_DEFAULT   0x00
#define CFGR5_DEFAULT   0x00

/*! IC variable structure */
typedef struct{
	uint16_t cell_V[15];				//cell voltages
	uint8_t num_Cells;
	int8_t num_balanced_cells;
	uint8_t core_state;               	//variable indicates IC state 0 - sleep, 1 - standby, 2 - refup, 3 - measure, 4 - extended balancing
    uint8_t address;                  	//address of IC
    uint8_t CFGR[BYTES_IN_REG];        	//configuration register group data
    uint8_t CFGRB[BYTES_IN_REG];
    uint8_t STAR[BYTES_IN_REG];        	//status register A group data
    uint8_t STBR[BYTES_IN_REG];       	//status register B group data
    uint8_t AUXD[BYTES_IN_REG];			//AUX register D group data
} LTC6811_2_IC;

#endif /* INC_LTC_STRUCTS_H_ */
