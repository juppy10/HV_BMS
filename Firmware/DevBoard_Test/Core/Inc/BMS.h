#ifndef BMS_H_
#define BMS_H_

//Header files
#include "main.h"
#include "LTC_Structs.h"
#include "stm32f4xx_hal.h"
#include "current_sense_ADC.h"


HAL_StatusTypeDef SPI_Transmit(SPI_HandleTypeDef *hspi, uint8_t *data, uint16_t size);

HAL_StatusTypeDef SPI_Receive(SPI_HandleTypeDef *hspi, uint8_t *tx_buf, uint16_t tx_size, uint8_t *rx_buf, uint16_t rx_size);

void LTC6811_startup(LTC6811_2_IC *ic);

void LTC6811_startup_new(LTC6811_2_IC *ic);

void get_init_SoC(ACCUMULATOR *acc);

void SoC_Update(ACCUMULATOR *acc);

void updateSegmentVoltages(LTC6811_2_IC *ic);

void updateSegmentVoltages_new(LTC6811_2_IC *ic);

void updateSegmentVoltages_And_Temp(LTC6811_2_IC *ic);

void TEST_dischargeCell(LTC6811_2_IC *ic);

void TEST_dischargeCell2(LTC6811_2_IC *ic);

uint8_t check_UV_OV_flags(LTC6811_2_IC *ic);

uint8_t check_UV_OV_flags_new(LTC6811_2_IC *ic);

uint8_t check_temp(LTC6811_2_IC *ic);

void print_Cell_Voltages(LTC6811_2_IC *ic, ACCUMULATOR *acc);

uint8_t balanceThres(LTC6811_2_IC *ic);

void chargeMODE(LTC6811_2_IC *ic, ACCUMULATOR *acc);

uint8_t need_balance(LTC6811_2_IC *ic);

uint8_t need_balance_new(LTC6811_2_IC *ic);

void insertion_sort(uint16_t cellArry[15][2], uint8_t numCells);

#endif /* INC_BMS_H_ */

