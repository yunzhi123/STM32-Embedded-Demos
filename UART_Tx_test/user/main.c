#include "stm32f10x.h"
#include <stdio.h>

int fputc(int ch, FILE *f) {
	// read the TxE flag
	while (!(USART2->SR & (1<<7)));
	
	// write data
	USART2->DR = (uint8_t)ch;
	return ch;
}

void My_USART2_Init() {
	RCC->APB1ENR |= (1<<17);
	
	// baud rate
	USART2->BRR = 0x139;
	
	uint32_t tempCR1 = 0;
	tempCR1 &= ~(1 << 12); // M: 0 = 8 Data bits
	tempCR1 &= ~(1 << 10); // PCE: 0 = Parity control disabled
	tempCR1 |=  (1 << 3);  // TE: 1 = Transmitter enabled
	tempCR1 |=  (1 << 2);  // RE: 1 = Receiver enabled
	
	USART2->CR1 = tempCR1;

	// set stop bit = 1
	USART2->CR2 &= ~(0x3 << 12);

	// USART Enable
	USART2->CR1 |= (1 << 13);
}
	
int main(void)
{
	RCC->APB2ENR |= (1<<2);
	GPIOA->CRL &= ~(0xF<<8);
	GPIOA->CRL |= (0x9<<8);
	
	GPIOA->CRL &= ~(0xF<<12);
	GPIOA->CRL |= (0x4<<12);	

	My_USART2_Init();
	printf("hello world\n");
}
