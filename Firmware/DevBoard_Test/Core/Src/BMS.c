#include <BMS.h>

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

void print_Cell_Voltages(uint16_t *cell_V){
	int str_len;
	char cellV[18];
	for(int i=0; i<15;i++){
		str_len = snprintf(cellV, 6, "%d",cell_V[i]);

		HAL_UART_Transmit(&huart2, (uint8_t *)cellV, str_len, 100);
	}
	str_len = sprintf(cellV, "\r\n");
	HAL_UART_Transmit(&huart2, (uint8_t *)cellV, str_len, 100);
}



