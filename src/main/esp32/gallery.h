#ifndef __ESP_GALLERY_H__
#define __ESP_GALLERY_H__

#include "SPI.h"
#include <TFT_eSPI.h>  
#include <vector>
#include "PNG_support.h"
#include "thumbnail_utils.h"
#include "LittleFS.h"
#include "image/image.h"
#include "renderer/renderer.h"
#include "../core/img_holder.h"

class Gallery {
    public:
        Gallery(Renderer &renderer): 
            renderer(renderer), images(), thumbnails() {};
        ~Gallery();

        void init();
        void draw();
        void nextImage();
        void nextHighlight();
        bool isImageOpen();
        void openImage();
        void closeImage();

    private:
        Renderer &renderer;  
        std::vector<Image*> images;
        std::vector<Image*> thumbnails;
        int highlightIndex = 0;
        int imageIndex = -1;

        void goToNextHighlightBox();
        void drawHighlightBox(uint32_t);
        void showThumbnail(int index);
        void showThumbnail(Image* thumbnail, uint8_t x, uint8_t y);
        void showThumbnails(std::vector<Image*> thumbnails);
                
        uint16_t getBoxX(int i);
        uint16_t getBoxY(int i);
        

};

#endif