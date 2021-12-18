// Day1.cpp : Defines the entry point for the application.
//

#include "Day1.h"

std::vector<int> ReadInput()
{
    std::vector<int> input;

    std::string text;
    while (std::getline(std::cin, text))
    {
        if (text.empty())
        {
            continue;
        }
        int val = std::stoi(text);
        input.push_back(val);
    }
    return input;
}

auto CountIncreases(std::vector<int> const& data)
{
    std::vector<int> differences(data.size());

    std::adjacent_difference(std::begin(data), std::end(data), std::begin(differences));

    auto increases = std::count_if(std::next(std::begin(differences)), std::end(differences),
        [](int v) { return v > 0; });

    return increases;
}

auto SumSlidingWindow(std::vector<int> const& data)
{
    std::vector<int> windowSums;
    windowSums.reserve(data.size() - 2);

    for (size_t i = 0; i < data.size() - 2; ++i)
    {
        windowSums.push_back(data[i] + data[i + 1] + data[i + 2]);
    }

    return windowSums;
}

int main()
{
    auto input = ReadInput();

    auto increases = CountIncreases(input);

    std::cout << "Depth increased " << increases << " times.\n";

    auto slidingWindows = SumSlidingWindow(input);
    auto windowIncreases = CountIncreases(slidingWindows);

    std::cout << "Sliding Window Depth increased " << windowIncreases << " times.\n";
}
