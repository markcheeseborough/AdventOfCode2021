#include <algorithm>
#include <array>
#include <compare>
#include <functional>
#include <iostream>
#include <numeric>
#include <regex>
#include <set>
#include <string>
#include <vector>

struct Point
{
    int x;
    int y;
    int z;
    auto operator<=>(const Point&) const = default;
};

Point operator-(Point const& a, Point const& b)
{
    return { a.x - b.x, a.y - b.y, a.z - b.z };
}
Point operator+(Point const& a, Point const& b)
{
    return { a.x + b.x, a.y + b.y, a.z + b.z };
}

Point RotateZ(Point const& p)
{
    return { -p.y, p.x, p.z };
}
Point RotateY(Point const& p)
{
    return { -p.z, p.y, p.x };
}
Point RotateX(Point const& p)
{
    return { p.x, -p.z, p.y };
}

int ManhattenDistance(Point const& a, Point const& b)
{
    return abs(a.x - b.x) + abs(a.y - b.y) + abs(a.z - b.z);
}

struct Scanner
{
    int id{ -1 };
    bool locked{ false };
    Point offset{ 0,0,0 };
    std::vector<Point> points;
};

auto ReadInput()
{
    std::vector<Scanner> result;
    std::string line;

    std::regex scannerIdRegex("--- scanner (\\d+) ---");
    std::regex pointRegex("(-?\\d+),(-?\\d+),(-?\\d+)");

    while (std::getline(std::cin, line))
    {
        if (line.empty())
        {
            break;
        }

        int scannerId = -1;
        std::smatch scannerIdMatch;
        if (std::regex_match(line, scannerIdMatch, scannerIdRegex))
        {
            if (scannerIdMatch.size() == 2)
            {
                scannerId = std::stoi(scannerIdMatch[1].str());
            }
        }
        else
        {
            throw std::runtime_error("Could not identify scanner: " + line);
        }

        Scanner s{ .id = scannerId };
        // now read the points until an empty line
        while (std::getline(std::cin, line))
        {
            if (line.empty())
            {
                break;
            }

            std::smatch pointMatch;
            if (std::regex_match(line, pointMatch, pointRegex))
            {
                if (pointMatch.size() == 4)
                {
                    auto x = std::stoi(pointMatch[1].str());
                    auto y = std::stoi(pointMatch[2].str());
                    auto z = std::stoi(pointMatch[3].str());
                    s.points.emplace_back(x, y, z);
                }
            }
            else
            {
                throw std::runtime_error("Could not identify point: " + line);
            }
        }
        result.push_back(s);
    }
    return result;
}

int main()
{
    auto scanners = ReadInput();

    std::set<Point> lockedPoints{ scanners[0].points.begin(), scanners[0].points.end() };
    scanners[0].locked = true;

    std::array<std::function<Point(Point const&)>, 24> rotations = {
        // assume we start facing +x and try each way up (rotate around x-axis)
        [](Point const& p) { return p; },
        [](Point const& p) { return RotateX(p); },
        [](Point const& p) { return RotateX(RotateX(p)); },
        [](Point const& p) { return RotateX(RotateX(RotateX(p))); },
        //  now point to -x
        [](Point const& p) { return RotateY(RotateY(p)); },
        [](Point const& p) { return RotateX(RotateY(RotateY(p))); },
        [](Point const& p) { return RotateX(RotateX(RotateY(RotateY(p)))); },
        [](Point const& p) { return RotateX(RotateX(RotateX(RotateY(RotateY(p))))); },
        // now to facing along z-axis
        [](Point const& p) { return RotateY(p); },
        [](Point const& p) { return RotateZ(RotateY(p)); },
        [](Point const& p) { return RotateZ(RotateZ(RotateY(p))); },
        [](Point const& p) { return RotateZ(RotateZ(RotateZ(RotateY(p)))); },
        // and the other way on the z-axis
        [](Point const& p) { return RotateY(RotateY(RotateY(p))); },
        [](Point const& p) { return RotateZ(RotateY(RotateY(RotateY(p)))); },
        [](Point const& p) { return RotateZ(RotateZ(RotateY(RotateY(RotateY(p))))); },
        [](Point const& p) { return RotateZ(RotateZ(RotateZ(RotateY(RotateY(RotateY(p)))))); },
        // now facing along the y-axis
        [](Point const& p) { return RotateZ(p); },
        [](Point const& p) { return RotateY(RotateZ(p)); },
        [](Point const& p) { return RotateY(RotateY(RotateZ(p))); },
        [](Point const& p) { return RotateY(RotateY(RotateY(RotateZ(p)))); },
        // and the other way along the y-axis
        [](Point const& p) { return RotateZ(RotateZ(RotateZ(p))); },
        [](Point const& p) { return RotateY(RotateZ(RotateZ(RotateZ(p)))); },
        [](Point const& p) { return RotateY(RotateY(RotateZ(RotateZ(RotateZ(p))))); },
        [](Point const& p) { return RotateY(RotateY(RotateY(RotateZ(RotateZ(RotateZ(p)))))); },
    };

    while (std::any_of(scanners.begin(), scanners.end(), [](auto const& s) { return !s.locked; }))
    {
        for (auto& s : scanners)
        {
            if (!s.locked)
            {
                bool didLock = false;
                //for (auto const& r : rotations)
                for (auto rotIt = rotations.begin(); rotIt != rotations.end() && !didLock; ++rotIt)
                {
                    auto const& r = *rotIt;
                    //for (auto const& lp : lockedPoints)
                    for (auto lpIt = lockedPoints.begin(); lpIt != lockedPoints.end() && !didLock; ++lpIt)
                    {
                        auto const& lp = *lpIt;
                        //for (auto const& sp : s.points)
                        for (auto spIt = s.points.begin(); spIt != s.points.end() && !didLock; ++spIt)
                        {
                            auto const& sp = *spIt;
                            auto offset = lp - r(sp);
                            // check how many points from the scanner line up with locked points
                            auto overlapCount = std::count_if(s.points.begin(), s.points.end(), [&lockedPoints, &offset, &r](auto const& p) { return lockedPoints.contains(offset + r(p)); });
                            if (overlapCount >= 12)
                            {
                                std::cout << "Locking scanner " << s.id << "\n";
                                didLock = true;
                                s.locked = true;
                                s.offset = offset;
                                std::transform(s.points.begin(), s.points.end(), std::inserter(lockedPoints, lockedPoints.begin()), [&offset, &r](auto const& p) { return offset + r(p); });
                            }
                        }
                    }
                }
            }
        }
    }

    std::cout << "Total number of points after aligning: " << lockedPoints.size() << "\n";

    int largestManhattenDistance = 0;
    for (int i = 0; i < scanners.size() - 1; ++i)
    {
        for (int j = i + 1; j < scanners.size(); ++j)
        {
            auto distance = ManhattenDistance(scanners[i].offset, scanners[j].offset);
            if (distance > largestManhattenDistance)
            {
                largestManhattenDistance = distance;
            }
        }
    }

    std::cout << "Largest Manhatten Distance between scanners: " << largestManhattenDistance << "\n";
}
