// #include "png_decoder.h"

// PNGDecoder::PNGDecoder() {
// }
// PNGDecoder::~PNGDecoder() {
//     if(file){
//         file.close();
//     }
// }
// void PNGDecoder::init() {
//     // Initialize PNG decoder if needed
// }

// void PNGDecoder::decode(char* filepath, std::function<int(void*)> drawCallback, void* holder) {

//     PNG &png = *static_cast<PNG*>(holder);
//     this->drawCallback = drawCallback;
//     Serial.println("Opening image..");
//     int16_t rc = png.open(strname.c_str(), pngOpen, pngClose, pngRead, pngSeek, pngDraw);
//     if (rc == PNG_SUCCESS) {
//         Serial.printf("image specs: (%d x %d), %d bpp, pixel type: %d\n", png.getWidth(), png.getHeight(), png.getBpp(), png.getPixelType());
//         uint32_t dt = millis();
//         if (png.getWidth() > MAX_IMAGE_WIDTH) {
//           Serial.println("Image too wide for allocated line buffer size!");
//         }
//         else {
//           rc = png.decode(NULL, 0);
//           png.close();
//         }
//         // How long did rendering take...
//         Serial.print(millis()-dt); Serial.println("ms");
//     }
//     this->drawCallback = nullptr;
// }

// void * PNGDecoder::pngOpen(const char *filename, int32_t *size) {
//   Serial.printf("Attempting to open %s\n", filename);
//   pngfile = FileSys.open(filename, "r");
//   *size = pngfile.size();
//   return &pngfile;
// }

// void PNGDecoder::pngClose(void *handle) {
//   File pngfile = *((File*)handle);
//   if (pngfile) pngfile.close();
// }

// int32_t PNGDEcoder::pngRead(PNGFILE *page, uint8_t *buffer, int32_t length) {
//   if (!pngfile) return 0;
//   page = page; // Avoid warning
//   return pngfile.read(buffer, length);
// }

// int32_t PNGDecoder::pngSeek(PNGFILE *page, int32_t position) {
//   if (!pngfile) return 0;
//   page = page; // Avoid warning
//   return pngfile.seek(position);
// }

// int32_t PNGDecoder::pngDraw(PNGDRAW *pDraw) {
//   if (!pngfile) return 0;
//   if (drawCallback) {
//       return drawCallback((void*)pDraw);
//   }
//   return 0;
// }

