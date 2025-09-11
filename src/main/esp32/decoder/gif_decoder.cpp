#include "gif_decoder.h"
#include <LittleFS.h>

GIFDecoder::GIFDecoder(){
    GIFDecoder::gif.begin(BIG_ENDIAN_PIXELS);
}

GIFDecoder::GIFDecoder(std::vector<DelegatingDrawTargetFactory> &delegatingFactories): Decoder(delegatingFactories){
    GIFDecoder::gif.begin(BIG_ENDIAN_PIXELS);
}


void GIFDecoder::init(){
}

void GIFDecoder::decode(const char* filepath, IDrawTarget &iDrawTarget) {
  setIDrawTarget(wrapWithDelegates(&iDrawTarget));
  if(decodedPath.empty()){
    gif.open(filepath, &GIFDecoder::GIFOpenFile, &GIFDecoder::GIFCloseFile, 
            &GIFDecoder::GIFReadFile, &GIFDecoder::GIFSeekFile, &GIFDecoder::GIFDraw);
    decodedPath = std::string(filepath);
  }else{
      gif.playFrame(true, NULL);
  }
  destroyWrappingDelegates();
}

void * GIFDecoder::GIFOpenFile(const char *filename, int32_t *pSize)
{
  GIFDecoder::file = std::make_unique<File>(LittleFS.open(filename, "r"));
  if (GIFDecoder::file) {
    *pSize = GIFDecoder::file->size();
    return (void *)GIFDecoder::file.get();
  }
  return nullptr;
}

void GIFDecoder::GIFCloseFile(void *pHandle)
{
  File *f = static_cast<File *>(pHandle);
  if (f != NULL)
     f->close();
} /* GIFCloseFile() */

int32_t GIFDecoder::GIFReadFile(GIFFILE *pFile, uint8_t *pBuf, int32_t iLen)
{
    int32_t iBytesRead;
    iBytesRead = iLen;
    File *f = static_cast<File *>(pFile->fHandle);
    // Note: If you read a file all the way to the last byte, seek() stops working
    if ((pFile->iSize - pFile->iPos) < iLen)
       iBytesRead = pFile->iSize - pFile->iPos - 1; // <-- ugly work-around
    if (iBytesRead <= 0)
       return 0;
    iBytesRead = (int32_t)f->read(pBuf, iBytesRead);
    pFile->iPos = f->position();
    return iBytesRead;
} /* GIFReadFile() */

int32_t GIFDecoder::GIFSeekFile(GIFFILE *pFile, int32_t iPosition)
{
  int i = micros();
  File *f = static_cast<File *>(pFile->fHandle);
  f->seek(iPosition);
  pFile->iPos = (int32_t)f->position();
  i = micros() - i;
//  Serial.printf("Seek time = %d us\n", i);
  return pFile->iPos;
} /* GIFSeekFile() */

// Draw a line of image directly on the LCD
void GIFDecoder::GIFDraw(GIFDRAW *pDraw)
{
  uint8_t *s;
  uint16_t *d, *usPalette;
  int x, y, iWidth, iCount;

  // Display bounds check and cropping
  iWidth = pDraw->iWidth;
  if (iWidth + pDraw->iX > MAX_IMAGE_WIDTH)
    iWidth = MAX_IMAGE_HEIGHT - pDraw->iX;
  usPalette = pDraw->pPalette;
  y = pDraw->iY + pDraw->y; // current line
  if (y >= MAX_IMAGE_HEIGHT || pDraw->iX >= MAX_IMAGE_WIDTH || iWidth < 1)
    return;

  // Old image disposal
  s = pDraw->pPixels;
  if (pDraw->ucDisposalMethod == 2) // restore to background color
  {
    for (x = 0; x < iWidth; x++)
    {
      if (s[x] == pDraw->ucTransparent)
        s[x] = pDraw->ucBackground;
    }
    pDraw->ucHasTransparency = 0;
  }

  // Apply the new pixels to the main image
  if (pDraw->ucHasTransparency) // if transparency used
  {
    uint8_t *pEnd, c, ucTransparent = pDraw->ucTransparent;
    pEnd = s + iWidth;
    x = 0;
    iCount = 0; // count non-transparent pixels
    while (x < iWidth)
    {
      c = ucTransparent - 1;
      d = &GIFDecoder::usTemp[0][0];
      while (c != ucTransparent && s < pEnd && iCount < BUFFER_SIZE )
      {
        c = *s++;
        if (c == ucTransparent) // done, stop
        {
          s--; // back up to treat it like transparent
        }
        else // opaque
        {
          *d++ = usPalette[c];
          iCount++;
        }
      } // while looking for opaque pixels
      if (iCount) // any opaque pixels?
      {
        // DMA would degrtade performance here due to short line segments
        GIFDecoder::iDrawTarget->setAddrWindow(pDraw->iX + x, y, iCount, 1);
        GIFDecoder::iDrawTarget->pushPixels(GIFDecoder::usTemp, iCount);
        x += iCount;
        iCount = 0;
      }
      // no, look for a run of transparent pixels
      c = ucTransparent;
      while (c == ucTransparent && s < pEnd)
      {
        c = *s++;
        if (c == ucTransparent)
          x++;
        else
          s--;
      }
    }
  }
  else
  {
    s = pDraw->pPixels;

    // Unroll the first pass to boost DMA performance
    // Translate the 8-bit pixels through the RGB565 palette (already byte reversed)
    if (iWidth <= BUFFER_SIZE)
      for (iCount = 0; iCount < iWidth; iCount++) GIFDecoder::usTemp[dmaBuf][iCount] = usPalette[*s++];
    else
      for (iCount = 0; iCount < BUFFER_SIZE; iCount++) GIFDecoder::usTemp[dmaBuf][iCount] = usPalette[*s++];

    GIFDecoder::iDrawTarget->setAddrWindow(pDraw->iX, y, iWidth, 1);
    GIFDecoder::iDrawTarget->pushPixels(&GIFDecoder::usTemp[0][0], iCount);


    iWidth -= iCount;
    // Loop if pixel buffer smaller than width
    while (iWidth > 0)
    {
      // Translate the 8-bit pixels through the RGB565 palette (already byte reversed)
      if (iWidth <= BUFFER_SIZE)
        for (iCount = 0; iCount < iWidth; iCount++) GIFDecoder::usTemp[dmaBuf][iCount] = usPalette[*s++];
      else
        for (iCount = 0; iCount < BUFFER_SIZE; iCount++) GIFDecoder::usTemp[dmaBuf][iCount] = usPalette[*s++];
      GIFDecoder::iDrawTarget->pushPixels(&GIFDecoder::usTemp[0][0], iCount);
      iWidth -= iCount;
    }
  }
} /* GIFDraw() */
