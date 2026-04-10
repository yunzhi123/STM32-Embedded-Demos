#include "stm32f10x.h"
#include "delay.h"
int main(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
  GPIOA->CRL &= ~(0xF<<0);
	GPIOA->CRL |= (0x2<<0);
	
	GPIOA->CRL &= ~(0xF<<4);
	GPIOA->CRL |= (0x8<<4);
	GPIOA->BSRR = (1<<1);
	
	while(1)
	{
		if ((GPIOA->IDR & (1<<1)) == 0) {
			GPIOA->BSRR = (1<<0);
		} else {
			GPIOA->BSRR = (1<<16);
		}
	}
}
