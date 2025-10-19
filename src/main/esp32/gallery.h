#ifndef __ESP_GALLERY_H__
#define __ESP_GALLERY_H__

#include <vector>
#include "image/image.h"
#include "renderer/renderer.h"
#include <span>
#include <optional>
#include "renderer/renderer_cache.h"
#include "../core/configs.h"

class Gallery {
    public:
        Gallery(Renderer &renderer, RendererCache &rendererCache, fs::FS &fileSys): 
            renderer(renderer), rendererCache(rendererCache), fileSys(fileSys), 
            images(), thumbnails(), thumbnailsOnPage(), thumbnailManager() {};
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
    template<typename T> 
    std::span<T> getOnPage(T* arr, size_t size, int page);
                
        uint16_t getBoxX(int i);
        uint16_t getBoxY(int i);
        
        template<size_t Radius>
        class PageWindow {
            public:
                PageWindow(int totalPages, std::function<void(int)> onAdded,
                           std::function<void(int)> onRemoved):
                    current(-1), totalPages(totalPages), onAdded(std::move(onAdded)), onRemoved(std::move(onRemoved)) {
                        std::fill_n(pages, windowSize, -1);
                        std::fill_n(oldPages, windowSize, -1);
                }

                void setCurrentPage(int page);

                std::span<int> getPages(){
                    return std::span(pages, pages+windowSize);
                }
            private:
                static constexpr size_t windowSize = 2 * Radius + 1;
                int totalPages;
                int current;
                int pages[windowSize];
                int oldPages[windowSize];
                int count = 0;
                std::function<void(int)> onAdded;
                std::function<void(int)> onRemoved;

                bool contains(const int* arr, int len, int val);
                void updateWindow();
        };

        class ThumbnailManager{
            public: 
                ThumbnailManager(Gallery &gallery, int totalPages);
                void setCurrentPage(int page){
                    pageWindow.setCurrentPage(page);
                }
                bool loadNextThumbnail();
            private:
                Gallery &gallery;
                PageWindow<THUMBNAIL_PAGES_CACHED> pageWindow;
                std::unique_ptr<std::string[]> allImageNames;
                size_t allImageNamesSize;
                // Window size used by PageWindow is (2*THUMBNAIL_PAGES_CACHED + 1).
                // Ensure loadQueue can hold filenames for the whole window (worst-case).
                static const size_t maxLoadQueueSize = (2 * THUMBNAIL_PAGES_CACHED + 1) * GRID_MAX_COLS * GRID_MAX_ROWS;
                std::string loadQueue[maxLoadQueueSize];
                size_t loadQueueSize = 0;
                size_t loadQueueNextIndex = 0;

                void unloadPage(int page);
                void loadPage(int page);
                void rebuildLoadQueue();
        };
        friend class ThumbnailManager;
        std::optional<ThumbnailManager> thumbnailManager;
};

#endif