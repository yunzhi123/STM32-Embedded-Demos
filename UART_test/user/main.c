#include "stm32f10x.h"

void MY_USART_SendBytes(USART_TypeDef *USARTx, uint8_t* data, uint16_t size) {
	for (uint32_t i = 0; i<size; i++) {
		while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
		USART_SendData(USART2, data[i]);
	}
	while (USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);
}

int main(void)
{
	// remap USART pin from PA9, PA10 to PB6, PB7
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
//	AFIO->MAPR &= ~(1<<2);
//	AFIO->MAPR |= (1<<2);
	
	// rx, tx config
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	GPIOA->CRL &= ~(0xF<<8);
	GPIOA->CRL |= (0x9<<8);
	
	GPIOA->CRL &= ~(0xF<<12);
	GPIOA->CRL |= (0x4<<12);	

	// USART config
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	
	USART_InitTypeDef USART_InitStruct;
	
	USART_InitStruct.USART_BaudRate = 115200;
	USART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_InitStruct.USART_Parity = USART_Parity_No;
	USART_InitStruct.USART_StopBits = USART_StopBits_1;
	USART_InitStruct.USART_WordLength = USART_WordLength_8b;
	
	USART_Init(USART2, &USART_InitStruct);
	USART_Cmd(USART2, ENABLE);
	
	uint8_t send[] = {1,2,3,4,5};
	
	while(1)
	{
		MY_USART_SendBytes(USART2, send, 5);
		for(uint32_t i = 0; i < 2000000; i++); // ????
	}
}
