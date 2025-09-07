#pragma once

#include "png_decoder.h"
#include "../../core/configs.h"


class PNGDownscaleDecoder : public PNGDecoder{
    public:
        PNGDownscaleDecoder();
        virtual ~PNGDownscaleDecoder();

        virtual void decode(const char* filepath, DrawCallbackFunc &drawCallback) override;
    private:
        inline static uint16_t resultBuf[GRID_ELEMENT_WIDTH];
        inline static int16_t lineN = 0;
        static int pngDrawDownscaling(PNGDRAW *pDraw);
};

