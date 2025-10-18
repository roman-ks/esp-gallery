#ifndef __ESP_GALLERY_H__
#define __ESP_GALLERY_H__

#include <vector>
#include "image/image.h"
#include "renderer/renderer.h"
#include <span>
#include "renderer/renderer_cache.h"
#include "../core/configs.h"

class Gallery {
    public:
        Gallery(Renderer &renderer, RendererCache &rendererCache, fs::FS &fileSys): 
            renderer(renderer), rendererCache(rendererCache), fileSys(fileSys), 
            images(), thumbnails(), thumbnailsOnPage(), thumbnailManager(*this) {};
        ~Gallery();

        void init();
        void draw();
        void nextImage();
        void prevImage();
        void nextHighlight();
        void prevHighlight();
        void nextPage();
        void prevPage();
        void updatePage();
        bool isImageOpen();
        void openImage();
        void closeImage();

    private:
        Renderer &renderer; 
        RendererCache &rendererCache; 
        fs::FS &fileSys;
        std::vector<Image*> images;
        std::vector<Image*> thumbnails;
        std::span<Image*> thumbnailsOnPage;
        int highlightIndex = 0;
        int imageIndex = -1;
        int pageNum = 0;
        int pageCount = 0;
        int maxThumbnailsPerPage=0;
        int thumbnailsPerPage=0;
        // int lastLoadedThumbnailIndex = 0;

        void goToNextHighlightBox();
        void drawHighlightBox(uint32_t);
        void showThumbnail(int index);
        void showThumbnail(Image* thumbnail, uint8_t x, uint8_t y);
        void showThumbnails();
        std::span<Image*> getThumbnailsOnPage(uint8_t page);
                
        uint16_t getBoxX(int i);
        uint16_t getBoxY(int i);
        
        template<size_t Radius>
        class PageWindow {
            public:
                PageWindow(std::function<void(int)> onAdded,
                           std::function<void(int)> onRemoved):
                    current(-1) {
                }

                void setCurrentPage(int page);
                void setTotalPages(int pages){
                    totalPages = pages;
                }
            private:
                static constexpr size_t totalCount = 2 * Radius + 1;
                int totalPages;
                int current;
                int pages[totalCount];
                int oldPages[totalCount];
                int count = 0;
                std::function<void(int)> onAdded;
                std::function<void(int)> onRemoved;

                bool contains(const int* arr, int len, int val);
                void updateWindow();
        };

        class ThumbnailManager{
            public: 
                ThumbnailManager(Gallery &gallery):
                    gallery(gallery),
                    pageWindow(
                        [this](int page)->void {this->loadPage(page);},
                        [this](int page)->void {this->unloadPage(page);}
                    ){}
                void setCurrentPage(int page){
                    pageWindow.setCurrentPage(page);
                }

                void setTotalPages(int pages){
                    pageWindow.setTotalPages(pages);
                }
                bool loadNextThumbnail();
            private:
                Gallery &gallery;
                PageWindow<THUMBNAIL_PAGES_CACHED> pageWindow;
                std::span<Image*> currentPageThumbnails;
                int nextThumbnailIndex = 0;

                void unloadPage(int page);
                void loadPage(int page);
        };
        friend class ThumbnailManager;
        ThumbnailManager thumbnailManager;
};

#endif