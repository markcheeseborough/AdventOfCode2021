#include <algorithm>
#include <array>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>

struct Entry
{
    std::array<std::string, 10> signalPattern;
    std::array<std::string, 4> outputValue;
};

std::vector<Entry> ReadInput()
{
    std::vector<Entry> result;
    std::string inLine;
    while (std::getline(std::cin, inLine))
    {
        if (inLine.empty())
        {
            break;
        }

        std::stringstream in{ inLine };
        Entry e;
        for (int i = 0; i < 10; ++i)
        {
            in >> e.signalPattern[i];
            std::sort(e.signalPattern[i].begin(), e.signalPattern[i].end());
        }
        char pipe;
        in >> pipe; // '|'
        for (int i = 0; i < 4; ++i)
        {
            in >> e.outputValue[i];
            std::sort(e.outputValue[i].begin(), e.outputValue[i].end());
        }
        result.push_back(e);
    }
    return result;
}

void PartOne(std::vector<Entry> const& entries)
{
    int uniqueOutputCount = 0;
    for (auto const& e : entries)
    {
        for (auto const& s : e.outputValue)
        {
            auto length = s.length();
            if (length == 2 || length == 3 || length == 4 || length == 7)
            {
                ++uniqueOutputCount;
            }
        }
    }
    std::cout << "Unique output digits = " << uniqueOutputCount << "\n";
}

bool ContainsCharacters(std::string const& target, std::string const& chars)
{
    for (auto c : chars)
    {
        if (target.find(c) == std::string::npos)
        {
            return false;
        }
    }
    return true;
}

void PartTwo(std::vector<Entry> const& entries)
{
    int outputSum = 0;
    for (const auto& e : entries)
    {
        auto onePattern = *std::find_if(e.signalPattern.begin(), e.signalPattern.end(), [](const auto& s) { return s.length() == 2; });
        auto fourPattern = *std::find_if(e.signalPattern.begin(), e.signalPattern.end(), [](const auto& s) { return s.length() == 4; });
        auto sevenPattern = *std::find_if(e.signalPattern.begin(), e.signalPattern.end(), [](const auto& s) { return s.length() == 3; });
        auto eightPattern = *std::find_if(e.signalPattern.begin(), e.signalPattern.end(), [](const auto& s) { return s.length() == 7; });

        auto threePattern = *std::find_if(e.signalPattern.begin(), e.signalPattern.end(), [&onePattern](const auto& s) { return s.length() == 5 && ContainsCharacters(s, onePattern); });
        auto sixPattern = *std::find_if(e.signalPattern.begin(), e.signalPattern.end(), [&onePattern](const auto& s) { return s.length() == 6 && !ContainsCharacters(s, onePattern); });
        auto ninePattern = *std::find_if(e.signalPattern.begin(), e.signalPattern.end(), [&fourPattern](const auto& s) { return s.length() == 6 && ContainsCharacters(s, fourPattern); });
        auto zeroPattern = *std::find_if(e.signalPattern.begin(), e.signalPattern.end(), [&sixPattern, &ninePattern](const auto& s) { return s.length() == 6 && s != ninePattern && s != sixPattern; });
        auto fivePattern = *std::find_if(e.signalPattern.begin(), e.signalPattern.end(), [&sixPattern](const auto& s) { return s.length() == 5 && ContainsCharacters(sixPattern, s); });
        auto twoPattern = *std::find_if(e.signalPattern.begin(), e.signalPattern.end(), [&threePattern, &fivePattern](const auto& s) { return s.length() == 5 && s != threePattern && s != fivePattern; });

        std::array patterns{ zeroPattern, onePattern, twoPattern, threePattern, fourPattern, fivePattern, sixPattern, sevenPattern, eightPattern, ninePattern };

        int output = 0;
        for (const auto& outVal : e.outputValue)
        {
            for (int i = 0; i <= 9; ++i)
            {
                if (patterns[i] == outVal)
                {
                    output = (output * 10) + i;
                    break;
                }
            }
        }
        std::cout << "Output value " << output << "\n";
        outputSum += output;
    }

    std::cout << "Sum of output values = " << outputSum << "\n";
}

int main()
{
    auto entries = ReadInput();

    PartOne(entries);
    PartTwo(entries);
}
