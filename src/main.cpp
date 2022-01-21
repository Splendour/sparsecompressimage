#include "common.h"

#include "image.h"

#ifdef _DEBUG
#define DISPLAY_TILES
#endif

struct Pixel
{
    u8 r;
    u8 g;
    u8 b;
};

bool operator==(const Pixel& x, const Pixel& y)
{
    return x.r == y.r && x.g == y.g && x.b == y.b;
}

bool operator!= (const Pixel& x, const Pixel& y)
{
    return !(x == y);
}

struct Tile
{
    static constexpr uint Dims = 8;
    Pixel m_pixels[Dims][Dims];
};

bool operator==( const Tile& a, const Tile& b) 
{
    for (uint y = 0; y < Tile::Dims; ++y)
        for (uint x = 0; x < Tile::Dims; ++x)
            if (a.m_pixels[y][x] != b.m_pixels[y][x])
                return false;

    return true;
}

struct CompressedImage
{
    vector<int> m_tileHandle;
    vector<Tile> m_tiles;

    uint m_width, m_height;

#ifdef DISPLAY_TILES
    vector<bool> m_tileIsDuplicate;
#endif

};

CompressedImage compress(const ImageRGB& image)
{
    uint tilesX = (image.m_width + Tile::Dims - 1) / Tile::Dims;
    uint tilesY = (image.m_height + Tile::Dims - 1) / Tile::Dims;
    CompressedImage compressed;
    compressed.m_width = image.m_width;
    compressed.m_height = image.m_height;

    uint tilesCount = tilesX * tilesY;
    compressed.m_tileHandle.resize(tilesCount);
    compressed.m_tiles.reserve(tilesCount);
#ifdef DISPLAY_TILES
    compressed.m_tileIsDuplicate.resize(tilesCount);
#endif

    const Pixel* imgData = reinterpret_cast<const Pixel*>(image.m_data);
    for (uint y = 0; y < tilesY; ++y)
        for (uint x = 0; x < tilesX; ++x)
        {
            Tile t{};
            for (uint inTileY = 0; inTileY < Tile::Dims; ++inTileY)
            {
                for (uint inTileX = 0; inTileX < Tile::Dims; ++inTileX)
                {
                    uint imgX = inTileX + x * Tile::Dims;
                    uint imgY = inTileY + y * Tile::Dims;
                    if (imgX < (uint)image.m_width && imgY < (uint)image.m_height)
                        t.m_pixels[inTileY][inTileX] = imgData[image.m_width * imgY + imgX];
                }
            }
            uint tileIndex = y * tilesX + x;
            auto it = std::find(compressed.m_tiles.begin(), compressed.m_tiles.end(), t);
            bool found = it != compressed.m_tiles.end();
            if (found)
            {
                uint foundAtIndex = (uint)std::distance(compressed.m_tiles.begin(), it);
                compressed.m_tileHandle[tileIndex] = foundAtIndex;
            }
            else
            {
                compressed.m_tiles.push_back(t);
                compressed.m_tileHandle[tileIndex] = (uint)(compressed.m_tiles.size() - 1);
            }
#ifdef DISPLAY_TILES
            compressed.m_tileIsDuplicate[tileIndex] = found;
#endif
        }

    return compressed;
}

void decompress(const CompressedImage& compressed, ImageRGB& image)
{
    image.m_height = compressed.m_height;
    image.m_width = compressed.m_width;
    image.m_data = (u8*)malloc(image.m_width * image.m_height * sizeof(Pixel));
    Pixel* imgData = reinterpret_cast<Pixel*>(image.m_data);

    uint tilesX = (image.m_width + Tile::Dims - 1) / Tile::Dims;

    for (uint y = 0; y < (uint)image.m_height; ++y)
    {
        for (uint x = 0; x < (uint)image.m_width; ++x)
        {
            uint tileIndexX = x / Tile::Dims;
            uint tileIndexY = y / Tile::Dims;
            uint tileIndex = tileIndexY * tilesX + tileIndexX;

            uint tileHandle = compressed.m_tileHandle[tileIndex];
            const Tile& tile = compressed.m_tiles[tileHandle];

            uint inTileX = x % Tile::Dims;
            uint inTileY = y % Tile::Dims;

            Pixel p = tile.m_pixels[inTileY][inTileX];

#ifdef DISPLAY_TILES
            if (compressed.m_tileIsDuplicate[tileIndex] && (inTileX == 0 || inTileY == 0))
                p = { 0, 255, 0 };
#endif

            imgData[y * image.m_width + x] = p;
        }
    }
}

int main()
{
    ImageRGB imageOut;
    {
        ImageRGB imageIn;
        bool loaded = imageIn.loadFromFile("C:/pics/compress/in.jpg");
        assert(loaded);

        CompressedImage comp = compress(imageIn);
        uint sizeIn = imageIn.m_width * imageIn.m_height * ImageRGB::RGBChannelCount;
        uint sizeOut = (uint)(comp.m_tileHandle.size() * sizeof(int) + comp.m_tiles.size() * sizeof(Tile));
        uint maxTilesX = (imageIn.m_width + Tile::Dims - 1) / Tile::Dims;
        uint maxTilesY = (imageIn.m_height + Tile::Dims - 1) / Tile::Dims;
        uint duplicateTiles = maxTilesX * maxTilesY - (uint)comp.m_tiles.size();
        fmt::print("Image {}x{}, Size: {}\n", imageIn.m_width, imageIn.m_height, sizeIn);
        fmt::print("Tiles: {}, Duplicate: {}, Size: {}\n", comp.m_tiles.size(), duplicateTiles, sizeOut);
        fmt::print("Saved {:.2f}% memory!\n", (1 - double(sizeOut) / double(sizeIn)) * 100);
        decompress(comp, imageOut);
    }

    bool saved = imageOut.saveToFile("C:/pics/compress/out.jpg");
    assert(saved);

    fmt::print("Done!");
}
