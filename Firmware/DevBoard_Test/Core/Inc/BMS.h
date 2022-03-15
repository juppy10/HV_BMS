#ifndef INC_BMS_H_
#define INC_BMS_H_

//Header files
#include "main.h"
#include "LTC6811_2.h"
#include "stm32f4xx_hal.h"


HAL_StatusTypeDef SPI_Transmit(SPI_HandleTypeDef *hspi, uint8_t *data, uint16_t size);

HAL_StatusTypeDef SPI_Receive(SPI_HandleTypeDef *hspi, uint8_t *tx_buf, uint16_t tx_size, uint8_t *rx_buf, uint16_t rx_size);

void print_Cell_Voltages(uint16_t *cell_V);


#endif /* INC_BMS_H_ */
