#ifndef HAL_H
#define HAL_H
#include <inttypes.h>

class HAL
{
public:
    HAL();
    virtual ~HAL();
    virtual void selectRf24(bool s) = 0;
    virtual void enableRf24(bool s) = 0;
    virtual void rf24SpiTransfer(void * tx, void * rx, int n) = 0;
    virtual void delay(uint64_t ms) = 0;
    virtual uint64_t ms() = 0;
};

#endif // HAL_H
