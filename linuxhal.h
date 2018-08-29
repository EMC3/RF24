#ifndef LINUXHAL_H
#define LINUXHAL_H

#include "hal.h"
#include <string>
class LinuxHal : public HAL
{
public:
    LinuxHal(const char *spiDevice, int gpio);

    // HAL interface
public:
    virtual void selectRf24(bool s);
    virtual void enableRf24(bool s);
    virtual void rf24SpiTransfer(void *tx, void *rx, int n);
    virtual void delay(uint64_t ms);
    virtual uint64_t ms();

private:
    void writeInFile(std::string f, std::string d);

    int spiFd;
    std::string gpioPath;
    uint32_t speed;
};

#endif // LINUXHAL_H
