#include <algorithm>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>

struct Point
{
    int x;
    int y;
    auto operator<=>(const Point&) const = default;
};

struct Line
{
    Point start;
    Point end;
    auto operator<=>(const Line&) const = default;

    constexpr bool IsHorizontal() const noexcept
    {
        return start.y == end.y;
    }
    constexpr bool IsVertical() const noexcept
    {
        return start.x == end.x;
    }
    constexpr bool IsDiagonalUp() const noexcept
    {
        return start.x != end.x && start.y < end.y;
    }
    constexpr bool IsDiagonalDown() const noexcept
    {
        return start.x != end.x && start.y > end.y;
    }
};

struct LineIterator
{
    Line line;
    Point loc{ 0,0 };

    using value_type = Point;
    using pointer = Point*;
    using reference = Point&;
    using iterator_category = std::forward_iterator_tag;
    using difference_type = std::ptrdiff_t;

    constexpr LineIterator& operator++() noexcept
    {
        if (line.IsHorizontal())
        {
            ++loc.x;
        }
        else if (line.IsVertical())
        {
            ++loc.y;
        }
        else if (line.IsDiagonalUp())
        {
            ++loc.x;
            ++loc.y;
        }
        else
        {
            ++loc.x;
            --loc.y;
        }
        return *this;
    }
    constexpr LineIterator operator++(int) noexcept
    {
        auto const prev = *this;
        ++(*this);
        return prev;
    }

    [[nodiscard]] constexpr bool operator!=(LineIterator const& other) const noexcept
    {
        return loc != other.loc;
    }
    [[nodiscard]] constexpr Point const& operator*() const noexcept
    {
        return loc;
    }
    [[nodiscard]] constexpr Point& operator*() noexcept
    {
        return loc;
    }
};
[[nodiscard]] constexpr LineIterator begin(Line const& l) noexcept
{
    return LineIterator{ l, { l.start } };
}
[[nodiscard]] constexpr LineIterator end(Line const& l) noexcept
{
    if (l.IsHorizontal())
    {
        return LineIterator{ l, { l.end.x + 1, l.end.y } };
    }
    else if (l.IsVertical())
    {
        return LineIterator{ l, { l.end.x, l.end.y + 1 } };
    }
    else if (l.IsDiagonalUp())
    {
        return LineIterator{ l, { l.end.x + 1, l.end.y + 1 } };
    }
    else
    {
        return LineIterator{ l, { l.end.x + 1, l.end.y - 1 } };
    }
}

std::vector<Line> ReadInput()
{
    std::vector<Line> result;

    std::string inLine;
    while (std::getline(std::cin, inLine))
    {
        if (inLine.empty())
        {
            break;
        }

        std::stringstream in{ inLine };
        int x1;
        int y1;
        char comma;
        in >> x1;
        in >> comma; //,
        in >> y1;
        std::string arrow;
        in >> arrow; // ->
        int x2;
        int y2;
        in >> x2;
        in >> comma; //,
        in >> y2;

        Point p1{ x1, y1 };
        Point p2{ x2, y2 };
        p1 < p2 ?
            result.emplace_back(p1, p2) :
            result.emplace_back(p2, p1);
    }

    return result;
}

int MaximumWidth(std::vector<Line> const& lines)
{
    int maxWidth = 0;
    for (auto const& l : lines)
    {
        if (l.end.x > maxWidth)
        {
            maxWidth = l.end.x;
        }
    }
    return maxWidth + 1;
}

int MaximumHeight(std::vector<Line> const& lines)
{
    int maxHeight = 0;
    for (auto const& l : lines)
    {
        if (l.end.y > maxHeight)
        {
            maxHeight = l.end.y;
        }
    }
    return maxHeight + 1;
}

int main()
{
    auto lines = ReadInput();

    auto maxWidth = MaximumWidth(lines);
    auto maxHeight = MaximumHeight(lines);

    std::vector<std::vector<int>> grid;
    grid.reserve(maxWidth);
    for (int i = 0; i < maxWidth; ++i)
    {
        grid.emplace_back(maxHeight, 0);
    }

    for (auto const& l : lines)
    {
        //if (l.IsHorizontal() || l.IsVertical())
        {
            std::for_each(begin(l), end(l),
                [&grid](Point const& p)
                {
                    ++grid[p.x][p.y];
                });
        }
    }

    int dangerPoints = 0;
    for (int x = 0; x < maxWidth; ++x)
    {
        for (int y = 0; y < maxHeight; ++y)
        {
            if (grid[x][y] > 1)
            {
                ++dangerPoints;
            }
        }
    }

    std::cout << "There are " << dangerPoints << " points with overlap.\n";
}
