#include "sd_init.h"
#include "SD.h"
#include "../log.h"

bool SdIniter::init(){
    bool mounted = false;
    int attempt = 0;
    // give time to settle
    delay(1000);
    LOG_D("Delay finished");
    do{
        pulseClock(200);
        if (!SD.begin(cs, spi)) {
            LOG_W("Card Mount Failed");
            SD.end();
            pulseClock(200);
            attempt++;
            uint16_t backoffMs = attempt*10;
            if(backoffMs>200)
                backoffMs=200;
            delay(backoffMs);
        } else {
            LOG_I("SD started");
            printSdInfo();
            mounted = true;
        }
    }while(!mounted);

    
    return mounted;
}

void SdIniter::pulseClock(int count){
    // spi.beginTransaction(SPISettings(240000, MSBFIRST, SPI_MODE0));
    digitalWrite(cs, HIGH);
    for(int i=0;i<count;i++){
        digitalWrite(sck, HIGH);
        digitalWrite(sck, LOW);
    }
    // spi.endTransaction();
}

void SdIniter::printSdInfo(){
  uint8_t cardType = SD.cardType();

  if (cardType == CARD_NONE) {
    LOG_W("No SD card attached");
    return;
  }

  LOG_I("SD Card Type: ");
  if (cardType == CARD_MMC) {
    LOG_I("SD Card Type: MMC");
  } else if (cardType == CARD_SD) {
    LOG_I("SD Card Type: SDSC");
  } else if (cardType == CARD_SDHC) {
    LOG_I("SD Card Type: SDHC");
  } else {
    LOG_I("SD Card Type: UNKNOWN");
  }

  uint64_t cardSize = SD.cardSize() / (1024 * 1024);
  LOGF_I("SD Card Size: %lluMB\n", cardSize);
}