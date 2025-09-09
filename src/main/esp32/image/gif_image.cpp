#include "gif_image.h"
#include "../renderer/renderer.h"

void GIFImage::render(Renderer &rendered) const {
    rendered.render(*this);
}