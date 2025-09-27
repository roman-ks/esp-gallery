from PIL import Image


def rgb888_to_rgb565(r, g, b):
    """Convert 8-bit RGB to 16-bit RGB565 and swap bytes for big-endian"""
    val = ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3)
    # Swap bytes: high byte first
    return ((val & 0xFF) << 8) | (val >> 8)


def png_to_rgb565_array(png_path):
    img = Image.open(png_path).convert("RGB")
    width, height = img.size
    print(f"Image size: ${width}, ${height}")

    pixels = list(img.getdata())

    # Convert each pixel to RGB565
    rgb565 = [rgb888_to_rgb565(r, g, b) for r, g, b in pixels]
    print(f"Pixels: ${len(rgb565)}")

    cpp_lines = []
    line = ""
    # Format output as C++ array
    for i, val in enumerate(rgb565):
        line += f"0x{val:04X},"
        if (i + 1) % width == 0:
            cpp_lines.append(line)
            line = ""


    return "\n".join(cpp_lines)


if __name__ == "__main__":
    # Replace with your PNG path
    png_path = "play_icon.png"
    cpp_code = png_to_rgb565_array(png_path)
    with open("rgb565_pixels.txt", "w") as f:
        for line in cpp_code:
            f.write(line)
