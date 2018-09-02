#include "linuxhal.h"
#include <fcntl.h>
#include <unistd.h>
#include <linux/spi/spidev.h>
#include <sys/ioctl.h>
#include <inttypes.h>
#include <string.h>
#include <string>
#include <time.h>
#include <sys/time.h>

LinuxHal::LinuxHal(const char * spiDevice, int gpio)
{
    speed = 4000000;
    spiFd = open(spiDevice,O_RDWR);
    uint32_t mode = 0;
    ioctl(spiFd, SPI_IOC_WR_MODE, &mode);
    ioctl(spiFd, SPI_IOC_RD_MODE, &mode);
    ioctl(spiFd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
    ioctl(spiFd, SPI_IOC_RD_MAX_SPEED_HZ, &speed);


    writeInFile("/sys/class/gpio/export",std::to_string(gpio));

    delay(5);

    writeInFile("/sys/class/gpio/gpio"+std::to_string(gpio)+"/direction","out");

    gpioPath = "/sys/class/gpio/gpio"+std::to_string(gpio)+"/value";
}


void LinuxHal::selectRf24(bool s)
{

}

void LinuxHal::enableRf24(bool s)
{
    if(s){
        writeInFile(gpioPath,"1");
    }
    else{
        writeInFile(gpioPath,"0");
    }
}

void LinuxHal::rf24SpiTransfer(void *tx, void *rx, int n)
{
    struct spi_ioc_transfer tr;
    memset(&tr, 0, sizeof(tr));
    tr.tx_buf = (uint64_t)tx;
    tr.rx_buf = (uint64_t)rx;
    tr.len = n;
    tr.speed_hz = speed; //RF24_SPIDEV_SPEED;
    tr.delay_usecs = 0;
    tr.bits_per_word = 8;
    tr.cs_change = 0;

    int ret;
    ret = ioctl(spiFd, SPI_IOC_MESSAGE(1), &tr);
}

void LinuxHal::delay(uint64_t ms)
{
    usleep(10+1000*ms);
}

uint64_t LinuxHal::ms()
{
    struct timeval t;
    gettimeofday(&t, NULL);
    uint64_t seconds  = t.tv_sec;
    uint64_t useconds = t.tv_usec;
    return  ((seconds) * 1000 + useconds/1000);
}
#include <fstream>

void LinuxHal::writeInFile(std::string f, std::string d)
{
    std::ofstream fs(f);
    fs << d;
    fs.close();
}
