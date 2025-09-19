#include "gallery.h"
#include "image/image_factory.h"
#include "../core/configs.h"
#include <cstring>
#include "thumbnail_utils.h"
#include "log.h"

Gallery::~Gallery() {
}

void Gallery::init() {
    thumbnailsPerPage = GRID_MAX_COLS*GRID_MAX_ROWS;
    fs::File root = LittleFS.open("/", "r");
    while(fs::File file = root.openNextFile()){
        if(file.isDirectory())
          continue;
        char buf[1000];
        snprintf(buf, sizeof(buf), "/%s", file.name());
        Image *img = ImageFactory::createImage(std::string(buf));
        if(img){
            img->setPosition(0,0); // todo decide where to set position
            images.push_back(img);
        }

        Image *thumbnail = ImageFactory::createDownscaledImage(std::string(buf));
        if(thumbnail){
          thumbnails.push_back(thumbnail);
        }

    }
    pageCount = thumbnails.size()/thumbnailsPerPage + (thumbnails.size()%thumbnailsPerPage==0 ? 0: 1);
    
    LOGF_I("Found %d images for %d pages\n", images.size(), pageCount);  
    LOGF_I("Max cols: %d, max rows: %d, thumbnails per page: %d\n", GRID_MAX_COLS, GRID_MAX_ROWS, thumbnailsPerPage);

    thumbnailsOnPage = getThumbnailsOnPage(0);
    showThumbnails();
    drawHighlightBox(HIGHLIGHT_COLOR);
}

void Gallery::draw(){
    if(imageIndex >= 0){
        images[imageIndex]->render(renderer);
    } else {
        if(lastLoadedThumbnailIndex<thumbnailsPerPage){
            int nextPage = pageNum+1;
            if(nextPage > pageCount)
                return;
            int nextThumb = std::min<int>(lastLoadedThumbnailIndex+nextPage*thumbnailsPerPage, thumbnails.size()-1);
            rendererCache.loadAndCache(thumbnails[nextThumb]->filePath);
            LOG_D("Finished load");
            lastLoadedThumbnailIndex++;
        }
    }
}

void Gallery::nextImage(){
    // todo move to some util
    imageIndex++;
    if(imageIndex >= images.size()){
        imageIndex = 0;
    }
    renderer.reset();
    images[imageIndex]->render(renderer);
}

void Gallery::nextHighlight(){
    // overwrite current highlight
    drawHighlightBox(BG_COLOR);   
    showThumbnail(highlightIndex);

    goToNextHighlightBox();
    drawHighlightBox(HIGHLIGHT_COLOR);  
  }

bool Gallery::isImageOpen(){
    return imageIndex >= 0;
}

void Gallery::openImage(){
    imageIndex = pageNum*thumbnailsPerPage + highlightIndex;
    renderer.reset();
    images[imageIndex]->render(renderer);
  }

void Gallery::closeImage(){
    imageIndex= -1;
    renderer.reset();
    showThumbnails();
    drawHighlightBox(HIGHLIGHT_COLOR);
    // advance page
}

void Gallery::goToNextHighlightBox(){
  highlightIndex++;
  LOGF_D("Highlighting index %d\n", highlightIndex);
 
  // todo fix highlighIndex going out of oounds here
  if(highlightIndex >= thumbnailsPerPage || highlightIndex+ pageNum*thumbnailsPerPage>= thumbnails.size()-1){
    LOG_D("Resetting highlight index");
    highlightIndex=0;
    nextPage();
  }
}

void Gallery::nextPage(){
    lastLoadedThumbnailIndex = 0;
    pageNum++;
    if(pageNum > pageCount){
      pageNum=0;
    }
    thumbnailsOnPage = getThumbnailsOnPage(pageNum);
    LOGF_D("Thumbnails on page %d: %d\n", pageNum, thumbnailsOnPage.size());
    renderer.reset();
    showThumbnails();
}

void Gallery::drawHighlightBox(uint32_t color){
  renderer.renderBorder(getBoxX(highlightIndex), getBoxY(highlightIndex), 
    GRID_ELEMENT_WIDTH, GRID_ELEMENT_HEIGHT, HIGHLIGHT_THICKNESS, color);
}

void Gallery::showThumbnails(){
  for (size_t i = 0; i < thumbnailsOnPage.size(); i++){
      showThumbnail(i);
  }
  LOGF_D("Showing %d thumbnails\n", thumbnailsOnPage.size());
}

void Gallery::showThumbnail(int i){
    showThumbnail(thumbnailsOnPage[i], getBoxX(i), getBoxY(i));
}

void Gallery::showThumbnail(Image* thumbnail, uint8_t x, uint8_t y){
    LOGF_D("Rendering thumbnail %s\n", thumbnail->filePath.c_str());
    thumbnail->setPosition(x, y);
    thumbnail->render(renderer);
}

std::span<Image*> Gallery::getThumbnailsOnPage(uint8_t page){
  uint16_t pageStart = page * thumbnailsPerPage;
  uint16_t pageEnd;
  if(thumbnails.size() - pageStart >= thumbnailsPerPage){
    pageEnd = pageStart + thumbnailsPerPage;
  } else {
    pageEnd = thumbnails.size();
  }
  LOGF_D("Page %d indexes: %d-%d(out of %d)\n", page, pageStart, pageEnd, thumbnails.size());
  return std::span{thumbnails.data() + pageStart, thumbnails.data() + pageEnd};
}


uint16_t Gallery::getBoxX(int i){
  return (i % GRID_MAX_COLS) * GRID_ELEMENT_WIDTH;
}

uint16_t Gallery::getBoxY(int i){
  return (i / GRID_MAX_COLS) * GRID_ELEMENT_HEIGHT;
}


