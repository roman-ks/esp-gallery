#pragma once

#include "SPI.h"

#define MAX_SD_INIT_ATTEMPTS 50

class SdIniter{
    public:
        SdIniter(int sck, int cs, SPIClass &spi)
            : sck(sck), cs(cs), spi(spi) {}
        bool init();
    private:
        int sck; 
        int cs;
        SPIClass &spi;

        void pulseClock(int count);
        void printSdInfo();
};