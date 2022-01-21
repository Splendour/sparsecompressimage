#pragma once

struct ImageRGB
{
    // Read
    bool loadFromFile(const char* filename);

    // Write
    void init(int width, int height);
    bool saveToFile(const char* filename) const;

    ~ImageRGB();

    int m_width    = 0;
    int m_height   = 0;
    u8* m_data = nullptr;

    constexpr static int RGBChannelCount = 3;
};
