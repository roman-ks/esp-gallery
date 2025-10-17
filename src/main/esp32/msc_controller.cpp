#include "msc_controller.h"
#include "USB.h"
#include "my_sd/MySD.h"
#define LOG_LEVEL 2
#include "log.h"

void MscController::onPause(){
    tft.fillScreen(TFT_BLACK);
}

void MscController::onResume(){
    if(usb_msc == nullptr){
        tft.fillScreen(TFT_BLACK);
        tft.setCursor(0, 2, 2);
        tft.setTextColor(TFT_WHITE);
        tft.println(" USB Drive mode activating...");
        LOGF_I("ARDUINO_USB_MODE: %d\n", ARDUINO_USB_MODE);
        uint32_t sectors = SD.numSectors();
        sectorSize = SD.sectorSize();
        usb_msc = std::make_unique<USBMSC>();

        usb_msc->vendorID("ESP32");       //max 8 chars
        usb_msc->productID("USB_MSC");    //max 16 chars
        usb_msc->productRevision("1.0");  //max 4 chars
        usb_msc->onRead(onRead);
        usb_msc->onWrite(onWrite);
        usb_msc->mediaPresent(true);
        usb_msc->isWritable(true); 

        usb_msc->begin(sectors, sectorSize);
        USB.begin();
        tft.println(" USB Drive mode activated");
        tft.println("\n\n Reboot to exit USB Drive mode");

        LOG_I("USB MSC started");
        // tft.println(" USB mode activated");

    }
}

int32_t MscController::onWrite(uint32_t lba, uint32_t offset, uint8_t *buffer, uint32_t bufsize) {
  LOGF_D("MSC WRITE: lba: %lu, offset: %lu, bufsize: %lu\n", lba, offset, bufsize);
  uint32_t sectors = bufsize/sectorSize;
  if(!SD.writeRAW((uint8_t*)buffer, lba, sectors)){
    return -1;
  }
  return bufsize;
}

int32_t MscController::onRead(uint32_t lba, uint32_t offset, void *buffer, uint32_t bufsize) {
  LOGF_D("MSC READ: lba: %lu, offset: %lu, bufsize: %lu\n", lba, offset, bufsize);
  uint32_t sectors = bufsize/sectorSize;
  if(!SD.readRAW((uint8_t*)buffer, lba, sectors)){
    return -1;
  }
  return bufsize;
}
