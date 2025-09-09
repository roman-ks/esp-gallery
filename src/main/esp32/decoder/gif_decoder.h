#pragma once

#include "decoder.h"
#include <FS.h>
#include <AnimatedGIF.h>
#define BUFFER_SIZE 256            // Optimum is >= GIF width or integral division of width
#include "../../core/configs.h"
#include <string>


class GIFDecoder: public Decoder {

    public:
        GIFDecoder();
        ~GIFDecoder()=default;

        void init() override;
        void decode(const char* filepath, IDrawTarget &iDrawTarget) override;
        static void * GIFOpenFile(const char *filename, int32_t *pSize);
        static void GIFCloseFile(void *pHandle);
        static int32_t GIFReadFile(GIFFILE *pFile, uint8_t *pBuf, int32_t iLen);
        static int32_t GIFSeekFile(GIFFILE *pFile, int32_t iPosition);
        static void GIFDraw(GIFDRAW *pDraw);

        static void setIDrawTarget(IDrawTarget &cb){
            iDrawTarget = &cb;
        }

    private:
        inline static IDrawTarget *iDrawTarget = nullptr;
        inline static std::unique_ptr<File> file = nullptr;
        inline static AnimatedGIF gif = AnimatedGIF();
        inline static uint16_t usTemp[1][BUFFER_SIZE];    // Global to support DMA use
        inline static bool dmaBuf = 0;
        std::string decodedPath;

};