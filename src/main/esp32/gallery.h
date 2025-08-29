#ifndef __ESP_GALLERY_H__
#define __ESP_GALLERY_H__

#include "SPI.h"
#include <TFT_eSPI.h>  
#include <vector>
#include "PNG_support.h"
#include "thumbnail_utils.h"
#include "LittleFS.h"
#include "../core/img_holder.h"

class Gallery {
    public:
        Gallery(TFT_eSPI &tft);
        ~Gallery();

        void init();
        void hightlightNext();

    private:
        TFT_eSPI &tft;
        short imageNum = 0;
        std::vector<char*> images;
        int highlightIndex = 0;

        void goToNextHighlightBox();
        void drawHighlightBox();
        void showThumbnail(int index);
        void showThumbnail(char* tnPath, int x, int y);
        void showThumbnails(std::vector<char*> thumbnailPaths);
        void showPng(IMG_HOLDER imageHolder, uint16_t x, uint16_t y);
        uint8_t getBoxX(int i);
        uint8_t getBoxY(int i);
};

#endif