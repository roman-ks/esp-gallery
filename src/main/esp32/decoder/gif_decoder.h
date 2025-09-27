#pragma once

#include "../decoder/advancable_decoder.h"
#include <FS.h>
#include <AnimatedGIF.h>
#define BUFFER_SIZE 256            // Optimum is >= GIF width or integral division of width
#include "../../core/configs.h"
#include <string>


class GIFDecoder: public AdvancableDecoder {

    public:
        GIFDecoder();
        GIFDecoder(std::vector<DelegatingDrawTargetFactory> &delegatingFactories);
        ~GIFDecoder()=default;

        void init() override;
        void decode(const std::string &filepath, IDrawTarget &iDrawTarget) override;
        void advance(IDrawTarget &iDrawTarget) override;
        
        static void * GIFOpenFile(const char *filename, int32_t *pSize);
        static void GIFCloseFile(void *pHandle);
        static int32_t GIFReadFile(GIFFILE *pFile, uint8_t *pBuf, int32_t iLen);
        static int32_t GIFSeekFile(GIFFILE *pFile, int32_t iPosition);
        static void GIFDraw(GIFDRAW *pDraw);

        static void setIDrawTarget(IDrawTarget *target){
            iDrawTarget = target;
        }

    private:
        inline static IDrawTarget *iDrawTarget = nullptr;
        inline static std::unique_ptr<File> file = nullptr;
        inline static AnimatedGIF gif = AnimatedGIF();
        inline static uint16_t usTemp[1][BUFFER_SIZE];    // Global to support DMA use
        inline static bool dmaBuf = 0;
        std::string decodedPath;

};