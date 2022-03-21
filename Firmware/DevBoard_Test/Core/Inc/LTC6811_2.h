/*
 * LTC6811_2.h
 *
 *  Created on: Mar 10, 2022
 *      Author: Will
 */

#ifndef INC_LTC6811_2_H_
#define INC_LTC6811_2_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "stm32f4xx_hal.h"
#include "main.h"
#include "BMS.h"

//physical constants
#define NUM_RX_BYT 	    8   //8
#define NUM_CV_REG 	    4 	//number of cell voltage registers per IC
#define TOTAL_IC	    3  	//number of ICs in the accumulator (currently set to 3 for testing purposes)
#define SEG_IC		    3 	//number of ICs per segment
#define TOTAL_SEG_CELLS	28	//number of cells in a segment
#define TOTAL_CELLS 	144	//number of cells in an accumulator

//cell limits
#define CELL_UV         30000    //under cell voltage limit (E-4 V)
#define CELL_OV         42100    //over cell voltage limit (E-4 V)

//register constants
#define BYTES_IN_REG        6      //number of bytes in register groups
#define CMD_LEN             12     //number of bytes in an single read/write command

#define CFGR0_DEFAULT   0b11111100
#define CFGR4_DEFAULT   0x00
#define CFGR5_DEFAULT   0x00

/*! Segment variable structure */
typedef struct{
	uint8_t segmaID;		//from 0 to 4
	uint16_t cell_V[15];	//cell voltages
} segmaSlave;

/*! IC variable structure */
typedef struct{
    uint8_t core_state;               	//variable indicates IC state 0 - sleep, 1 - standby, 2 - refup, 3 - measure, 4 - extended balancing
    uint8_t address;                  	//address of IC
    uint8_t CFGR[BYTES_IN_REG];        	//configuration register group data
    uint8_t CFGRB[BYTES_IN_REG];
    uint8_t STAR[BYTES_IN_REG];        	//status register A group data
    uint8_t STBR[BYTES_IN_REG];       	//status register B group data
    uint8_t AUXD[BYTES_IN_REG];			//AUX register D group data
} LTC6811_2_IC;




/* Generic function to write 68xx commands. Function calculates PEC for tx_cmd data. */
void cmd_68(uint8_t tx_cmd[2]);

/* Generic function to write 68xx commands and write payload data. Function calculates PEC for tx_cmd data and the data to be transmitted.*/
void write_68(uint8_t tx_cmd[2], uint8_t data[BYTES_IN_REG]);

int8_t read_68(uint8_t tx_cmd[2], uint8_t *rx_data);

//Wakes up all devices - NEED TO VERIFY THAT THE CS_LOW WILL FUNCTION (and write functions)
void wakeup_sleep(void);

//Wakes up all devices from idle
void wakeup_idle(void);

//turns on all reference voltage
void refup_state(LTC6811_2_IC *ic);

/* Start cell voltage ADC Conversion for ALL devices */
void LTC6811_ADC_start(uint8_t MD, uint8_t DCP, uint8_t CH);

/* Start open wire detection ADC Conversion for ALL devices */
void LTC6811_Open_Wire_ADC_start(uint8_t MD, uint8_t PUP, uint8_t DCP, uint8_t CH);

/* Writes the configuration register group*/
void LTC6811_WRCFGA(LTC6811_2_IC ic);

/* Reads the configuration register group*/
int8_t LTC6811_RDCFGA(LTC6811_2_IC *ic);

int8_t LTC6811_RDSTATA(LTC6811_2_IC *ic);

int8_t LTC6811_RDSTATB(LTC6811_2_IC *ic);

int8_t LTC6811_RDAUXD(LTC6811_2_IC *ic);

/*Reads and parses the LTC6811 cell voltage registers.
Inputs---
    segmaSlave - Array of the parsed cell codes
*/
//CURRENTLY READS ONE SEGMENTS VOLTAGE
void LTC6811_rdcv(segmaSlave *segma);

/* Writes the command and reads the raw cell voltage register data
Inputs---
    reg  - which cell voltage reg to read
    ic   - address of the IC to be read
    data - Array of unpared cell voltages
*/
void LTC6811_rdcllV(uint8_t reg, uint8_t *data);

/*Calculates  and returns the CRC15
Inputs ----
    len - Number of bytes that will be used to calculate a PEC
    data - Array of data that will be used to calculate  a PEC
*/
uint16_t LTC6811_pec15_calc(uint8_t len, uint8_t *data);

void LTC6811_startup(LTC6811_2_IC *ic, segmaSlave *segma);

void updateSegmentVoltages(segmaSlave *segma);

void TEST_dischargeCell(LTC6811_2_IC *ic);

void TEST_dischargeCell2(LTC6811_2_IC *ic);

uint8_t check_UV_OV_flags(LTC6811_2_IC *ic);

#endif /* INC_LTC6811_2_H_ */
