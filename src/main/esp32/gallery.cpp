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

    thumbnailManager.emplace(*this, pageCount);
    thumbnailManager.value().setCurrentPage(0);

    // load all thumbnails for current page
    int loaded = 0;
    while((loaded++<thumbnailsOnPage.size() || millis() - splashStart < SPLASH_MIN_DISPLAY_TIME_MS) 
            && thumbnailManager.value().loadNextThumbnail()){
        LOGF_D("Loading thumbnail on splash time: %d", loaded);
    }

   // keep splash for at least SPLASH_MIN_DISPLAY_TIME_MS
    uint32_t splashDuration = millis() - splashStart;
    if(splashDuration < SPLASH_MIN_DISPLAY_TIME_MS){
        LOGF_D("Waiting for splash to finish %lu", splashDuration);
        delay(SPLASH_MIN_DISPLAY_TIME_MS - splashDuration);
    }

    renderer.reset();
    showThumbnails();
    drawHighlightBox(HIGHLIGHT_COLOR);

    delete splashImg;
}

void Gallery::draw(){
    if(imageIndex >= 0){
        images[imageIndex]->render(renderer);
    } else {
        thumbnailManager.value().loadNextThumbnail();
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
    thumbnailManager.value().setCurrentPage(pageNum);
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
    LOGF_T("Rendering thumbnail %s\n", thumbnail->filePath.c_str());
    thumbnail->setPosition(x, y);
    thumbnail->render(renderer);
}

std::span<Image*> Gallery::getThumbnailsOnPage(uint8_t page){
  std::span<Image*> tmp_span = getOnPage<Image*>(thumbnails.data(), thumbnails.size(), page);
//   LOGF_D("Page %d indexes: %d-%d(out of %d)\n", 
//     page, thumbnails.data()-tmp_span.begin(), 
//     thumbnails.data()-tmp_span.end(), thumbnails.size());
  return tmp_span;
}

template<typename T> 
std::span<T> Gallery::getOnPage(T* arr, size_t size, int page){
    if(page < 0 || size <=0) return std::span<T>{};
    uint16_t pageStart = static_cast<uint16_t>(page) * thumbnailsPerPage;
    uint16_t pageEnd;
    if(size - pageStart >= thumbnailsPerPage){
        pageEnd = pageStart + thumbnailsPerPage;
    } else {
        // clamp to provided size
        pageEnd = size;
    }
    LOGF_D("getOnPage: Size: %d, page: %d, thumbnailsPerPage, %d, pageStart: %d, pageEnd: %d\n", 
        size, page, thumbnailsPerPage, pageStart, pageEnd);

    return std::span{arr + pageStart, arr + pageEnd};
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
    LOGF_D("PageWindow: Setting current page: new - %d, current - %d\n", page, current);
    if(page == current) 
        return;

    current = (page + totalPages) % totalPages;
    LOGF_D("PageWindow: Updated current - %d\n", current);
    updateWindow();
}

template<size_t Radius>
bool Gallery::PageWindow<Radius>::contains(const int* arr, int len, int val){
    for(int i=0; i<len; ++i){
        if(arr[i] == val) return true;
    }
    return false;
}

template<size_t Radius>
void Gallery::PageWindow<Radius>::updateWindow(){
    LOGF_D("Updating current page: %d\n", current);
    memcpy(oldPages, pages, sizeof(pages));
    count = 0;
    int radius = static_cast<int>(Radius);
    LOGF_D("Radius: %d\n", radius);
    for(int offset = -radius; offset <= radius; ++offset){
        int page = (current + offset + totalPages) % totalPages;
        pages[count++] = page;
    }

    if(LOG_LEVEL <= 1){
        LOG_D("Updated window: ");
        LOG_ARRAY("%d", pages, windowSize);
        LOG_D("Old window: ");
        LOG_ARRAY("%d", oldPages, windowSize);
    }

    LOG_T("Created pages");

    bool callBackCalled = false;
    if(onRemoved){
        for(int i=0; i<count; ++i){
            if(oldPages[i]==-1) continue;
            if(!contains(pages, count, oldPages[i])){
                onRemoved(oldPages[i]);
                callBackCalled = true;
                break;
            }
        }
    } else LOG_W("PageWindow: onRemoved is empty\n");

    if(callBackCalled)
        return;

    if(onAdded){
        bool added=false;
        for(int i=0; i<count; ++i){
            if(!contains(oldPages, count, pages[i])){
                added = true;
                break;
            }
        }
        onAdded();
    } else LOG_W("PageWindow: onAdded is empty");
}


// ----------------
// ThumbnailManager

Gallery::ThumbnailManager::ThumbnailManager(Gallery &gallery, int totalPages):
    gallery(gallery),
    pageWindow(totalPages, 
        [this]()->void {this->loadPage();},
        [this](int page)->void {this->unloadPage(page);}
){
    allImageNamesSize = gallery.thumbnails.size();
    LOGF_T("ThumbnailManager: allImageNamesSize: %d\n", allImageNamesSize);

    allImageNames = std::make_unique<std::string[]>(allImageNamesSize);
    int i=0;
    for (auto &thumbnailImage : gallery.thumbnails){
         allImageNames[i++] = std::string(thumbnailImage->filePath.c_str());
    }
}

void Gallery::ThumbnailManager::unloadPage(int page){
    for(auto& thumb : gallery.getThumbnailsOnPage(page)){
        gallery.rendererCache.unload(thumb->filePath);
    }
    rebuildLoadQueue();
}

void Gallery::ThumbnailManager::loadPage(){
    rebuildLoadQueue();
 }

 void Gallery::ThumbnailManager::rebuildLoadQueue(){
    loadQueueNextIndex = 0;
    size_t loadQueueEndIndex = 0;
    // todo replace with putting pages close to the middle first and farthest last
    LOG_D("Rebuilding load queue");
    for (auto &page : pageWindow.getPages()){
        LOGF_D("Adding page %d to load queue\n", page);
        if(page < 0 || page >= gallery.pageCount){
            LOGF_W("ThumbnailManager::loadPage: skipping invalid page %d\n", page);
            continue;
        }
        LOGF_D("loadPage: allImageNamesSize: %d\n", allImageNamesSize);
        std::span<std::string> currPageFilenames = gallery.getOnPage(allImageNames.get(), allImageNamesSize, page);
        size_t avail = maxLoadQueueSize - loadQueueEndIndex;
        if(currPageFilenames.size() > avail){
            LOGF_W("ThumbnailManager::loadPage: Truncating load queue: have %d, avail %d\n", currPageFilenames.size(), avail);
        }
        size_t toCopy = std::min(currPageFilenames.size(), avail);
        for(int i = 0;i < toCopy; i++){
            LOGF_T("i: %d, toCopy: %d, span size: %d\n", i, toCopy, currPageFilenames.size());
            LOGF_T("Load q: %s\n", loadQueue[loadQueueEndIndex+i].c_str());
            LOGF_T("Curr name: %s\n", currPageFilenames[i].c_str());
            loadQueue[loadQueueEndIndex+i]= currPageFilenames[i];
        }
        loadQueueEndIndex += toCopy;
    }
    loadQueueSize = loadQueueEndIndex;
 }

bool Gallery::ThumbnailManager::loadNextThumbnail(){
    if(loadQueueNextIndex >= loadQueueSize)
        return false;
    gallery.rendererCache.exists(loadQueue[loadQueueNextIndex++]);
    return true;
 }




