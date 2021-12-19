#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

std::vector<std::string> ReadInput()
{
    std::vector<std::string> commands;

    std::string input;
    while (std::getline(std::cin, input))
    {
        if (input.empty())
        {
            break;
        }
        commands.push_back(input);
    }
    return commands;
}

auto CountBitsInPos(std::vector<std::string> const& input, int pos, char bit)
{
    auto bitCount = std::count_if(input.begin(), input.end(),
        [pos, bit](std::string const& s)
        {
            return s[pos] == bit;
        });
    return bitCount;
}

auto ValuesWithBitInPos(std::vector<std::string> const& input, int pos, char bit)
{
    std::vector<std::string> result;
    std::copy_if(input.begin(), input.end(), std::back_inserter(result),
        [pos, bit](std::string const& s)
        {
            return s[pos] == bit;
        });
    return result;
}

auto FindOxygenRating(std::vector<std::string> const& input)
{
    int bitLength = input[0].size();
    auto values = input;
    for (int pos = 0; pos < bitLength && values.size() > 1; ++pos)
    {
        int zeroBits = CountBitsInPos(values, pos, '0');
        int oneBits = CountBitsInPos(values, pos, '1');

        auto mostCommon = oneBits >= zeroBits ? '1' : '0';
        values = ValuesWithBitInPos(values, pos, mostCommon);
    }
    return values[0];
}

auto FindScrubberRating(std::vector<std::string> const& input)
{
    int bitLength = input[0].size();
    auto values = input;
    for (int pos = 0; pos < bitLength && values.size() > 1; ++pos)
    {
        int zeroBits = CountBitsInPos(values, pos, '0');
        int oneBits = CountBitsInPos(values, pos, '1');

        auto leastCommon = zeroBits <= oneBits ? '0' : '1';
        values = ValuesWithBitInPos(values, pos, leastCommon);
    }
    return values[0];
}

int main()
{
    auto input = ReadInput();
    int bitLength = input[0].size();

    std::vector<char> gammaBits;
    std::vector<char> epsilonBits;

    for (int pos = 0; pos < bitLength; ++pos)
    {
        int zeroBits = CountBitsInPos(input, pos, '0');
        int oneBits = CountBitsInPos(input, pos, '1');

        gammaBits.push_back(zeroBits > oneBits ? '0' : '1');
        epsilonBits.push_back(zeroBits < oneBits ? '0' : '1');
    }

    std::string gamma(gammaBits.begin(), gammaBits.end());
    std::string epsilon(epsilonBits.begin(), epsilonBits.end());

    int gammaVal = std::stoi(gamma, nullptr, 2);
    int epsilonVal = std::stoi(epsilon, nullptr, 2);

    std::cout << "Gamma: " << gammaVal << " - Epsilon: " << epsilonVal << " - multiplied: " << gammaVal * epsilonVal << "\n";

    auto oxygenRating = FindOxygenRating(input);
    auto scrubberRating = FindScrubberRating(input);
    auto oxygenVal = std::stoi(oxygenRating, nullptr, 2);
    auto scrubberVal = std::stoi(scrubberRating, nullptr, 2);
    std::cout << "Oxygen rating: " << oxygenVal << " - Scrubber rating: " << scrubberVal << " - mulitpled: " << oxygenVal * scrubberVal << "\n";
}
