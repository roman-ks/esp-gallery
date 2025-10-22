#include "gif_decoder.h"
#include "../../core/configs.h"
#define LOG_LEVEL 2
#include "../log.h"

GIFDecoder::GIFDecoder(DelegatingDrawTarget *firstDelegate, DelegatingDrawTarget *lastDelegate): 
  AdvancableDecoder(firstDelegate, lastDelegate) {
    GIFDecoder::gif.begin(BIG_ENDIAN_PIXELS);
}

void GIFDecoder::decode(uint8_t *fileBytes, size_t bytesCount, IDrawTarget &iDrawTarget) {
  gif.open(fileBytes, bytesCount, &GIFDecoder::GIFDraw);

  valid = false;
  LOGF_D("image specs: (%d x %d)\n",gif.getFrameWidth(), gif.getFrameHeight());
  if (gif.getFrameWidth() > MAX_IMAGE_WIDTH) {
      LOG_I("Image too wide for the screen!");
  } else if (gif.getFrameHeight() > MAX_IMAGE_HEIGHT) {
      LOG_I("Image too high for the screen!");
  } else {
      valid = true;
      advance(iDrawTarget);
  }
}

void GIFDecoder::advance(IDrawTarget &iDrawTarget){
  if(valid){
    setIDrawTarget(wrapWithDelegates(&iDrawTarget));
    gif.playFrame(true, NULL);
    destroyWrappingDelegates();
  }
}

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
