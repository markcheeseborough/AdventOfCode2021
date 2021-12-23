#include <algorithm>
#include <iostream>
#include <set>
#include <stack>
#include <string>
#include <sstream>
#include <vector>

struct Point
{
    int x;
    int y;
    auto operator<=>(const Point&) const = default;
};

auto ReadInput()
{
    std::vector<std::vector<int>> result;

    std::string inLine;
    while (std::getline(std::cin, inLine))
    {
        if (inLine.empty())
            break;

        std::stringstream in{ inLine };
        std::vector<int> row{ std::istreambuf_iterator<char>{in}, {} };
        std::transform(row.begin(), row.end(), row.begin(), [](int v) {return v - '0'; });
        result.push_back(row);
    }

    return result;
}

auto FindLowPoints(std::vector<std::vector<int>> const& grid)
{
    std::vector<Point> result;

    for (int y = 0; y < grid.size(); ++y)
    {
        for (int x = 0; x < grid[y].size(); ++x)
        {
            int curVal = grid[y][x];
            bool lowerThanNorth = y == 0 || curVal < grid[y - 1][x];
            bool lowerThanSouth = y == (grid.size() - 1) || curVal < grid[y + 1][x];
            bool lowerThanWest = x == 0 || curVal < grid[y][x - 1];
            bool lowerThanEast = x == (grid[y].size() - 1) || curVal < grid[y][x + 1];

            if (lowerThanNorth && lowerThanSouth && lowerThanWest && lowerThanEast)
            {
                result.emplace_back(x, y);
            }
        }
    }
    return result;
}

auto SumRiskLevels(std::vector<std::vector<int>> const& grid, std::vector<Point> const& lowPoints)
{
    int riskScore = 0;
    for (auto const& p : lowPoints)
    {
        riskScore += 1 + grid[p.y][p.x];
    }
    return riskScore;
}

auto GetBasinSize(std::vector<std::vector<int>> const& grid, Point const& p)
{
    std::stack<Point> pointsToVisit;
    std::set<Point> basinPoints;

    pointsToVisit.push(p);
    basinPoints.insert(p);

    while (!pointsToVisit.empty())
    {
        auto curPoint = pointsToVisit.top();
        pointsToVisit.pop();

        if (curPoint.x > 0 && grid[curPoint.y][curPoint.x - 1] != 9)
        {
            Point newP{ curPoint.x - 1, curPoint.y };
            if (!basinPoints.contains(newP))
            {
                basinPoints.insert(newP);
                pointsToVisit.push(newP);
            }
        }
        if (curPoint.x < grid[0].size() - 1 && grid[curPoint.y][curPoint.x + 1] != 9)
        {
            Point newP{ curPoint.x + 1, curPoint.y };
            if (!basinPoints.contains(newP))
            {
                basinPoints.insert(newP);
                pointsToVisit.push(newP);
            }
        }
        if (curPoint.y > 0 && grid[curPoint.y - 1][curPoint.x] != 9)
        {
            Point newP{ curPoint.x, curPoint.y - 1 };
            if (!basinPoints.contains(newP))
            {
                basinPoints.insert(newP);
                pointsToVisit.push(newP);
            }
        }
        if (curPoint.y < grid.size() - 1 && grid[curPoint.y + 1][curPoint.x] != 9)
        {
            Point newP{ curPoint.x, curPoint.y + 1 };
            if (!basinPoints.contains(newP))
            {
                basinPoints.insert(newP);
                pointsToVisit.push(newP);
            }
        }
    }
    return basinPoints.size();
}

auto GetAllBasinSizes(std::vector<std::vector<int>> const& grid, std::vector<Point> const& lowPoints)
{
    std::vector<int> basinSizes;
    for (auto const& p : lowPoints)
    {
        auto basinSize = GetBasinSize(grid, p);
        basinSizes.push_back(basinSize);
    }
    return basinSizes;
}

int main()
{
    auto grid = ReadInput();

    auto lowPoints = FindLowPoints(grid);

    auto riskScore = SumRiskLevels(grid, lowPoints);

    std::cout << "Sum of risk levels is " << riskScore << "\n";

    auto basinSizes = GetAllBasinSizes(grid, lowPoints);

    std::sort(basinSizes.begin(), basinSizes.end(), std::greater{});

    int threeLargest = basinSizes[0] * basinSizes[1] * basinSizes[2];

    std::cout << "Product of 3 largest basin sizes is " << threeLargest << "\n";
}
