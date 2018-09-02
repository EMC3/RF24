/*
 * HAL.h
 *
 * Created: 29.08.2018 17:07:35
 *  Author: gilbe
 */ 


#ifndef HAL_H_
#define HAL_H_

#include <stdint.h>

#define GPIO_OUT 1
#define GPIO_IN 0

#define HIGH 1 
#define LOW 0

class HAL{
public:
	HAL();
	
	void selectRf24(uint8_t state);
	void enableRf24(uint8_t state);
	
	void rf24SpiTransfer(const uint8_t * tx, uint8_t * rx, uint8_t n);
	
	void delay(uint32_t ms);
	uint32_t ms();
private:

};

#endif /* HAL_H_ */