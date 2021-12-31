#include <array>
#include <exception>
#include <iostream>
#include <iterator>
#include <memory>
#include <string>
#include <vector>

struct Packet
{
    Packet(int v, int t)
        : version{ v },
        type{ t }
    {}
    virtual ~Packet() {}

    virtual int SumVersions() const = 0;
    virtual int64_t ExpressionValue() const = 0;

    int version;
    int type;
};

struct LiteralValue : Packet
{
    LiteralValue(int v, int t, int64_t val)
        : Packet{ v, t },
        value{ val }
    {}

    int SumVersions() const override { return version; }
    int64_t ExpressionValue() const override { return value; }

    int64_t value;
};

struct Operator : Packet
{
    Operator(int v, int t, std::vector<std::unique_ptr<Packet>> sub)
        : Packet{ v, t },
        subpackets{ std::move(sub) }
    {}

    int SumVersions() const override
    {
        int sum = 0;
        for (auto const& p : subpackets)
        {
            sum += p->SumVersions();
        }
        return sum + version;
    }
    int64_t ExpressionValue() const override
    {
        switch (type)
        {
        case 0: // sum
        {
            int64_t sum = 0;
            for (auto const& p : subpackets)
            {
                sum += p->ExpressionValue();
            }
            return sum;
        }
        case 1: // product
        {
            int64_t prod = 1;
            for (auto const& p : subpackets)
            {
                prod *= p->ExpressionValue();
            }
            return prod;
        }
        case 2: // minimum
        {
            int64_t min = std::numeric_limits<int64_t>::max();
            for (auto const& p : subpackets)
            {
                auto val = p->ExpressionValue();
                if (val < min)
                {
                    min = val;
                }
            }
            return min;
        }
        case 3: // maximum
        {
            int64_t max = std::numeric_limits<int>::min();
            for (auto const& p : subpackets)
            {
                auto val = p->ExpressionValue();
                if (val > max)
                {
                    max = val;
                }
            }
            return max;
        }
        case 5: // greater than
        {
            return subpackets[0]->ExpressionValue() > subpackets[1]->ExpressionValue() ? 1 : 0;
        }
        case 6: // less than
        {
            return subpackets[0]->ExpressionValue() < subpackets[1]->ExpressionValue() ? 1 : 0;
        }
        case 7: // equal
        {
            return subpackets[0]->ExpressionValue() == subpackets[1]->ExpressionValue() ? 1 : 0;
        }
        }
        return 0;
    }

    std::vector<std::unique_ptr<Packet>> subpackets;
};

std::array<uint8_t, 4> HexCharToBits(char c)
{
    switch (c)
    {
    case '0':
        return { 0,0,0,0 };
    case '1':
        return { 0,0,0,1 };
    case '2':
        return { 0,0,1,0 };
    case '3':
        return { 0,0,1,1 };
    case '4':
        return { 0,1,0,0 };
    case '5':
        return { 0,1,0,1 };
    case '6':
        return { 0,1,1,0 };
    case '7':
        return { 0,1,1,1 };
    case '8':
        return { 1,0,0,0 };
    case '9':
        return { 1,0,0,1 };
    case 'A': case 'a':
        return { 1,0,1,0 };
    case 'B': case 'b':
        return { 1,0,1,1 };
    case 'C': case 'c':
        return { 1,1,0,0 };
    case 'D': case 'd':
        return { 1,1,0,1 };
    case 'E': case 'e':
        return { 1,1,1,0 };
    case 'F': case 'f':
        return { 1,1,1,1 };
    default:
        throw std::runtime_error(std::string("Unrecgonised hex character: ") + c);
    }
}

auto InputToBits(std::string const& input)
{
    std::vector<uint8_t> bits;

    for (auto const& c : input)
    {
        auto newBits = HexCharToBits(c);
        bits.insert(bits.end(), newBits.begin(), newBits.end());
    }
    return bits;
}

auto BitsToValue(std::input_iterator auto first, std::input_iterator auto last)
{
    int value = 0;
    while (first != last)
    {
        value = (value << 1) + *first;
        ++first;
    }
    return value;
}



std::vector<std::unique_ptr<Packet>> BitsToPackets(std::input_iterator auto &first, std::input_iterator auto last, int packetLimit)
{
    std::vector<std::unique_ptr<Packet>> result;

    while (std::distance(first, last) > 10 && packetLimit > 0)
    {
        --packetLimit;
        auto version = BitsToValue(first, first + 3);
        std::advance(first, 3);
        auto type = BitsToValue(first, first + 3);
        std::advance(first, 3);

        if (type == 4)
        {
            // literal
            int64_t val = 0;
            int notLast = 0;
            do
            {
                notLast = *first;
                ++first;
                auto nextVal = BitsToValue(first, first + 4);
                val = (val << 4) + nextVal;
                std::advance(first, 4);
            } while (notLast != 0);
            auto packet = std::make_unique<LiteralValue>(version, type, val);
            result.push_back(std::move(packet));
        }
        else
        {
            // operator
            auto lengthType = *first;
            ++first;
            if (lengthType == 0)
            {
                auto subPacketBits = BitsToValue(first, first + 15);
                std::advance(first, 15);
                auto subStart = first;
                auto subPackets = BitsToPackets(subStart, first + subPacketBits, std::numeric_limits<int>::max());
                std::advance(first, subPacketBits);
                auto packet = std::make_unique<Operator>(version, type, std::move(subPackets));
                result.push_back(std::move(packet));
            }
            else
            {
                auto subPacketCount = BitsToValue(first, first + 11);
                std::advance(first, 11);
                auto subStart = first;
                auto subPackets = BitsToPackets(subStart, last, subPacketCount);
                first = subStart;

                auto packet = std::make_unique<Operator>(version, type, std::move(subPackets));
                result.push_back(std::move(packet));
            }
        }
    }

    return result;
}

int main()
{
    std::string input;
    std::getline(std::cin, input);

    auto bits = InputToBits(input);

    auto first = bits.begin();
    auto packets = BitsToPackets(first, bits.end(), std::numeric_limits<int>::max());

    auto versionSum = packets[0]->SumVersions();
    std::cout << "Sum of packet versions: " << versionSum << "\n";

    auto exprValue = packets[0]->ExpressionValue();
    std::cout << "Value of expression: " << exprValue << "\n";
}
