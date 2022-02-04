#include <array>
#include <iostream>
#include <regex>
#include <string>
#include <vector>

std::vector<std::string> ReadInput()
{
    std::vector<std::string> result;

    std::string input;
    while (std::getline(std::cin, input))
    {
        if (input.empty())
        {
            break;
        }
        result.push_back(input);
    }
    return result;
}

struct Params
{
    int op;
    int correction;
    int offset;
};

auto ExtractParams(std::vector<std::string> const& input)
{
    std::regex regex("... . (-?\\d+)");
    std::smatch matches;

    std::array<Params, 14> result{};

    for (int block = 0; block < 14; ++block)
    {
        int inputOffset = block * 18;
        // op = inputOffset + 4
        std::regex_match(input[inputOffset + 4], matches, regex);
        int op = std::stoi(matches[1].str());
        // correction = inputOffset + 5
        std::regex_match(input[inputOffset + 5], matches, regex);
        int correction = std::stoi(matches[1].str());
        // offset = inputOffset + 15
        std::regex_match(input[inputOffset + 15], matches, regex);
        int offset = std::stoi(matches[1].str());

        result[block] = { op, correction, offset };
    }
    return result;
}

struct OperationPair
{
    int addIndex;
    int subIndex;
    int adjustment;
};

auto GroupOperations(std::array<Params, 14> const& params)
{
    std::vector<std::pair<int, int>> stack;
    std::vector<OperationPair> result;
    for (int i = 0; i < 14; ++i)
    {
        auto const& [op, correction, offset] = params[i];
        if (op == 1)
        {
            stack.emplace_back(offset, i);
        }
        else
        {
            auto const& [off, in] = stack.back();
            stack.pop_back();
            result.emplace_back(in, i, off + correction);
        }
    }

    return result;
}

auto MaximumNumber(std::vector<OperationPair> const& operationGroups)
{
    std::array<int, 14> number{ 0 };
    for (auto const& op : operationGroups)
    {
        if (op.adjustment < 0)
        {
            number[op.addIndex] = 9;
            number[op.subIndex] = 9 + op.adjustment;
        }
        else
        {
            number[op.subIndex] = 9;
            number[op.addIndex] = 9 - op.adjustment;
        }
    }
    return number;
}

auto MinimumNumber(std::vector<OperationPair> const& operationGroups)
{
    std::array<int, 14> number{ 0 };
    for (auto const& op : operationGroups)
    {
        if (op.adjustment < 0)
        {
            number[op.addIndex] = 1 - op.adjustment;
            number[op.subIndex] = 1;
        }
        else
        {
            number[op.subIndex] = 1 + op.adjustment;
            number[op.addIndex] = 1;
        }
    }
    return number;
}

void PrintNumber(std::array<int, 14> const& number)
{
    for (auto const& d : number)
    {
        std::cout << d;
    }
}

int main()
{
    auto rawInput = ReadInput();

    auto paramSet = ExtractParams(rawInput);

    auto operationGroups = GroupOperations(paramSet);

    auto maximum = MaximumNumber(operationGroups);
    std::cout << "Maximum: ";
    PrintNumber(maximum);
    std::cout << '\n';

    auto minimum = MinimumNumber(operationGroups);
    std::cout << "Minimum: ";
    PrintNumber(minimum);
    std::cout << '\n';
}
