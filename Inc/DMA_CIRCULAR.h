#include "stm32f4xx_hal.h"

void USART_IrqHandler (UART_HandleTypeDef *huart, DMA_HandleTypeDef *hdma);

void DMA_IrqHandler (DMA_HandleTypeDef *hdma);
//void call_store_buffer(uint8_t val,uint8_t val2);
