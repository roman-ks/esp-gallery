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
#include <span>

class Gallery {
    public:
        Gallery(Renderer &renderer): 
            renderer(renderer), images(), thumbnails() {};
        ~Gallery();

        void init();
        void draw();
        void nextImage();
        void nextHighlight();
        void nextPage();
        bool isImageOpen();
        void openImage();
        void closeImage();

    private:
        Renderer &renderer;  
        std::vector<Image*> images;
        std::vector<Image*> thumbnails;
        std::span<Image*> thumbnailsOnPage;
        int highlightIndex = 0;
        int imageIndex = -1;
        int pageNum = 0;
        int pageCount = 0;
        int thumbnailsPerPage=0;

        void goToNextHighlightBox();
        void drawHighlightBox(uint32_t);
        void showThumbnail(int index);
        void showThumbnail(Image* thumbnail, uint8_t x, uint8_t y);
        void showThumbnails();
        std::span<Image*> getThumbnailsOnPage(uint8_t page);
                
        uint16_t getBoxX(int i);
        uint16_t getBoxY(int i);
        

};

#endif