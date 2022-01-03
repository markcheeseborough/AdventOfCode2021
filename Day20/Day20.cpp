#include <algorithm>
#include <array>
#include <iostream>
#include <iterator>
#include <string>
#include <vector>

enum class Pixel
{
    Dark,
    Light
};

class Grid
{
public:
    Grid(int width, int height)
        : width_{ width },
        height_{ height },
        pixels_(height_, std::vector<Pixel>(width_, Pixel::Dark))
    {}

    Pixel At(int x, int y, Pixel def) const noexcept
    {
        if (x < 0 || x >= width_)
            return def;
        if (y < 0 || y >= height_)
            return def;
        return pixels_[y][x];
    }

    void Set(int x, int y, Pixel p)
    {
        pixels_[y][x] = p;
    }

    int Width() const noexcept
    {
        return width_;
    }
    int Height() const noexcept
    {
        return height_;
    }

    void Print(std::ostream& out) const
    {
        for (auto const& row : pixels_)
        {
            std::transform(row.begin(), row.end(), std::ostream_iterator<char>(out), [](Pixel p) { return p == Pixel::Light ? '#' : '.'; });
            out << "\n";
        }
    }

    int LitPixels() const noexcept
    {
        int count = 0;
        for (auto const& row : pixels_)
        {
            count += static_cast<int>(std::count_if(row.begin(), row.end(), [](Pixel p) { return p == Pixel::Light; }));
        }
        return count;
    }

private:
    int width_;
    int height_;
    std::vector<std::vector<Pixel>> pixels_;
};

auto ReadEnhancementKey()
{
    std::string line;
    std::getline(std::cin, line);

    std::vector<Pixel> result(512, Pixel::Dark);

    std::transform(line.begin(), line.end(), result.begin(), [](char c) { return c == '#' ? Pixel::Light : Pixel::Dark; });

    return result;
}

auto ReadInput()
{
    std::vector<std::string> gridData;
    std::string line;
    while (std::getline(std::cin, line))
    {
        if (line.empty() && !gridData.empty())
        {
            break;
        }
        if (line.empty())
        {
            continue;
        }

        gridData.push_back(line);
    }

    int width = static_cast<int>(gridData[0].size());
    int height = static_cast<int>(gridData.size());

    Grid result(width, height);

    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            result.Set(x, y, gridData[y][x] == '#' ? Pixel::Light : Pixel::Dark);
        }
    }

    return result;
}

auto EnhanceGrid(Grid const& grid, std::vector<Pixel> const& enhanceKey, Pixel def)
{
    int newWidth = grid.Width() + 2;
    int newHeight = grid.Height() + 2;

    Grid newGrid(newWidth, newHeight);

    for (int y = 0; y < newHeight; ++y)
    {
        for (int x = 0; x < newWidth; ++x)
        {
            std::array lookup{
                grid.At(x - 2, y - 2, def),
                grid.At(x - 1, y - 2, def),
                grid.At(x, y - 2, def),
                grid.At(x - 2, y - 1, def),
                grid.At(x - 1, y - 1, def),
                grid.At(x, y - 1, def),
                grid.At(x - 2, y, def),
                grid.At(x - 1, y, def),
                grid.At(x, y, def)
            };
            int enhanceIndex = 0;
            for (auto const& p : lookup)
            {
                enhanceIndex <<= 1;
                enhanceIndex += p == Pixel::Light ? 1 : 0;
            }
            newGrid.Set(x, y, enhanceKey[enhanceIndex]);
        }
    }
    return newGrid;
}

int main()
{
    auto enhanceKey = ReadEnhancementKey();

    auto startGrid = ReadInput();

    Pixel def = Pixel::Dark;

    for (int i = 0; i < 50; ++i)
    {
        startGrid = EnhanceGrid(startGrid, enhanceKey, def);
        std::cout << "Total lit pixels after " << i + 1 << " iterations: " << startGrid.LitPixels() << "\n";
        if (enhanceKey.front() == Pixel::Light)
        {
            def = i % 2 == 0 ? enhanceKey.front() : enhanceKey.back();
        }
    }
}
