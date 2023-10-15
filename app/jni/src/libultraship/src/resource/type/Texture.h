#pragma once

#include "resource/Resource.h"
#include "libultraship/libultra/types.h"

#define TEX_FLAG_LOAD_AS_RAW (1 << 0)

namespace LUS {
enum class TextureType {
    Error = 0,
    RGBA32bpp = 1,
    RGBA16bpp = 2,
    Palette4bpp = 3,
    Palette8bpp = 4,
    Grayscale4bpp = 5,
    Grayscale8bpp = 6,
    GrayscaleAlpha4bpp = 7,
    GrayscaleAlpha8bpp = 8,
    GrayscaleAlpha16bpp = 9,
};

class Texture : public Resource<uint8_t> {
  public:
    using Resource::Resource;

    Texture();

    uint8_t* GetPointer() override;
    size_t GetPointerSize() override;

    TextureType Type;
    uint16_t Width, Height;
    uint32_t Flags = 0;
    float HByteScale = 1.0;
    float VPixelScale = 1.0;
    uint32_t ImageDataSize;
    uint8_t* ImageData = nullptr;

    ~Texture();
};
} // namespace LUS
