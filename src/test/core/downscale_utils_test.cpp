#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <cstdint>
#include "../../main/core/downscale_utils.h"
#include "../../../libs/stb/stb_image_write.h"


// Simple base64 decoding table
static const int B64index[256] = {
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,62,0,0,0,63,52,53,54,55,56,57,58,59,60,61,0,0,0,0,0,0,
    0,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,0,0,0,0,0,
    0,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};

// Simple base64 decoder (returns decoded bytes)
std::vector<uint8_t> base64_decode(const std::string& base64) {
    std::vector<uint8_t> out;
    int val = 0, valb = -8;
    for (unsigned char c : base64) {
        if (c == '=' || c > 127) break;
        int d = B64index[c];
        val = (val << 6) + d;
        valb += 6;
        if (valb >= 0) {
            out.push_back((uint8_t)((val >> valb) & 0xFF));
            valb -= 8;
        }
    }
    return out;
}

// Reads a file containing base64, decodes it, and returns a vector of uint16_t
std::vector<uint16_t> readBase64FileToUint16Array(const std::string& filename) {
    // Read file into string
    std::ifstream file(filename, std::ios::in);
    if (!file) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return {};
    }
    std::string base64((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();

    // Remove whitespace (base64 may have newlines)
    base64.erase(std::remove_if(base64.begin(), base64.end(), ::isspace), base64.end());

    // Decode base64
    std::vector<uint8_t> decodedBytes = base64_decode(base64);

    // Convert to uint16_t array (big-endian)
    if (decodedBytes.size() % 2 != 0) {
        std::cerr << "Decoded data length is not a multiple of 2!" << std::endl;
        return {};
    }
    std::vector<uint16_t> result(decodedBytes.size() / 2);
    for (size_t i = 0; i < result.size(); ++i) {
        result[i] = decodedBytes[2 * i] << 8 | (decodedBytes[2 * i + 1]);
    }
    return result;
}

void writeImageToPng(const std::string& filename, int width, int height, const uint16_t* data) {
    // Note: data is in RGB565 format, we need to convert it to RGB888 for PNG
    std::vector<uint8_t> rgb888(width * height * 3);
    for (int i = 0; i < width * height; ++i) {
        uint16_t pixel = data[i];
        uint8_t r8, g8, b8;
        convertRgb565ToRgb888(pixel, r8, g8, b8);
        rgb888[3 * i] = r8;
        rgb888[3 * i + 1] = g8;
        rgb888[3 * i + 2] = b8;
    }
    stbi_write_png(filename.c_str(), width, height, 3, rgb888.data(), width * 3);
}

int main()
{
    std::vector<uint16_t> arr = readBase64FileToUint16Array("resources/base64_tn.txt");
    std::cout << "Decoded array size: " << arr.size() << std::endl;

    IMG_HOLDER testImg{80, 80, arr.data()};
    // IMG_HOLDER tnImg = createThumbnailFloat(&testImg);

    writeImageToPng("output_tn.png", testImg.width, testImg.height, testImg.imageBytes);
    std::cout << "Press Enter to exit..." << std::endl;
    std::cin.get(); // <-- Add this line
    return 0;
}