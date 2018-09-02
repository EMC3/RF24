/*
 * HAL.cpp
 *
 * Created: 29.08.2018 17:07:18
 *  Author: gilbe
 */ 
#include "HAL.h"

#define F_CPU 20000000
#include <avr/io.h>
#include "avr/delay.h"

static uint32_t msc;

HAL::HAL(){
	msc = 0;
	DDRB = 0;
	DDRB |= 1; // CE
	DDRB |= 1<<1; // PWMC
	DDRB |= 1<<2; // CSN
	DDRB |= 1<<3; // MOSI
	DDRB |= 1<<5; // SCK
	
	SPCR = ((1<<SPE)|               // SPI Enable
			(0<<SPIE)|              // SPI Interupt Enable
			(0<<DORD)|              // Data Order (0:MSB first / 1:LSB first)
			(1<<MSTR)|              // Master/Slave select
			(0<<SPR1)|(0<<SPR0)|    // SPI Clock Rate
			(0<<CPOL)|              // Clock Polarity (0:SCK low / 1:SCK hi when idle)
			(0<<CPHA));             // Clock Phase (0:leading / 1:trailing edge sampling)

	SPSR = (0<<SPI2X);              // NOT Double Clock Rate  --> 5Mhz
}

void HAL::selectRf24(uint8_t state){
	if(state){
		PORTB &= ~(1<<2);
	}else{
		PORTB |= 1<<2;
	}
}

void HAL::enableRf24(uint8_t state){
	if(state){
		PORTB |= 1;
		}else{
		PORTB &= ~(1);
	}
}

void HAL::rf24SpiTransfer(const uint8_t * tx, uint8_t * rx, uint8_t n){
	selectRf24(true);
	uint8_t i;
	for (i = 0; i < n; i++) {
		SPDR = *tx;
		while((SPSR & (1<<SPIF))==0);
		*rx = SPDR;
		tx++;
		rx++;
	}
	selectRf24(false);
}

void HAL::delay(uint32_t ms){
	while(ms > 0){
		_delay_ms(1);
		ms--;
	}
}

uint32_t HAL::ms(){
	return msc++;
}