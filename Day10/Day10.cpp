#include <algorithm>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

enum class Token
{
    END,
    L_PARAN = '(',
    R_PARAN = ')',
    L_BRACE = '{',
    R_BRACE = '}',
    L_BRACKET = '[',
    R_BRACKET = ']',
    L_ANGLE = '<',
    R_ANGLE = '>'
};

class Lexer
{
public:
    explicit Lexer(std::istream &input)
        : input_{ input }
    {}

    Token GetToken()
    {
        char ch = 0;
        do
        {
            if (!input_.get(ch))
            {
                return Token::END;
            }
        } while (ch != '\n' && isspace(ch));

        switch (ch)
        {
        case '(':
        case ')':
        case '{':
        case '}':
        case '[':
        case ']':
        case '<':
        case '>':
            return Token{ ch };
        default:
            throw std::runtime_error(std::string{ "Bad token in input: " } + ch);
        }
    }

private:
    std::istream& input_;
};

struct ParseResult
{
    bool stop;
    bool isError;
    Token lastToken;
    std::vector<Token> completion_;
};

class Parser
{
public:
    explicit Parser(std::string const& input)
        : input_{ input },
        inputStream_{ input_ },
        lexer_{ inputStream_ },
        currentToken_{ Token::END }
    {}

    ParseResult Result()
    {
        currentToken_ = lexer_.GetToken();

        ParseResult result{};
        do
        {
            result = MatchBrackets(MatchingClose(currentToken_));
        } while (!result.stop);

        return result;
    }

private:
    bool IsOpen(Token t)
    {
        switch (t)
        {
        case Token::L_PARAN:
        case Token::L_BRACE:
        case Token::L_BRACKET:
        case Token::L_ANGLE:
            return true;
        default:
            return false;
        }
    }

    Token MatchingClose(Token t)
    {
        switch (t)
        {
        case Token::L_PARAN:
            return Token::R_PARAN;
        case Token::L_BRACE:
            return Token::R_BRACE;
        case Token::L_BRACKET:
            return Token::R_BRACKET;
        case Token::L_ANGLE:
            return Token::R_ANGLE;
        default:
            return Token::END;
        }
    }

    ParseResult MatchBrackets(Token expectedClose)
    {
        currentToken_ = lexer_.GetToken();
        if (currentToken_ == Token::END)
        {
            return { true, false, currentToken_, { expectedClose } };
        }

        while (IsOpen(currentToken_))
        {
            auto expectClose = MatchingClose(currentToken_);
            auto res = MatchBrackets(expectClose);
            if (res.stop)
            {
                res.completion_.push_back(expectedClose);
                return res;
            }
        }
        if (currentToken_ != expectedClose)
        {
            return { true, true, currentToken_ };
        }
        else
        {
            currentToken_ = lexer_.GetToken();
            if (currentToken_ == Token::END)
            {
                return { true, false, currentToken_, {} };
            }
            else
            {
                return { false, false, currentToken_ };
            }
        }
    }

    std::string input_;
    std::stringstream inputStream_;
    Lexer lexer_;
    Token currentToken_;
};

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

int IllegalTokenScore(Token t)
{
    switch (t)
    {
    case Token::END:
        return 0;
    case Token::L_PARAN:
        return 0;
    case Token::R_PARAN:
        return 3;
    case Token::L_BRACE:
        return 0;
    case Token::R_BRACE:
        return 1197;
    case Token::L_BRACKET:
        return 0;
    case Token::R_BRACKET:
        return 57;
    case Token::L_ANGLE:
        return 0;
    case Token::R_ANGLE:
        return 25137;
    default:
        return 0;
    }
}

int CompletionTokenScore(Token t)
{
    switch (t)
    {
    case Token::END:
        return 0;
    case Token::L_PARAN:
        return 0;
    case Token::R_PARAN:
        return 1;
    case Token::L_BRACE:
        return 0;
    case Token::R_BRACE:
        return 3;
    case Token::L_BRACKET:
        return 0;
    case Token::R_BRACKET:
        return 2;
    case Token::L_ANGLE:
        return 0;
    case Token::R_ANGLE:
        return 4;
    default:
        return 0;
    }
}

uint64_t CompletionScore(std::vector<Token> tokens)
{
    uint64_t score = 0;
    for (auto t : tokens)
    {
        score *= 5;
        score += CompletionTokenScore(t);
    }
    return score;
}

int main()
{
    auto input = ReadInput();

    int totalErrorScore = 0;
    std::vector<uint64_t> completionScores;
    for (auto const& line : input)
    {
        Parser p{ line };
        auto result = p.Result();
        if (result.isError)
        {
            auto score = IllegalTokenScore(result.lastToken);
            totalErrorScore += score;
        }
        else
        {
            auto score = CompletionScore(result.completion_);
            completionScores.push_back(score);
        }
    }

    std::cout << "Total score for error lines: " << totalErrorScore << "\n";

    auto completionCount = completionScores.size();
    std::sort(completionScores.begin(), completionScores.end());
    auto completionResult = completionScores[completionCount / 2];

    std::cout << "Complete score for mid-point: " << completionResult << "\n";
}
