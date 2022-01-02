#include <iostream>
#include <memory>
#include <optional>
#include <sstream>
#include <string>
#include <vector>

enum class NumberType
{
    Pair,
    Regular
};

class FishNumber abstract
{
public:
    virtual ~FishNumber() {};

    virtual void Print(std::ostream& out) const = 0;
    virtual uint64_t Magnitude() const noexcept = 0;
    virtual NumberType Type() const noexcept = 0;
    virtual std::unique_ptr<FishNumber> Copy() const = 0;
    virtual std::unique_ptr<FishNumber> CopyAddingToLeft(int val) const = 0;
    virtual std::unique_ptr<FishNumber> CopyAddingToRight(int val) const = 0;
    virtual std::unique_ptr<FishNumber> Split() const = 0;
};

std::ostream& operator<<(std::ostream& out, FishNumber const& number)
{
    number.Print(out);
    return out;
}

struct ExplodeResult
{
    std::unique_ptr<FishNumber> newNumber;
    int addLeft;
    int addRight;
};

class FishPair : public FishNumber
{
public:
    FishPair(std::unique_ptr<FishNumber> l, std::unique_ptr<FishNumber> r)
        : left{ std::move(l) },
        right{ std::move(r) }
    {}

    void Print(std::ostream& out) const override
    {
        out << "[";
        left->Print(out);
        out << ",";
        right->Print(out);
        out << "]";
    }

    uint64_t Magnitude() const noexcept override
    {
        return 3 * left->Magnitude() + 2 * right->Magnitude();
    }
    NumberType Type() const noexcept override
    {
        return NumberType::Pair;
    }

    std::unique_ptr<FishNumber> Copy() const override
    {
        return std::make_unique<FishPair>(
            left->Copy(),
            right->Copy());
    }
    std::unique_ptr<FishNumber> CopyAddingToLeft(int val) const override
    {
        return std::make_unique<FishPair>(
            left->CopyAddingToLeft(val),
            right->Copy());
    }
    std::unique_ptr<FishNumber> CopyAddingToRight(int val) const override
    {
        return std::make_unique<FishPair>(
            left->Copy(),
            right->CopyAddingToRight(val));
    }

    std::unique_ptr<FishNumber> Split() const override
    {
        auto leftResult = left->Split();
        if (leftResult)
        {
            return std::make_unique<FishPair>(
                std::move(leftResult),
                right->Copy());
        }
        auto rightResult = right->Split();
        if (rightResult)
        {
            return std::make_unique<FishPair>(
                left->Copy(),
                std::move(rightResult));
        }
        return nullptr;
    }

    ExplodeResult Explode(int curDepth, int maxDepth);

private:
    std::unique_ptr<FishNumber> left;
    std::unique_ptr<FishNumber> right;
};

class RegularNumber : public FishNumber
{
public:
    explicit RegularNumber(int val)
        : value{ val }
    {}

    void Print(std::ostream &out) const override
    {
        out << value;
    }

    uint64_t Magnitude() const noexcept override
    {
        return value;
    }
    NumberType Type() const noexcept override
    {
        return NumberType::Regular;
    }

    std::unique_ptr<FishNumber> Copy() const override
    {
        return std::make_unique<RegularNumber>(value);
    }
    std::unique_ptr<FishNumber> CopyAddingToLeft(int val) const override
    {
        return std::make_unique<RegularNumber>(value + val);
    }
    std::unique_ptr<FishNumber> CopyAddingToRight(int val) const override
    {
        return std::make_unique<RegularNumber>(value + val);
    }

    std::unique_ptr<FishNumber> Split() const override
    {
        if (value >= 10)
        {
            auto newLeft = value / 2;
            auto newRight = (value / 2) + (value % 2);
            return std::make_unique<FishPair>(
                std::make_unique<RegularNumber>(newLeft),
                std::make_unique<RegularNumber>(newRight));
        }
        return nullptr;
    }

private:
    int const value;
};

