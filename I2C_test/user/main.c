#include "stm32f10x.h"
#include <stdio.h>
#include "delay.h"

void I2C1_init() {
	// AFIO clock
	RCC->APB2ENR |= (1<<0);
	// GPIOB clock
	RCC->APB2ENR |= (1<<3);
	// I2C clock
	RCC->APB1ENR |= (1<<21);
	
	// I2C1 remap to PB8, PB9
	AFIO->MAPR |= (1<<1);
	
	// PB8, PB9 config -> AF open drain output
	GPIOB->CRH &= ~(0xFF<<0);
	GPIOB->CRH |= (0xFF<<0);
	
	// PB10 config -> General PP output(for boardLED)
	GPIOB->CRH &= ~(0xF<<8);
	GPIOB->CRH |= (0x1<<8);	

	// restart
	RCC->APB1RSTR |= (1<<21);
	RCC->APB1RSTR &= ~(1<<21);
	
	I2C1->CR2 = 36;                    // FREQ = 36
	I2C1->CCR = (1 << 15) | 30;        // FS = 1 (Fast), CCR = 30
	I2C1->TRISE = 11;                  // Max Rise Time

	// enable I2C1 and ACK
	I2C1->CR1 |= (1<<0) | (1<<10);
}

int I2C_Send(uint8_t Addr, uint8_t* data, uint16_t size) {
	// wait busy
	while (I2C1->SR2 & (1<<1));
	
	// send start bit
	I2C1->CR1 |= (1<<8);
	
	// Wait for SB flag to be set
	while (!(I2C1->SR1 & (1 << 0)));
	
	// clear AF(Ack failure) if set
	I2C1->SR1 &= ~(1<<10);
	// send address + write bit(0)
	I2C1->DR = (Addr & (0xfe));
	
	// SR1 Bit 1: ADDR(Address sent & match)
	while (!(I2C1->SR1 & (1<<1))) {
		// detect AF, send STOP and return -1, means find address failure
		if (I2C1->SR1 & (1<<10)) {
			I2C1->CR1 |= (1<<9);
			return -1;
		}
	}
	(void)I2C1->SR1;
	(void)I2C1->SR2;
	
	// send data
	for (uint16_t i = 0; i < size; i++) {
			while (!(I2C1->SR1 & (1 << 7))) {
					if (I2C1->SR1 & (1 << 10)) { 
							I2C1->CR1 |= (1 << 9);
							return -2;
					}
			}
			I2C1->DR = data[i];
	}
	while (!(I2C1->SR1 & (1 << 2)));
	I2C1->CR1 |= (1<<9);
	return 0;
}

int I2C_Receive(uint8_t Addr, uint8_t* data, uint16_t size) {
	// send start bit
	I2C1->CR1 |= (1<<8);
	
	// wait SB
	while (!(I2C1->SR1 & (1<<0)));
	
	// clear AF
	I2C1->SR1 &= ~(1<<10);
	
	// send address
	I2C1->DR = (Addr | 0x01);
	
	while (!(I2C1->SR1 & (1<<1))) {
		if (I2C1->SR1 & (1<<10)) {
			I2C1->CR1 |= (1<<9);
			return -1;
		}
	}
	
	if (size > 0) {
		(void)I2C1->SR1;
		(void)I2C1->SR2;
	}
	
	for (uint8_t i=0; i<size; i++) {
		if (i == size-1) {
			// send NACK ans set STOP
			I2C1->CR1 &= ~(1<<10);
			I2C1->CR1 |= 1<<9;
		} else {
			// send ACK to continue receiving
			I2C1->CR1 |= 1<<10;
		}
		// wait RxNE
		while (!(I2C1->SR1 & (1<<6)));
		data[i] = I2C1->DR;
	}
	return 0;
}

int main(void)
{
	I2C1_init();
	uint8_t datas[] = {0x00, 0x8d, 0x14, 0xaf, 0xa5};
	// 0x78 is a address of OLED SSD1306
	I2C_Send(0x78, datas, 5);
	Delay(10);
	uint8_t rcvd;
	I2C_Receive(0x78, &rcvd, 1);
	// detect whether the OLED is lighted
	if (!(rcvd & (0x01<<6))) {
		// if lighted, turn on the boardLED
		GPIOB->BSRR = 1<<10;
	} else GPIOB->BSRR = 1<<26;
}
