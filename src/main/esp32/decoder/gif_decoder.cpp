#include "gif_decoder.h"

GIFDecoder::GIFDecoder(){
    GIFDecoder::gif.begin(BIG_ENDIAN_PIXELS);
}

GIFDecoder::decode(const char* filepath, DrawCallbackFunc &drawCallback) {

}

void * GIFOpenFile(const char *filename, int32_t *pSize)
{
  GIFDecoder::file = std::make_unique<File>(LittleFS.open(filename, "r"));
  if (GIFDecoder::file) {
    *pSize = GIFDecoder::file->size();
    return (void *)GIFDecoder::file.get();
  }
  return nullptr;
}

void GIFCloseFile(void *pHandle)
{
  File *f = static_cast<File *>(pHandle);
  if (f != NULL)
     f->close();
} /* GIFCloseFile() */

int32_t GIFReadFile(GIFFILE *pFile, uint8_t *pBuf, int32_t iLen)
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

int32_t GIFSeekFile(GIFFILE *pFile, int32_t iPosition)
{ 
  int i = micros();
  File *f = static_cast<File *>(pFile->fHandle);
  f->seek(iPosition);
  pFile->iPos = (int32_t)f->position();
  i = micros() - i;
//  Serial.printf("Seek time = %d us\n", i);
  return pFile->iPos;
} /* GIFSeekFile() */
