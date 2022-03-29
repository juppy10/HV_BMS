/*
 * LTC6811_2.h
 *
 *  Created on: Mar 10, 2022
 *      Author: Will
 */

#ifndef LTC6811_2_H_
#define LTC6811_2_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "stm32f4xx_hal.h"
#include "main.h"
#include "LTC_Structs.h"
#include "BMS.h"



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
void LTC6811_WRCFGB(LTC6811_2_IC ic);

/* Reads the configuration register group*/
int8_t LTC6811_RDCFGA(LTC6811_2_IC *ic);
int8_t LTC6811_RDCFGB(LTC6811_2_IC *ic);
int8_t LTC6811_RDSTATA(LTC6811_2_IC *ic);
int8_t LTC6811_RDSTATB(LTC6811_2_IC *ic);
int8_t LTC6811_RDAUXD(LTC6811_2_IC *ic);

//clears status registers
void LTC6811_CLRSTAT(void);

/*Reads and parses the LTC6811 cell voltage registers.
Inputs---
    segmaSlave - Array of the parsed cell codes
*/
//CURRENTLY READS ONE SEGMENTS VOLTAGE
void LTC6811_rdcv(LTC6811_2_IC *ic);

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

#endif /* LTC6811_2_H_ */