ExplodeResult FishPair::Explode(int curDepth, int maxDepth)
{
    if (curDepth >= maxDepth)
    {
        // do the explode - this pair will only have regular numbers in it
        return
        {
            std::make_unique<RegularNumber>(0),
            static_cast<int>(left->Magnitude()),
            static_cast<int>(right->Magnitude())
        };
    }
    else
    {
        // do left or right need to explode?
        if (left->Type() == NumberType::Pair)
        {
            auto leftResult = (static_cast<FishPair&>(*left)).Explode(curDepth + 1, maxDepth);
            if (leftResult.newNumber)
            {
                // there was an explosion
                // copy the right side with addRight
                // return new pair along with addLeft (addRight now 0)
                return
                {
                    std::make_unique<FishPair>(
                        std::move(leftResult.newNumber),
                        right->CopyAddingToLeft(leftResult.addRight)),
                    leftResult.addLeft,
                    0
                };
            }
        }
        if (right->Type() == NumberType::Pair)
        {
            auto rightResult = (static_cast<FishPair&>(*right)).Explode(curDepth + 1, maxDepth);
            if (rightResult.newNumber)
            {
                // there was an explosion
                // copy the left side with addLeft
                // return the new pair along with addRight (addLeft now 0)
                return
                {
                    std::make_unique<FishPair>(
                        left->CopyAddingToRight(rightResult.addLeft),
                        std::move(rightResult.newNumber)),
                    0,
                    rightResult.addRight
                };
            }
        }
        // nothing to do
        return
        {
            nullptr,
            0,
            0
        };
    }
}

auto ExplodeNumber(std::unique_ptr<FishNumber> number, bool &didExplode)
{
    if (number->Type() == NumberType::Pair)
    {
        auto result = (static_cast<FishPair&>(*number)).Explode(0, 4);
        if (result.newNumber)
        {
            didExplode = true;
            return std::move(result.newNumber);
        }
    }
    didExplode = false;
    return number;
}

auto SplitNumber(std::unique_ptr<FishNumber> number, bool &didSplit)
{
    auto splitResult = number->Split();
    if (splitResult)
    {
        didSplit = true;
        return splitResult;
    }
    didSplit = false;
    return number;
}

auto ReduceNumber(std::unique_ptr<FishNumber> number)
{
    bool hasReduced = false;
    do
    {
        bool didExplode{ false };
        number = ExplodeNumber(std::move(number), didExplode);

        if (!didExplode)
        {
            bool didSplit{ false };
            number = SplitNumber(std::move(number), didSplit);
            hasReduced = didSplit;
        }
        else
        {
            hasReduced = true;
        }
    } while (hasReduced);
    return number;
}

auto AddFishNumbers(std::unique_ptr<FishNumber> left, std::unique_ptr<FishNumber> right)
{
    return ReduceNumber(std::make_unique<FishPair>(
        std::move(left),
        std::move(right)));
}

enum class TokenType
{
    End,
    Number,
    Comma = ',',
    LB = '[',
    RB = ']'
};

struct Token
{
    TokenType type{ TokenType::End };
    std::optional<int> value;
};

class Lexer
{
public:
    explicit Lexer(std::string const &in)
        : input{ in },
        stream{ input }
    {}

    Token GetToken()
    {
        char ch = 0;
        do
        {
            if (!stream.get(ch))
            {
                return Token{ TokenType::End };
            }
        } while (ch != '\n' && isspace(ch));
        switch (ch)
        {
        case '[':
        case ']':
        case ',':
            return Token{ TokenType{ch} };
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
        {
            stream.putback(ch);
            int number;
            stream >> number;
            return Token{ TokenType::Number, number };
        }
        default:
            throw std::runtime_error(std::string{ "Bad token in input: " } + ch);
        }
    }

private:
    std::string const input;
    std::stringstream stream;
};

class Parser
{
public:
    explicit Parser(std::string const& in)
        : lexer{ in }
    {}

    std::unique_ptr<FishNumber> Result()
    {
        currentToken = lexer.GetToken();

        return ReadPair();
    }

private:

    std::unique_ptr<FishNumber> ReadPair()
    {
        if (currentToken.type != TokenType::LB)
        {

        }
        auto left = ReadSub();
        currentToken = lexer.GetToken(); // ,
        auto right = ReadSub();
        currentToken = lexer.GetToken(); // ]

        return std::make_unique<FishPair>(
            std::move(left),
            std::move(right));
    }

    std::unique_ptr<FishNumber> ReadSub()
    {
        currentToken = lexer.GetToken();
        if (currentToken.type == TokenType::LB)
        {
            return ReadPair();
        }
        else if (currentToken.type == TokenType::Number)
        {
            return std::make_unique<RegularNumber>(currentToken.value.value());
        }
        else
        {
            throw std::runtime_error("Unexpected token type in input");
        }
    }

