#ifndef BMS_H_
#define BMS_H_

//Header files
#include "main.h"
#include "LTC_Structs.h"
#include "stm32f4xx_hal.h"


HAL_StatusTypeDef SPI_Transmit(SPI_HandleTypeDef *hspi, uint8_t *data, uint16_t size);

HAL_StatusTypeDef SPI_Receive(SPI_HandleTypeDef *hspi, uint8_t *tx_buf, uint16_t tx_size, uint8_t *rx_buf, uint16_t rx_size);

void LTC6811_startup(LTC6811_2_IC *ica);

void updateSegmentVoltages(LTC6811_2_IC *ic);

void TEST_dischargeCell(LTC6811_2_IC *ic);

void TEST_dischargeCell2(LTC6811_2_IC *ic);

uint8_t check_UV_OV_flags(LTC6811_2_IC *ic);

void print_Cell_Voltages(uint16_t *cell_V);

void balance(LTC6811_2_IC *ic);

#endif /* INC_BMS_H_ */

