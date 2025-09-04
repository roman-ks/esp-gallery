#include "png_image.h"
#include "../renderer/renderer.h"

void PNGImage::render(Renderer &rendered) const {
    rendered.render(*this);
}