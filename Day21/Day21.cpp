#include <array>
#include <iostream>
#include <map>
#include <regex>
#include <string>

namespace PartOne
{
    class DeterministicDice
    {
    public:
        DeterministicDice(int sides)
            : sides_{ sides },
            nextRoll_{ 1 },
            rollCount_{ 0 }
        {}

        int Roll() noexcept
        {
            int result = nextRoll_;
            ++nextRoll_;
            ++rollCount_;
            if (nextRoll_ > sides_)
            {
                nextRoll_ = 1;
            }
            return result;
        }

        int TimesRolled() const noexcept
        {
            return rollCount_;
        }

    private:
        int const sides_;
        int nextRoll_;
        int rollCount_;
    };

    struct Player
    {
        int position;
        int score;
    };

    void TakeTurn(Player& p, DeterministicDice& d)
    {
        int totalRoll = 0;
        for (int i = 0; i < 3; ++i)
        {
            totalRoll += d.Roll();
        }
        p.position += totalRoll % 10;
        if (p.position > 10)
        {
            p.position %= 10;
        }
        p.score += p.position;
    }

    void Run(int playerOnePos, int playerTwoPos)
    {
        Player one{ playerOnePos, 0 };
        Player two{ playerTwoPos, 0 };

        DeterministicDice dice(100);

        Player winner;
        Player loser;

        while (true)
        {
            TakeTurn(one, dice);
            if (one.score >= 1000)
            {
                winner = one;
                loser = two;
                break;
            }
            TakeTurn(two, dice);
            if (two.score >= 1000)
            {
                winner = two;
                loser = one;
                break;
            }
        }

        std::cout << "Loser score multiplied by roll count: " << loser.score * dice.TimesRolled() << "\n";
    }
}

namespace PartTwo
{
    struct Player
    {
        int position;
        int score;
        auto operator<=>(const Player&) const = default;
    };
    struct Game
    {
        Player one;
        Player two;
        auto operator<=>(const Game&) const = default;
    };
    using GameFreqs = std::map<Game, uint64_t>;

    Player MovePlayer(Player const& p, int roll)
    {
        Player next{ p.position + roll, p.score };
        if (next.position > 10)
        {
            next.position = next.position % 10;
        }
        next.score += next.position;
        return next;
    }

    Game MakeMove(Game const& game, int roll, bool playerOne)
    {
        if (playerOne)
        {
            Player one = MovePlayer(game.one, roll);
            return { one, game.two };
        }
        else
        {
            Player two = MovePlayer(game.two, roll);
            return { game.one, two };
        }
    }

    auto Move(GameFreqs const &games, bool playerOne)
    {
        GameFreqs nextGames;
        uint64_t wins = 0;

        static std::map<int, int> const waysToRoll = {
            {3, 1},
            {4, 3},
            {5, 6},
            {6, 7},
            {7, 6},
            {8, 3},
            {9, 1}
        };

        for (auto const& [game, count] : games)
        {
            for (auto const& [roll, ways] : waysToRoll)
            {
                auto newGameState = MakeMove(game, roll, playerOne);
                auto newCount = count * ways;
                if (newGameState.one.score > 20 || newGameState.two.score > 20)
                {
                    wins += newCount;
                }
                else
                {
                    if (nextGames.contains(newGameState))
                    {
                        nextGames[newGameState] += newCount;
                    }
                    else
                    {
                        nextGames[newGameState] = newCount;
                    }
                }
            }
        }
        return std::make_pair(wins, nextGames);
    }

    void Run(int playerOnePos, int playerTwoPos)
    {
        GameFreqs gameCounts;

        Game initial{ {playerOnePos, 0}, {playerTwoPos, 0} };
        gameCounts[initial] = 1;

        std::array<uint64_t, 2> playerWins{ 0, 0 };
        bool playerOneTurn = true;

        while (!gameCounts.empty())
        {
            auto moveResult = Move(gameCounts, playerOneTurn);
            if (playerOneTurn)
            {
                playerWins[0] += moveResult.first;
            }
            else
            {
                playerWins[1] += moveResult.first;
            }
            playerOneTurn = !playerOneTurn;
            gameCounts = moveResult.second;
        }

        std::cout << "Player one won " << playerWins[0] << " times\n";
        std::cout << "Player two won " << playerWins[1] << " times\n";
    }
}


int ReadPlayerPosition()
{
    std::string line;
    std::getline(std::cin, line);

    std::regex posRegex("Player \\d starting position: (\\d)");
    std::smatch matches;
    if (std::regex_match(line, matches, posRegex))
    {
        if (matches.size() == 2)
        {
            return std::stoi(matches[1].str());
        }
    }
    return -1;
}

int main()
{
    int playerOnePos = ReadPlayerPosition();
    int playerTwoPos = ReadPlayerPosition();

    PartOne::Run(playerOnePos, playerTwoPos);

    PartTwo::Run(playerOnePos, playerTwoPos);
}
