#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

enum class Cell
{
    Empty,
    EastMover,
    SouthMover
};

class Grid
{
public:
    Grid(int width, int height)
        : width_{ width },
        height_{ height },
        cells_(height_, std::vector<Cell>(width_, Cell::Empty))
    {}

    Cell At(int x, int y) const noexcept
    {
        return cells_[y % height_][x % width_];
    }

    void Set(int x, int y, Cell c)
    {
        cells_[y % height_][x % width_] = c;
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
        for (auto const& row : cells_)
        {
            std::transform(row.begin(), row.end(), std::ostream_iterator<char>(out), [](Cell c) { return c == Cell::Empty ? '.' : (c == Cell::EastMover ? '>' : 'v'); });
            out << "\n";
        }
    }

private:
    int width_;
    int height_;
    std::vector<std::vector<Cell>> cells_;
};

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
            auto cell = [](char c)
            {
                switch (c)
                {
                case '.': return Cell::Empty;
                case '>': return Cell::EastMover;
                case 'v': return Cell::SouthMover;
                }
            }(gridData[y][x]);
            result.Set(x, y, cell);
        }
    }

    return result;
}

bool DoMoveEast(Grid const& movers, Grid& target)
{
    bool didMove = false;
    for (int y = 0; y < movers.Height(); ++y)
    {
        for (int x = 0; x < movers.Width(); ++x)
        {
            if (movers.At(x, y) == Cell::EastMover && movers.At(x + 1, y) == Cell::Empty)
            {
                target.Set(x, y, Cell::Empty);
                target.Set(x + 1, y, Cell::EastMover);
                // now skip the the cell just moved into
                ++x;
                didMove = true;
            }
            else
            {
                target.Set(x, y, movers.At(x, y));
            }
        }
    }
    return didMove;
}

bool DoMoveSouth(Grid const& movers, Grid& target)
{
    bool didMove = false;
    for (int x = 0; x < movers.Width(); ++x)
    {
        for (int y = 0; y < movers.Height(); ++y)
        {
            if (movers.At(x, y) == Cell::SouthMover && movers.At(x, y + 1) == Cell::Empty)
            {
                target.Set(x, y, Cell::Empty);
                target.Set(x, y + 1, Cell::SouthMover);
                // now skip the the cell just moved into
                ++y;
                didMove = true;
            }
            else
            {
                target.Set(x, y, movers.At(x, y));
            }
        }
    }
    return didMove;
}


int main()
{
    auto moveEast = ReadInput();
    Grid moveSouth(moveEast.Width(), moveEast.Height());

    bool didMoveEast = false;
    bool didMoveSouth = false;
    int steps = 0;

    do
    {
        didMoveEast = DoMoveEast(moveEast, moveSouth);
        didMoveSouth = DoMoveSouth(moveSouth, moveEast);
        ++steps;
    } while (didMoveEast || didMoveSouth);

    std::cout << "Sea cucumbers stopped moving after " << steps << " steps.\n";

    //moveEast.Print(std::cout);
}
