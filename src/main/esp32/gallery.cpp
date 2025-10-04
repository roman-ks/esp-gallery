#include "gallery.h"
#include "image/image_factory.h"
#include "../core/configs.h"
#include <cstring>
#include "thumbnail_utils.h"
#define LOG_LEVEL 2
#include "log.h"

Gallery::~Gallery() {
}

void Gallery::init() {
    thumbnailsPerPage = GRID_MAX_COLS*GRID_MAX_ROWS;
    fs::File root = fileSys.open("/", FILE_READ);
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
            rendererCache.exists(thumbnails[nextThumb]->filePath);
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

void Gallery::prevImage(){
    LOG_D("prevImage");
    imageIndex--;
    if(imageIndex < 0){
        imageIndex = images.size()-1;
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

void Gallery::prevHighlight(){
    // overwrite current highlight
    drawHighlightBox(BG_COLOR);   
    showThumbnail(highlightIndex);

    highlightIndex--;
    LOGF_D("Highlighting index %d\n", highlightIndex);
 
    if(highlightIndex < 0){
        prevPage();
        highlightIndex = thumbnailsPerPage-1;
        LOGF_D("Resetting highlight index to %d\n", highlightIndex);
    }
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
    pageNum = imageIndex/thumbnailsPerPage;
    highlightIndex = imageIndex % thumbnailsPerPage;
    imageIndex= -1;
    thumbnailsOnPage = getThumbnailsOnPage(pageNum);
    renderer.reset();
    showThumbnails();
    drawHighlightBox(HIGHLIGHT_COLOR);
}

void Gallery::goToNextHighlightBox(){
  highlightIndex++;
  LOGF_D("Highlighting index %d\n", highlightIndex);
 
  if(highlightIndex >= thumbnailsPerPage || highlightIndex+ pageNum*thumbnailsPerPage>= thumbnails.size()){
    highlightIndex=0;
    LOGF_D("Resetting highlight index to %d\n", highlightIndex);
    nextPage();
  }
}

void Gallery::nextPage(){
  pageNum++;
    if(pageNum >= pageCount){
        pageNum=0;
    }
    updatePage();
}

void Gallery::prevPage(){
    pageNum--;
    if(pageNum < 0){
      pageNum=pageCount-1; 
    }
    updatePage();
}

void Gallery::updatePage(){
    lastLoadedThumbnailIndex = 0;
    thumbnailsOnPage = getThumbnailsOnPage(pageNum);
    LOGF_D("Thumbnails on page %d: %d\n", pageNum, thumbnailsOnPage.size());
    renderer.reset();
    showThumbnails();
    drawHighlightBox(HIGHLIGHT_COLOR);
}

void Gallery::drawHighlightBox(uint32_t color){
  renderer.renderBorder(getBoxX(highlightIndex), getBoxY(highlightIndex), 
    GRID_ELEMENT_WIDTH, GRID_ELEMENT_HEIGHT, HIGHLIGHT_THICKNESS, color);
}

void Gallery::showThumbnails(){
  // todo consider calling getThumbnailsOnPage only in this method
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


