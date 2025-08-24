#ifndef __PNG_SUPPORT__
#define __PNG_SUPPORT__

#include "LittleFS.h"
#include <PNGdec.h>

void * pngOpen(const char *filename, int32_t *size);

void pngClose(void *handle);

int32_t pngRead(PNGFILE *page, uint8_t *buffer, int32_t length);

int32_t pngSeek(PNGFILE *page, int32_t position);

#endif
