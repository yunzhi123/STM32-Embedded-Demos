#include "stm32f10x.h"
#include <stdio.h>
#include "delay.h"

void speak() {
	printf("hello everyone\n");
}

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
	// GPIOA and GPIOC clock enable
	RCC->APB2ENR |= (1<<2) | (1<<4);
	
	// PA2
	GPIOA->CRL &= ~(0xF<<8);
	GPIOA->CRL |= (0x9<<8);
	
	// PA3
	GPIOA->CRL &= ~(0xF<<12);
	GPIOA->CRL |= (0x4<<12);	
	
	// PA5
	GPIOA->CRL &= ~(0xF<<20);
	GPIOA->CRL |= (0x1<<20);
	
	// PC13 pull up input
	GPIOC->CRH &= ~(0xF<<20);
	GPIOC->CRH |= (0x8<<20);
	GPIOC->BSRR = (1<<13);

	My_USART2_Init();
	while (1) {
		
		if (USART2->SR & (1 << 5)) { 
				uint8_t byteRcvd = USART2->DR; 
				if (byteRcvd == '0') {
						GPIOA->BSRR = (1 << 5);
				} else if (byteRcvd == '1') {
						GPIOA->BRR = (1 << 5);
				}
		}
		if (!(GPIOC->IDR & (1<<13))) {
			speak();
			Delay(1000);
		}		
	}
}
