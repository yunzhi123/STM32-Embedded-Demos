#include "stm32f10x.h"
#include <stdio.h>
#include "delay.h"

#define scl_w(v) ((v) ? (GPIOA->BSRR = (1<<0)) : (GPIOA->BSRR = (1<<16)))
#define sda_w(v) ((v) ? (GPIOA->BSRR = (1<<1)) : (GPIOA->BSRR = (1<<17)))
#define sda_r ((GPIOA->IDR & (1<<1)) ? 1 : 0)

void SI2C_init() {
	// GPIOA
	RCC->APB2ENR |= (1<<2);
	
	// PA0, PA1, general pp output
	GPIOA->CRL &= ~(0xFF<<0);
	GPIOA->CRL |= (0x66<<0);
}

void delay_u(uint8_t us) {
	uint32_t time = us * 8;
	for (uint32_t i=0; i<time; i++){}
}

void send_start() {
	sda_w(0);
	delay_u(1);
}

void send_stop() {
	scl_w(0);
	sda_w(0);
	delay_u(1);
	scl_w(1);
	delay_u(1);
	sda_w(1);
	delay_u(1);
}
	
uint8_t send_byte(uint8_t byte) {
	for (int i=7; i>=0; i--) {
		scl_w(0);
		if ((byte & (1<<i)) != 0) sda_w(1);
		else sda_w(0);
		delay_u(1);
		scl_w(1);
	}
	
	// ACK or NACK
	scl_w(0);
	sda_w(1);
	delay_u(1);
	scl_w(1);
	delay_u(1);
	return sda_r;
}

void OLED_SendCommands(uint8_t address, uint8_t *cmds, uint8_t len) {
    send_start();
    
    if (send_byte(address) != 0) { 
        send_stop(); 
        return; 
    }    
    send_byte(0x00);
    
    for (uint8_t i = 0; i < len; i++) {
        send_byte(cmds[i]);
    }
    
    send_stop();
}

int main(void)
{
    SI2C_init();
    
    uint8_t oled_init_data[] = {0x8D, 0x14, 0xAF, 0xA5};
    
    while(1) {
        OLED_SendCommands(0x78, oled_init_data, sizeof(oled_init_data));
        for(uint32_t i=0; i<1000000; i++); 
    }
}
