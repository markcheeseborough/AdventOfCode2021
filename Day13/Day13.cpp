#include <iostream>
#include <regex>
#include <set>
#include <string>
#include <vector>

enum class Axis
{
    X,
    Y
};

auto ReadPoints()
{
    std::set<std::pair<int, int>> result;

    std::string input;
    while (std::getline(std::cin, input))
    {
        if (input.empty())
        {
            break;
        }
        auto split = input.find(',');
        auto first = std::stoi(input.substr(0, split));
        auto second = std::stoi(input.substr(split + 1));
        result.insert(std::make_pair(first, second));
    }
    return result;
}

auto ReadFolds()
{
    std::vector<std::pair<Axis, int>> result;

    std::regex regex("fold along (.)=(\\d+)");

    std::string input;
    while (std::getline(std::cin, input))
    {
        if (input.empty())
        {
            break;
        }

        std::smatch matches;
        if (std::regex_match(input, matches, regex))
        {
            if (matches.size() == 3)
            {
                auto axisStr = matches[1].str();
                auto valueStr = matches[2].str();

                auto axis = axisStr == "x" ? Axis::X : Axis::Y;
                auto value = std::stoi(valueStr);
                result.emplace_back(axis, value);
            }
        }
    }
    return result;
}

auto HorizontalFold(int pos, std::set<std::pair<int, int>> const &points)
{
    std::set<std::pair<int, int>> result;

    for (auto const &p : points)
    {
        if (p.second > pos)
        {
            auto newY = pos - (p.second - pos);
            result.insert(std::make_pair(p.first, newY));
        }
        else
        {
            result.insert(p);
        }
    }
    return result;
}

auto VerticalFold(int pos, std::set<std::pair<int, int>> const& points)
{
    std::set<std::pair<int, int>> result;

    for (auto const &p : points)
    {
        if (p.first > pos)
        {
            auto newX = pos - (p.first - pos);
            result.insert(std::make_pair(newX, p.second));
        }
        else
        {
            result.insert(p);
        }
    }
    return result;
}

int MaximumWidth(std::set<std::pair<int, int>> const& points)
{
    int maxWidth = 0;
    for (auto const& l : points)
    {
        if (l.first > maxWidth)
        {
            maxWidth = l.first;
        }
    }
    return maxWidth + 1;
}

int MaximumHeight(std::set<std::pair<int, int>> const& points)
{
    int maxHeight = 0;
    for (auto const& l : points)
    {
        if (l.second > maxHeight)
        {
            maxHeight = l.second;
        }
    }
    return maxHeight + 1;
}

int main()
{
    auto points = ReadPoints();

    auto folds = ReadFolds();

    std::cout << "Initial points: " << points.size() << "\n";

    int foldCount = 0;
    for (auto const& fold : folds)
    {
        if (fold.first == Axis::X)
        {
            points = VerticalFold(fold.second, points);
        }
        else
        {
            points = HorizontalFold(fold.second, points);
        }
        ++foldCount;

        std::cout << "Points after " << foldCount << " folds: " << points.size() << "\n";
    }

    std::cout << "\nPart 2\n\n";

    auto width = MaximumWidth(points);
    auto height = MaximumHeight(points);

    std::vector<std::vector<char>> grid(height, std::vector<char>(width, ' '));
    for (auto const& p : points)
    {
        grid[p.second][p.first] = '#';
    }
    for (auto const& row : grid)
    {
        for (auto const& c : row)
        {
            std::cout << c;
        }
        std::cout << "\n";
    }
}
