#pragma once

#include <TFT_eSPI.h> 
#include "i_controller.h"
#include "USBMSC.h"
#include <memory>

class MscController : public IController {
    public:
        MscController(TFT_eSPI &tft) : tft(tft){}
        ~MscController() = default;

        void onPause() override;
        void onResume() override;

        // noop implementations
        void loop() override {};  
        void handleRightButtonPress() override {};
        void handleRightButtonLongPress() override {};

        void handleEnterButtonPress() override {};

        void handleLeftButtonPress() override {};
        void handleLeftButtonLongPress() override {};
    private:
        TFT_eSPI &tft;
        std::unique_ptr<USBMSC> usb_msc;
        inline static size_t sectorSize = 512;

        static int32_t onWrite(uint32_t lba, uint32_t offset, uint8_t *buffer, uint32_t bufsize); 
        static int32_t onRead(uint32_t lba, uint32_t offset, void *buffer, uint32_t bufsize);
};