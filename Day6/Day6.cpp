#include <algorithm>
#include <array>
#include <iostream>
#include <list>
#include <vector>

std::array<uint64_t, 9> ReadFishTimers()
{
    std::array<uint64_t, 9> result = { 0 };
    while (std::cin)
    {
        int val;
        std::cin >> val;
        result[val]++;
        char ignore;
        if (std::cin.peek() == ',')
        {
            std::cin >> ignore;
        }
        else
        {
            break;
        }
    }
    return result;
}

int main()
{
    auto fishTimers = ReadFishTimers();

    for (int day = 0; day < 256; ++day)
    {
        auto fishToAdd = fishTimers[0];

        for (int i = 0; i < 8; ++i)
        {
            fishTimers[i] = fishTimers[i + 1];
        }

        fishTimers[6] += fishToAdd;
        fishTimers[8] = fishToAdd;

        uint64_t totalFish = 0;
        for (int i = 0; i < 9; ++i)
        {
            totalFish += fishTimers[i];
        }
        std::cout << "After " << day + 1 << " days there are " << totalFish << " fish.\n";
    }
}
