#pragma once

#include "../decoder/advancable_decoder.h"
#include <FS.h>
#include <AnimatedGIF.h>
#define BUFFER_SIZE 256            // Optimum is >= GIF width or integral division of width
#include "../../core/configs.h"
#include <string>


class GIFDecoder: public AdvancableDecoder {

    public:
        GIFDecoder(): GIFDecoder(nullptr, nullptr) {}
        GIFDecoder(DelegatingDrawTarget *firstDelegate, DelegatingDrawTarget *lastDelegate);
        ~GIFDecoder()=default;

        void decode(uint8_t *fileBytes, size_t bytesCount, IDrawTarget &iDrawTarget) override;
        void advance(IDrawTarget &iDrawTarget) override;
        
        static void GIFDraw(GIFDRAW *pDraw);

        static void setIDrawTarget(IDrawTarget *target){
            iDrawTarget = target;
        }

    private:
        inline static IDrawTarget *iDrawTarget = nullptr;
        inline static AnimatedGIF gif = AnimatedGIF();
        inline static uint16_t usTemp[1][BUFFER_SIZE];    // Global to support DMA use
        inline static bool dmaBuf = 0;
        inline static bool valid = false; 
        // std::string decodedPath;

};