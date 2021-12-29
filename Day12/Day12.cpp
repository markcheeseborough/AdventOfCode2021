#include <algorithm>
#include <iostream>
#include <map>
#include <set>
#include <string>
#include <vector>

struct Cave
{
    std::string name;
    bool large{ false };
    std::vector<std::string> links;
};

auto ReadInput()
{
    std::vector<std::pair<std::string, std::string>> result;

    std::string input;
    while (std::getline(std::cin, input))
    {
        if (input.empty())
        {
            break;
        }
        auto split = input.find('-');
        auto first = input.substr(0, split);
        auto second = input.substr(split + 1);
        result.emplace_back(first, second);
    }
    return result;
}

auto MakeCave(std::string name, std::string linkTo)
{
    auto isLarge = std::all_of(name.begin(), name.end(), isupper);
    return Cave{ name, isLarge, { linkTo } };
}

auto BuildCaves(std::vector<std::pair<std::string, std::string>> const& caveLinks)
{
    std::map<std::string, Cave> caves;

    for (auto const& link : caveLinks)
    {
        if (!caves.contains(link.first))
        {
            auto c = MakeCave(link.first, link.second);
            caves.insert({ link.first, c });
        }
        else
        {
            caves[link.first].links.push_back(link.second);
        }

        if (!caves.contains(link.second))
        {
            auto c = MakeCave(link.second, link.first);
            caves.insert({ link.second, c });
        }
        else
        {
            caves[link.second].links.push_back(link.first);
        }
    }

    return caves;
}

std::vector<std::vector<std::string>> GetPathsPartOne(std::map<std::string, Cave> const& caves, std::string currentCave, std::vector<std::string> pathSoFar, std::set<std::string> smallCavesVisited)
{
    auto const& caveIt = caves.find(currentCave);
    auto const& cave = caveIt->second;

    pathSoFar.push_back(currentCave);
    if (!cave.large)
    {
        smallCavesVisited.insert(currentCave);
    }

    if (currentCave == "end")
    {
        return { pathSoFar };
    }
    else
    {
        std::vector<std::vector<std::string>> results;
        for (auto const& link : cave.links)
        {
            if (!smallCavesVisited.contains(link))
            {
                auto paths = GetPathsPartOne(caves, link, pathSoFar, smallCavesVisited);
                results.insert(results.end(), paths.begin(), paths.end());
            }
        }
        return results;
    }
}

std::vector<std::vector<std::string>> GetPathsPartTwo(std::map<std::string, Cave> const& caves, std::string currentCave, std::vector<std::string> pathSoFar, std::map<std::string, int> smallCavesVisited)
{
    auto const& caveIt = caves.find(currentCave);
    auto const& cave = caveIt->second;

    pathSoFar.push_back(currentCave);
    if (!cave.large)
    {
        if (smallCavesVisited.contains(currentCave))
        {
            smallCavesVisited[currentCave]++;
        }
        else
        {
            smallCavesVisited.insert({ currentCave, 1 });
        }
    }

    if (currentCave == "end")
    {
        return { pathSoFar };
    }
    else
    {
        auto hasMultipleVisits = std::any_of(smallCavesVisited.begin(), smallCavesVisited.end(),
            [](auto const& p)
            {
                return p.second > 1;
            });
        std::vector<std::vector<std::string>> results;
        for (auto const& link : cave.links)
        {
            if (link != "start" && (!hasMultipleVisits || !smallCavesVisited.contains(link)))
            {
                auto paths = GetPathsPartTwo(caves, link, pathSoFar, smallCavesVisited);
                results.insert(results.end(), paths.begin(), paths.end());
            }
        }
        return results;
    }
}

int main()
{
    auto caveLinks = ReadInput();

    auto caves = BuildCaves(caveLinks);

    auto pathsPartOne = GetPathsPartOne(caves, "start", {}, {});

    std::cout << "Total Part 1 paths: " << pathsPartOne.size() << "\n";

    auto pathsPartTwo = GetPathsPartTwo(caves, "start", {}, {});

    std::cout << "Total Part 2 paths: " << pathsPartTwo.size() << "\n";
}

