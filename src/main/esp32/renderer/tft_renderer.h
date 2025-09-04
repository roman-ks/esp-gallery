// #ifndef __ESP_GALLERY_TFT_RENDERER_H__
// #define __ESP_GALLERY_TFT_RENDERER_H__

// #include "renderer.h"
// #include <TFT_eSPI.h>
// #include <AnimatedGIF.h>
// #include <PNGdec.h>


// class TFTRenderer : public Renderer {
//     public:
//         TFTRenderer(TFT_eSPI &tft) 
//             : tft(tft) {}
//         ~TFTRenderer();

//         void init() override;
//         void reset() override;
//         // void render(const GIFImage &gifImage) override;
//         void render(const PNGImage &pngImage) override;
//     private:
//         TFT_eSPI &tft;
//         AnimatedGIF gif;
//         bool staticImgDrawn = false;

// };
// #endif