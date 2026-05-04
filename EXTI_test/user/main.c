#include "stm32f10x.h"

void button_init() {
	// GPIOA and AFIO clock enable
	RCC->APB2ENR |= (1<<2) | (1<<0);
	
	// LED PA5 -> PP
	GPIOA->CRL &= ~(0xf<<20);
	GPIOA->CRL |= (0x2<<20);	
	
	// button PA6, PA7 -> IPU
	GPIOA->CRL &= ~(0xffu<<24);
	GPIOA->CRL |= (0x88u<<24);
	GPIOA->BSRR |= (1<<7) | (1<<6);
	
	// 0000 is for PA
	// EXTICR[1] -> control port 4-7
	AFIO->EXTICR[1] &= ~(0xf<<8);
	AFIO->EXTICR[1] &= ~(0xf<<12);
	
	// open the interrupt mask
	EXTI->IMR |= (1<<6) | (1<<7);
	
	// rise trigger
	EXTI->RTSR |= (1<<6) | (1<<7);
	
}

void NVIC_init() {
	NVIC->IP [EXTI9_5_IRQn] = (0x0<<4);
	NVIC->ISER[EXTI9_5_IRQn>>5] |= (1<<23);
}

void EXTI9_5_IRQHandler() {
	if (EXTI->PR & (1<<6)) {
		GPIOA->BSRR = (1<<5);
		EXTI->PR = (1<<6);
	} 
	if (EXTI->PR & (1<<7)) {
		GPIOA->BSRR = (1<<21);
		EXTI->PR = (1<<7);
	}
}

int main(void)
{
	NVIC_init();
	button_init();
	while(1){}
}
