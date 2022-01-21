#include "common.h"

#include "image.h"

//#define STBI_NO_HDR
#define STB_IMAGE_IMPLEMENTATION
#define STBI_MALLOC(sz)           malloc(sz)
#define STBI_REALLOC(p,newsz)     realloc(p,newsz)
#define STBI_FREE(p)              free(p)

#include "extern/stb/stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "extern/stb/stb_image_write.h"

bool ImageRGB::loadFromFile(const char* filename)
{
    int channels = 0;
    m_data = stbi_load(filename, &m_width, &m_height, &channels, RGBChannelCount);

    if (!m_data)
    {
        fmt::print("Could not load image {}", filename);
        return false;
    }

    if (channels != RGBChannelCount)
        fmt::print("Image {} has {} channels but we loaded {}", filename, channels, RGBChannelCount);    
    
    return true;
}

bool ImageRGB::saveToFile(const char* filename) const
{
    // quality is between 1 and 100.
    int quality = 100;
    int retCode = stbi_write_jpg(filename, m_width, m_height, RGBChannelCount, m_data, quality);

    if (!retCode)
        fmt::print("Error writing to {}", filename);

    return retCode;
}

ImageRGB::~ImageRGB()
{
    if (m_data)
        stbi_image_free(m_data);
}

void ImageRGB::init(int width, int height)
{
    m_width = width;
    m_height = height;

    if (m_data)
    {
        assert(false && "Double init image");
        stbi_image_free(m_data);
    }

    m_data = (u8*)STBI_MALLOC(m_width * m_height * ImageRGB::RGBChannelCount * sizeof(u8));
}
