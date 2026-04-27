#include "stm32f10x.h"
#include "delay.h"

volatile uint32_t blinkInterval = 1000;

void LED_init() {
	RCC->APB2ENR |= (1<<2);
	GPIOA->CRL &= ~(0xf<<20);
	GPIOA->CRL |= (0x2<<20);
}

void USART_init() {
	RCC->APB1ENR |= (1<<17);
	GPIOA->CRL &= ~(0xff<<8);
	// PA2(TX) -> AF PP 
	GPIOA->CRL |= (0xa<<8);
	
	// PA3(RX) -> IPU 
	GPIOA->CRL |= (0x8<<12);
	GPIOA->BSRR |= (1<<3);
	
	USART2->BRR = 0x139;
	
	// TX RX USART enable
	USART2->CR1 |= (1<<2) | (1<<3) | (1<<13);
	
	// RXNE interrupt enable
	USART2->CR1 |= (1<<5);
}

void NVIC_init() {
	// interrupt priority
	NVIC->IP[USART2_IRQn] = (uint8_t)(0x00<<4);
	
	// 38>>5 = 1, 38 % 32 = 6
	NVIC->ISER[USART2_IRQn>>5] |= (1<<6);
}

void USART2_IRQHandler() {
	// check whether RXNE is 1 again to make sure do the right interrupt
	if (USART2->SR & (1<<5)) {
		uint8_t data = (uint8_t)(USART2->DR & 0xff);
		if (data == '0') blinkInterval = 1000;
		else if (data == '1') blinkInterval = 200;
		else if (data == '2') blinkInterval = 60;
	}
}

int main(void)
{
	LED_init();
	USART_init();
	NVIC_init();
	while(1)
	{
		GPIOA->BSRR = (1<<5);
		Delay(blinkInterval);
		GPIOA->BSRR = (1<<21);
		Delay(blinkInterval);
	}
}
