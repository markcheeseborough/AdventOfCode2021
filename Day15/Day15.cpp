#include <algorithm>
#include <iostream>
#include <map>
#include <string>
#include <sstream>
#include <vector>

using Grid = std::vector<std::vector<int>>;

struct Point
{
    int x;
    int y;
    auto operator<=>(const Point&) const = default;
};
auto DistanceBetween(Point a, Point b)
{
    auto xDist = b.x - a.x;
    auto yDist = b.y - a.y;

    auto distSqrd = (xDist * xDist) + (yDist * yDist);
    auto distance = sqrt(distSqrd);
    return static_cast<int>(distance);
}


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

struct Node
{
    Point position;
    Point parentPosition;
    int fScore;
    int hScore;
    int gScore;
};

auto Neighbours(Grid const& map, Point pos)
{
    std::vector<Point> result;

    if (pos.x > 0)
    {
        result.emplace_back(pos.x - 1, pos.y);
    }
    if (pos.x < map[0].size() - 1)
    {
        result.emplace_back(pos.x + 1, pos.y);
    }
    if (pos.y > 0)
    {
        result.emplace_back(pos.x, pos.y - 1);
    }
    if (pos.y < map.size() - 1)
    {
        result.emplace_back(pos.x, pos.y + 1);
    }
    return result;
}

auto FindPath(Grid const& map, Point start, Point end)
{
    std::vector<Node> openSet;
    std::map<Point, Point> closedSet;
    auto foundEnd = false;

    openSet.emplace_back(start, Point{ -1, -1 }, 0, DistanceBetween(start, end), 0);

    while (!foundEnd && !openSet.empty())
    {
        auto currentIt = std::min_element(openSet.begin(), openSet.end(),
            [](auto const& a, auto const& b)
            {
                return a.fScore < b.fScore;
            });
        auto current = *currentIt;
        openSet.erase(currentIt);
        closedSet.insert(std::make_pair(current.position, current.parentPosition));

        if (current.position == end)
        {
            foundEnd = true;
            break;
        }

        auto neighbours = Neighbours(map, current.position);
        for (auto const& n : neighbours)
        {
            if (closedSet.contains(n))
            {
                // already checked this one
                continue;
            }

            int moveScore = map[n.y][n.x];
            int gScore = current.gScore + moveScore;

            auto it = std::find_if(openSet.begin(), openSet.end(),
                [&n](auto const& node)
                {
                    return node.position == n;
                });
            if (it != openSet.end())
            {
                if (it->gScore > gScore)
                {
                    // path through current node is the better way to position n
                    it->gScore = gScore;
                    it->fScore = gScore + it->hScore;
                    it->parentPosition = current.position;
                }
            }
            else
            {
                auto hScore = DistanceBetween(n, end);
                openSet.emplace_back(n, current.position, gScore + hScore, hScore, gScore);
            }
        }
    }

    std::vector<Point> path;
    if (foundEnd)
    {
        auto curPos = end;
        auto parentPos = closedSet[curPos];

        while (parentPos != Point{ -1, -1 })
        {
            path.insert(path.begin(), curPos);

            curPos = parentPos;
            parentPos = closedSet[curPos];
        }
    }
    return path;
}

auto ScorePath(Grid const& map, std::vector<Point> const& path)
{
    int score = 0;
    for (auto const& p : path)
    {
        score += map[p.y][p.x];
    }
    return score;
}

auto IncrementVector(std::vector<int> v)
{
    std::transform(v.begin(), v.end(), v.begin(),
        [](int i) { return i == 9 ? 1 : i + 1; });
    return v;
}

auto ExpandRow(std::vector<int> row)
{
    std::vector<int> result = row;

    for (int i = 0; i < 4; ++i)
    {
        row = IncrementVector(std::move(row));
        result.insert(result.end(), row.begin(), row.end());
    }
    return result;
}

auto ExpandMap(Grid const& map)
{
    Grid newMap;
    auto originalHeight = map.size();

    for (auto const& r : map)
    {
        auto newRow = ExpandRow(r);
        newMap.push_back(newRow);
    }

    for (int i = 0; i < 4; ++i)
    {
        for (int j = 0; j < originalHeight; ++j)
        {
            auto newRow = IncrementVector(newMap[(i * originalHeight) + j]);
            newMap.push_back(newRow);
        }
    }

    return newMap;
}

int main()
{
    auto map = ReadInput();
    auto mapWidth = static_cast<int>(map[0].size());
    auto mapHeight = static_cast<int>(map.size());

    auto path = FindPath(map, { 0, 0 }, { mapWidth - 1, mapHeight - 1 });

    auto riskScore = ScorePath(map, path);

    std::cout << "Part one risk score of path: " << riskScore << "\n";

    auto bigMap = ExpandMap(map);

    auto bigPath = FindPath(bigMap, { 0, 0 }, { (mapWidth * 5) - 1, (mapHeight * 5) - 1 });
    auto bigRiskScore = ScorePath(bigMap, bigPath);

    std::cout << "Part two risk score of path: " << bigRiskScore << "\n";
}
