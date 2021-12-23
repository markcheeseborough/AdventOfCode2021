#include <algorithm>
#include <iostream>
#include <numeric>
#include <vector>

std::vector<int> ReadCrabPositions()
{
    std::vector<int> result;
    while (std::cin)
    {
        int val;
        std::cin >> val;
        result.push_back(val);
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

void PartOne(std::vector<int> const& crabPositions)
{
    // get the median of the positions

    int median = 0;
    if (crabPositions.size() % 2 == 0)
    {
        auto mid1 = crabPositions[crabPositions.size() / 2 - 1];
        auto mid2 = crabPositions[crabPositions.size() / 2];
        median = (mid1 + mid2) / 2;
    }
    else
    {
        median = crabPositions[crabPositions.size() / 2];
    }

    // now sum the distances to the median
    int sumDistances = 0;
    for (auto e : crabPositions)
    {
        sumDistances += std::abs(e - median);
    }

    std::cout << "The Part 1 fuel cost to align on " << median << " is " << sumDistances << " units.\n";
}

void PartTwo(std::vector<int> const& crabPositions)
{
    int maxPos = *std::max_element(crabPositions.begin(), crabPositions.end());

    int bestFuel = std::numeric_limits<int>::max();
    for (int i = 0; i < maxPos; ++i)
    {
        std::vector<int> fuelCosts;
        fuelCosts.reserve(crabPositions.size());

        std::transform(crabPositions.begin(), crabPositions.end(), std::back_inserter(fuelCosts),
            [i](int pos)
            {
                int dist = std::abs(pos - i);
                return dist * (dist + 1) / 2;
            });
        auto totalCost = std::accumulate(fuelCosts.begin(), fuelCosts.end(), 0);
        if (totalCost < bestFuel)
        {
            bestFuel = totalCost;
        }
        else if (totalCost > bestFuel)
        {
            break;
        }
    }
    std::cout << "The part 2 fuel cost is " << bestFuel << " units.\n";
}

int main()
{
    auto crabPositions = ReadCrabPositions();

    std::sort(crabPositions.begin(), crabPositions.end());

    PartOne(crabPositions);
    PartTwo(crabPositions);
}

