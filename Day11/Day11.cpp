#include <algorithm>
#include <array>
#include <iostream>
#include <set>
#include <sstream>
#include <string>
#include <vector>

using Grid = std::vector<std::vector<int>>;

auto ReadInput()
{
    Grid result;

    std::string inLine;
    while (std::getline(std::cin, inLine))
    {
        if (inLine.empty())
            break;

        std::stringstream in{ inLine };
        std::vector<int> row{ std::istreambuf_iterator<char>{ in }, {} };
        std::transform(row.begin(), row.end(), row.begin(), [](int v) { return v - '0'; });
        result.push_back(row);
    }

    return result;
}

void IncreaseEnergy(Grid& grid)
{
    for (auto& row : grid)
    {
        for (auto& octopus : row)
        {
            ++octopus;
        }
    }
}

void ResetEnergy(Grid& grid)
{
    for (auto& row : grid)
    {
        for (auto& octopus : row)
        {
            if (octopus > 9)
            {
                octopus = 0;
            }
        }
    }
}

uint64_t DoFlashes(Grid& grid)
{
    std::set<std::pair<int, int>> hasFlashed{};

    while (true)
    {
        Grid flashEnergy(10, std::vector<int>(10, 0));
        int flashesThisPass = 0;
        for (int row = 0; row < 10; ++row)
        {
            for (int col = 0; col < 10; ++col)
            {
                if (grid[row][col] > 9 && !hasFlashed.contains(std::make_pair(row, col)))
                {
                    // flash
                    ++flashesThisPass;
                    hasFlashed.insert(std::make_pair(row, col));

                    for (int i = std::max(0, row - 1); i < std::min(row + 2, 10); ++i)
                    {
                        for (int j = std::max(0, col - 1); j < std::min(col + 2, 10); ++j)
                        {
                            ++flashEnergy[i][j];
                        }
                    }
                }
            }
        }

        for (int row = 0; row < 10; ++row)
        {
            for (int col = 0; col < 10; ++col)
            {
                grid[row][col] += flashEnergy[row][col];
            }
        }

        if (flashesThisPass == 0)
        {
            break;
        }
    }

    return hasFlashed.size();
}

int main()
{
    auto octopuses = ReadInput();

    uint64_t totalFlashes = 0;

    bool allFlashed = false;
    for (int i = 0; !allFlashed; ++i)
    {
        IncreaseEnergy(octopuses);
        auto flashesThisStep = DoFlashes(octopuses);
        totalFlashes += flashesThisStep;
        ResetEnergy(octopuses);

        if (flashesThisStep == 100)
        {
            allFlashed = true;
            std::cout << "All octopuses flashed on step " << i + 1 << "\n";
        }

        if (i == 99)
        {
            std::cout << "Total octopus flashes on step 100: " << totalFlashes << "\n";
        }
    }

}
