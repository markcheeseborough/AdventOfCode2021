#include <algorithm>
#include <array>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>

struct BingoNumber
{
    int value = 0;
    bool marked = false;
};

struct BingoCard
{
    std::array<std::array<BingoNumber, 5>, 5> numbers = { 0 };

    bool HasWinningRow() const
    {
        for (int row = 0; row < 5; ++row)
        {
            bool win = true;
            for (int col = 0; col < 5; ++col)
            {
                win = win && numbers[row][col].marked;
            }
            if (win)
            {
                return true;
            }
        }
        return false;
    }

    bool HasWinningColumn() const
    {
        for (int col = 0; col < 5; ++col)
        {
            bool win = true;
            for (int row = 0; row < 5; ++row)
            {
                win = win && numbers[row][col].marked;
            }
            if (win)
            {
                return true;
            }
        }
        return false;
    }

    bool HasWon() const
    {
        return HasWinningRow() || HasWinningColumn();
    }

    void MarkNumber(int val)
    {
        for (int row = 0; row < 5; ++row)
        {
            for (int col = 0; col < 5; ++col)
            {
                if (numbers[row][col].value == val)
                {
                    numbers[row][col].marked = true;
                    return;
                }
            }
        }
    }

    int SumUnmarkedNumbers() const
    {
        int sum = 0;
        for (int row = 0; row < 5; ++row)
        {
            for (int col = 0; col < 5; ++col)
            {
                if (!numbers[row][col].marked)
                {
                    sum += numbers[row][col].value;
                }
            }
        }
        return sum;
    }
};

std::vector<int> ReadBingoNumbers(std::string const &input)
{
    std::stringstream in{ input };

    std::vector<int> result;
    while (in)
    {
        int val;
        in >> val;
        result.push_back(val);
        char ignore;
        if (in.peek() == ',')
        {
            in >> ignore;
        }
        else
        {
            break;
        }
    }
    return result;
}

std::vector<BingoCard> ReadBingoCards()
{
    std::vector<BingoCard> cards;

    bool inputGood = true;
    while (inputGood)
    {
        BingoCard card;
        for (int row = 0; row < 5 && inputGood; ++row)
        {
            for (int col = 0; col < 5 && inputGood; ++col)
            {
                int val;
                std::cin >> val;
                card.numbers[row][col].value = val;
                if (std::cin.eof())
                {
                    inputGood = false;
                }
            }
        }
        if (inputGood)
        {
            cards.push_back(card);
        }
    }

    return cards;
}

void FindFirstWinner(std::vector<int> const& numbers, std::vector<BingoCard> cards)
{
    for (auto const v : numbers)
    {
        std::for_each(cards.begin(), cards.end(),
            [v](BingoCard& card)
            {
                card.MarkNumber(v);
            });

        auto winner = std::find_if(cards.begin(), cards.end(),
            [](BingoCard const& card)
            {
                return card.HasWon();
            });
        if (winner != cards.end())
        {
            int sumUnmarked = winner->SumUnmarkedNumbers();
            std::cout << "First winning card score is " << sumUnmarked * v << "\n";
            break;
        }
    }
}

void FindLastWinner(std::vector<int> const& numbers, std::vector<BingoCard> cards)
{
    std::vector<BingoCard>::iterator lastCard = cards.end();
    for (auto const v : numbers)
    {
        auto cardsRemaining = std::count_if(cards.begin(), cards.end(),
            [](BingoCard const& card)
            {
                return !card.HasWon();
            });
        if (cardsRemaining == 1)
        {
            lastCard = std::find_if(cards.begin(), cards.end(),
                [](BingoCard const& card)
                {
                    return !card.HasWon();
                });
        }
        std::for_each(cards.begin(), cards.end(),
            [v](BingoCard& card)
            {
                card.MarkNumber(v);
            });

        if (lastCard != cards.end() && lastCard->HasWon())
        {
            int sumUnmarked = lastCard->SumUnmarkedNumbers();
            std::cout << "Last winning card score is " << sumUnmarked * v << "\n";
            break;
        }
    }
}

int main()
{
    std::string bingoNumbers;
    std::getline(std::cin, bingoNumbers);
    auto numbers = ReadBingoNumbers(bingoNumbers);

    auto cards = ReadBingoCards();

    FindFirstWinner(numbers, cards);
    FindLastWinner(numbers, cards);
}