    Lexer lexer;
    Token currentToken;
};

void TestOne()
{
    std::cout << "Test one\n";

    Parser p1("[1,2]");
    auto valOne = p1.Result();

    Parser p2("[[3,4],5]");
    auto valTwo = p2.Result();

    std::cout << "Input: " << *valOne << " + " << *valTwo << "\n";

    auto added = AddFishNumbers(std::move(valOne), std::move(valTwo));
    std::cout << "Result: " << *added << "\n";
    std::cout << "Magnitude: " << added->Magnitude() << "\n";
}

void TestTwo()
{
    std::cout << "Test two\n";
    Parser p1("[[[[[9,8],1],2],3],4]");
    auto val1 = p1.Result();

    bool b{ false };

    std::cout << "Input: " << *val1 << "\n";
    val1 = ExplodeNumber(std::move(val1), b);
    std::cout << "Result: " << *val1 << "\n";

    Parser p2("[7,[6,[5,[4,[3,2]]]]]");
    auto val2 = p2.Result();
    std::cout << "Input: " << *val2 << "\n";
    val2 = ExplodeNumber(std::move(val2), b);
    std::cout << "Result: " << *val2 << "\n";

    Parser p3("[[6,[5,[4,[3,2]]]],1]");
    auto val3 = p3.Result();
    std::cout << "Input: " << *val3 << "\n";
    val3 = ExplodeNumber(std::move(val3), b);
    std::cout << "Result: " << *val3 << "\n";

    Parser p4("[[3,[2,[1,[7,3]]]],[6,[5,[4,[3,2]]]]]");
    auto val4 = p4.Result();
    std::cout << "Input: " << *val4 << "\n";
    val4 = ExplodeNumber(std::move(val4), b);
    std::cout << "Result: " << *val4 << "\n";
    val4 = ExplodeNumber(std::move(val4), b);
    std::cout << "Result: " << *val4 << "\n";
}

void TestThree()
{
    std::cout << "Test three\n";

    Parser p("[[[[0,7],4],[15,[0,13]]],[1,1]]");
    auto val = p.Result();

    bool b{ false };

    std::cout << "Input: " << *val << "\n";
    val = SplitNumber(std::move(val), b);
    std::cout << "Result: " << *val << "\n";
    val = SplitNumber(std::move(val), b);
    std::cout << "Result: " << *val << "\n";
}

void TestFour()
{
    std::cout << "Test four\n";

    Parser p1("[[[[4,3],4],4],[7,[[8,4],9]]]");
    auto val1 = p1.Result();

    Parser p2("[1,1]");
    auto val2 = p2.Result();

    std::cout << "Input: " << *val1 << " + " << *val2 << "\n";
    auto added = AddFishNumbers(std::move(val1), std::move(val2));
    std::cout << "Result: " << *added << "\n";
}

auto ReadInput()
{
    std::vector<std::unique_ptr<FishNumber>> result;

    std::string input;
    while (std::getline(std::cin, input))
    {
        if (input.empty())
        {
            break;
        }
        Parser p{ input };
        result.push_back(p.Result());
    }
    return result;
}

int main()
{
    //TestOne();
    //TestTwo();
    //TestThree();
    //TestFour();
    auto input = ReadInput();

    auto accum = input[0]->Copy();
    for (int i = 1; i < input.size(); ++i)
    {
        accum = AddFishNumbers(std::move(accum), input[i]->Copy());
    }

    std::cout << "\nPart one\nResult: " << *accum << "\n";
    std::cout << "Magnitude: " << accum->Magnitude() << "\n";

    uint64_t largestPairMagnitude = 0;
    for (int i = 0; i < input.size() - 1; ++i)
    {
        for (int j = i + 1; j < input.size(); ++j)
        {
            auto addFirst = AddFishNumbers(input[i]->Copy(), input[j]->Copy());
            auto firstMagnitude = addFirst->Magnitude();
            if (firstMagnitude > largestPairMagnitude)
            {
                largestPairMagnitude = firstMagnitude;
            }

            auto secondAdd = AddFishNumbers(input[j]->Copy(), input[i]->Copy());
            auto secondMagnitude = secondAdd->Magnitude();
            if (secondMagnitude > largestPairMagnitude)
            {
                largestPairMagnitude = secondMagnitude;
            }
        }
    }

    std::cout << "\nPart two\nLargest pair-wise magnitude: " << largestPairMagnitude << "\n";
}
