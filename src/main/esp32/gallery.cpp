#include "gallery.h"
#include "image/image_factory.h"
#define LOG_LEVEL 1
#include "log.h"

Gallery::~Gallery() {
}

void Gallery::init() {
    LOG_D("Gallery init");
    Image *splashImg = ImageFactory::createImage(SPLACH_IMAGE_PATH);
    splashImg->setPosition(0,0);
    splashImg->render(renderer);
    LOG_D("Splash screen shown");
    uint32_t splashStart = millis();

    thumbnailsPerPage = GRID_MAX_COLS*GRID_MAX_ROWS;
    uint32_t start = millis();
    fs::File root = fileSys.open("/", FILE_READ);
    LOGF_D("Opened root dir in %dms\n", millis()-start);

    while(true){
        start = millis();
        bool isDir = false;
        auto filename = root.getNextFileName(&isDir);
        if(isDir)
            continue;

        if(!filename || filename.isEmpty()){
            break;
        }
        uint32_t openTime = millis()-start;
        
        start = millis();
        Image *img = ImageFactory::createImage(std::string(filename.c_str()));
        uint32_t imgCreateTime = millis()-start;
        if(img){
            img->setPosition(0,0); // todo decide where to set position
            images.push_back(img);
        }

        start = millis();
        Image *thumbnail = ImageFactory::createDownscaledImage(std::string(filename.c_str()));
        uint32_t thumbCreateTime = millis()-start;
        if(thumbnail){
          thumbnails.push_back(thumbnail);
        }
        // LOGF_T("File: %s opened in %dms (imgCreate: %dms, thumbCreate: %dms)\n", 
        //       filename.c_str(), openTime, imgCreateTime, thumbCreateTime);
    }
    pageCount = thumbnails.size()/thumbnailsPerPage + (thumbnails.size()%thumbnailsPerPage==0 ? 0: 1);
    
    LOGF_I("Found %d images for %d pages\n", images.size(), pageCount);  
    LOGF_I("Max cols: %d, max rows: %d, thumbnails per page: %d\n", GRID_MAX_COLS, GRID_MAX_ROWS, thumbnailsPerPage);

    thumbnailsOnPage = getThumbnailsOnPage(0);

    // keep splash for at least SPLASH_MIN_DISPLAY_TIME_MS
    uint32_t splashDuration = millis() - splashStart;
    if(splashDuration < SPLASH_MIN_DISPLAY_TIME_MS){
        delay(SPLASH_MIN_DISPLAY_TIME_MS - splashDuration);
    }

    renderer.reset();
    showThumbnails();
    drawHighlightBox(HIGHLIGHT_COLOR);
    thumbnailManager.setTotalPages(pageCount);
    thumbnailManager.setCurrentPage(0);
    delete splashImg;
}

void Gallery::draw(){
    if(imageIndex >= 0){
        images[imageIndex]->render(renderer);
    } else {
        thumbnailManager.loadNextThumbnail();
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
        // set to max possible highlight index on previous page, prev page will adjust if out of range
        highlightIndex = thumbnailsPerPage;
        prevPage();
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
        highlightIndex = 0;
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
    thumbnailsOnPage = getThumbnailsOnPage(pageNum);
    LOGF_D("Thumbnails on page %d: %d\n", pageNum, thumbnailsOnPage.size());
    renderer.reset();
    showThumbnails();
    // back off to last thumbnail if current highlight is out of range
    while(highlightIndex >= thumbnailsOnPage.size() && highlightIndex > 0){
        highlightIndex--;
    }
    drawHighlightBox(HIGHLIGHT_COLOR);
    thumbnailManager.setCurrentPage(pageNum);
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

// PageWindow
template<size_t Radius>
void Gallery::PageWindow<Radius>::setCurrentPage(int page){
    if(page == current) return;

    current = (page + totalPages) % totalPages;
    updateWindow();
}

template<size_t Radius>
bool Gallery::PageWindow<Radius>::contains(const int* arr, int len, int val){
    for(int i=0; i<count; ++i){
        if(arr[i] == val) return true;
    }
    return false;
}

template<size_t Radius>
void Gallery::PageWindow<Radius>::updateWindow(){
    LOGF_D("Updating current page: %d", current);
    memccpy(oldPages, pages, sizeof(pages), sizeof(pages));
    count = 0;
    for(int offset = -Radius; offset <= Radius; ++offset){
        int page = (current + offset + totalPages) % totalPages;
        pages[count++] = page;
    }

    for(int i=0; i<count; ++i){
        if(!contains(oldPages, count, pages[i])){
            onAdded(pages[i]);
        }
    }

    for(int i=0; i<count; ++i){
        if(!contains(pages, count, oldPages[i])){
            onRemoved(oldPages[i]);
        }
    }
}

// ThumbnailManager
void Gallery::ThumbnailManager::unloadPage(int page){
    for(auto& thumb : gallery.getThumbnailsOnPage(page)){
        gallery.rendererCache.unload(thumb->filePath);
    }
}

void Gallery::ThumbnailManager::loadPage(int page){
    nextThumbnailIndex = 0;
    currentPageThumbnails = gallery.getThumbnailsOnPage(page);
 }

bool Gallery::ThumbnailManager::loadNextThumbnail(){
    // todo use load queue
    return false;
 }




