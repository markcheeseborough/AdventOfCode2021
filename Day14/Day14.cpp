#include <algorithm>
#include <iostream>
#include <map>
#include <numeric>
#include <regex>
#include <string>

using InsertionMap = std::map<std::pair<char, char>, char>;

std::string ReadStart()
{
    std::string result;
    std::getline(std::cin, result);
    return result;
}

InsertionMap ReadInsertions()
{
    InsertionMap result;

    std::regex regex("(.)(.) -> (.)");

    std::string input;
    while (std::getline(std::cin, input))
    {
        if (input.empty() && result.empty())
        {
            continue;
        }
        if (input.empty())
        {
            break;
        }

        std::smatch matches;
        if (std::regex_match(input, matches, regex))
        {
            if (matches.size() == 4)
            {
                auto first = matches[1].str()[0];
                auto second = matches[2].str()[0];
                auto insertion = matches[3].str()[0];

                result.insert(std::make_pair(std::make_pair(first, second), insertion));
            }
        }
    }

    return result;
}

auto CountChars(std::string const& str)
{
    std::map<char, int> result;

    for (auto const& c : str)
    {
        if (result.contains(c))
        {
            result[c]++;
        }
        else
        {
            result.emplace(c, 1);
        }
    }

    return result;
}

void PartOne(std::string input, InsertionMap const& insertions)
{
    auto currentString = input;

    for (int i = 0; i < 10; ++i)
    {
        currentString = std::inner_product(currentString.begin(), std::prev(currentString.end()), std::next(currentString.begin()),
            currentString.substr(0, 1),
            [](std::string acc, std::string val) { return acc + val; },
            [&insertions](char first, char second)
            {
                auto insertChar = insertions.find(std::make_pair(first, second));
                auto insert = insertChar->second;
                return std::string{ insert, second };
            });
    }

    auto charCounts = CountChars(currentString);

    auto const [minVal, maxVal] = std::minmax_element(charCounts.begin(), charCounts.end(),
        [](auto const& left, auto const& right)
        {
            return left.second < right.second;
        });

    auto maxMinDiff = maxVal->second - minVal->second;

    std::cout << "Part One Difference between most and least common chars: " << maxMinDiff << "\n";
}

void PartTwo(std::string input, InsertionMap const& insertions)
{
    using PairCounter = std::map<std::pair<char, char>, uint64_t>;
    std::map<char, uint64_t> charCounts;
    PairCounter pairCounts;

    auto addCharCount = [&charCounts](char c, uint64_t count)
    {
        if (charCounts.contains(c))
        {
            charCounts[c] += count;
        }
        else
        {
            charCounts[c] = count;
        }
    };
    auto addPairCount = [](PairCounter& pairCounts, std::pair<char, char> pair, uint64_t count)
    {
        if (pairCounts.contains(pair))
        {
            pairCounts[pair] += count;
        }
        else
        {
            pairCounts[pair] = count;
        }
    };

    for (auto const& c : input)
    {
        addCharCount(c, 1);
    }
    for (int i = 0; i < input.size() - 1; ++i)
    {
        auto first = input[i];
        auto second = input[i + 1];
        auto pair = std::make_pair(first, second);
        addPairCount(pairCounts, pair, 1);
    }

    for (int i = 0; i < 40; ++i)
    {
        PairCounter newPairCounts;

        for (auto const& [pair, count] : pairCounts)
        {
            auto insertChar = insertions.find(pair);

            auto p1 = std::make_pair(pair.first, insertChar->second);
            auto p2 = std::make_pair(insertChar->second, pair.second);

            addPairCount(newPairCounts, p1, count);
            addPairCount(newPairCounts, p2, count);
            addCharCount(insertChar->second, count);
        }
        pairCounts = std::move(newPairCounts);
    }
    auto const [minVal, maxVal] = std::minmax_element(charCounts.begin(), charCounts.end(),
        [](auto const& left, auto const& right)
        {
            return left.second < right.second;
        });

    auto maxMinDiff = maxVal->second - minVal->second;

    std::cout << "Part Two Difference between most and least common chars: " << maxMinDiff << "\n";
}

int main()
{
    auto startValue = ReadStart();

    auto insertions = ReadInsertions();

    PartOne(startValue, insertions);
    PartTwo(startValue, insertions);
}

