#include <iostream>
#include <regex>
#include <string>

struct Point
{
    int x;
    int y;
    auto operator<=>(const Point&) const = default;
};

struct BoundingBox
{
    BoundingBox(Point pos, Point s)
        : position{ pos },
        size{ s }
    {}

    Point const position;
    Point const size;

    bool Contains(Point const& p) const noexcept
    {
        auto inWidth = position.x <= p.x && p.x <= (position.x + size.x);
        auto inHeight = position.y <= p.y && p.y <= (position.y + size.y);
        return inWidth && inHeight;
    }
};

auto ReadInput()
{
    std::string line;
    std::getline(std::cin, line);

    std::regex regex(R"(target area: x=(\d+)\.\.(\d+), y=(-?\d+)\.\.(-?\d+))");

    std::smatch matches;
    if (std::regex_match(line, matches, regex))
    {
        if (matches.size() == 5)
        {
            auto x1 = std::stoi(matches[1].str());
            auto x2 = std::stoi(matches[2].str());
            auto y1 = std::stoi(matches[3].str());
            auto y2 = std::stoi(matches[4].str());

            return BoundingBox{ Point{x1, y1}, Point{x2 - x1, y2 - y1} };
        }
    }
    throw std::runtime_error("Input not recognised");
}

bool HasOverShot(BoundingBox const& box, Point const& p)
{
    if (p.x > box.position.x + box.size.x)
        return true;
    if (p.y < box.position.y)
        return true;
    return false;
}

std::pair<bool, int> TryVelocity(BoundingBox const& targetField, Point const & startVelocity)
{
    Point position{ 0,0 };
    Point velocity = startVelocity;
    int maxHeight = 0;

    while (!targetField.Contains(position) && !HasOverShot(targetField, position))
    {
        position.x += velocity.x;
        position.y += velocity.y;

        if (position.y > maxHeight)
        {
            maxHeight = position.y;
        }

        if (velocity.x > 0)
        {
            velocity.x -= 1;
        }
        else if (velocity.x < 0)
        {
            velocity.x += 1;
        }

        velocity.y -= 1;

    }
    if (targetField.Contains(position))
    {
        //std::cout << "Start velocity: (" << startVelocity.x << ", " << startVelocity.y << ")\n";
        //std::cout << "Reached target area! Maximum height reached: " << maxHeight << " \n";
        return { true, maxHeight };
    }
    else
    {
        return { false, maxHeight };
    }
}

int main()
{
    auto targetField = ReadInput();

    int maxHorizontalVelocity = targetField.position.x + targetField.size.x;
    int minVerticalVelocity = targetField.position.y;
    int maxVerticalVelocity = std::abs(targetField.position.y);

    int maxHeight = 0;
    int totalHits = 0;
    for (int x = 0; x <= maxHorizontalVelocity; ++x)
    {
        for (int y = minVerticalVelocity; y <= maxVerticalVelocity; ++y)
        {
            auto [hit, height] = TryVelocity(targetField, { x,y });
            if (hit)
            {
                ++totalHits;
                if (height > maxHeight)
                {
                    maxHeight = height;
                }
            }
        }
    }
    std::cout << "Max height reached from any start velocity: " << maxHeight << "\n";
    std::cout << "Total number of velocities that hit the target: " << totalHits << "\n";
}
