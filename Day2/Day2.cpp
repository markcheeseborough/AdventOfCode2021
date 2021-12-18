#include <iostream>
#include <sstream>
#include <string>
#include <vector>

enum class Direction
{
    Unknown,
    Forward,
    Up,
    Down
};

struct Command
{
    Direction dir;
    int distance;
};

struct Position
{
    int depth;
    int horizontal;
    int aim;
};

Command ParseCommand(std::string const& inputLine)
{
    std::stringstream in{ inputLine };
    std::string dir;
    int distance;
    in >> dir >> distance;

    Direction d = [](std::string const& inDir)
    {
        if (inDir == "forward")
        {
            return Direction::Forward;
        }
        else if (inDir == "up")
        {
            return Direction::Up;
        }
        else if (inDir == "down")
        {
            return Direction::Down;
        }
        else
        {
            return Direction::Unknown;
        }
    }(dir);

    return { d, distance };
}

std::vector<Command> ReadInput()
{
    std::vector<Command> commands;

    std::string input;
    while (std::getline(std::cin, input))
    {
        if (input.empty())
        {
            break;
        }
        auto c = ParseCommand(input);
        commands.push_back(c);
    }
    return commands;
}

Position ApplyCommand(Position currentPos, Command cmd)
{
    switch (cmd.dir)
    {
    case Direction::Forward:
        return { currentPos.depth + (cmd.distance * currentPos.aim), currentPos.horizontal + cmd.distance, currentPos.aim};
    case Direction::Down:
        return { currentPos.depth, currentPos.horizontal, currentPos.aim + cmd.distance };
    case Direction::Up:
        return{ currentPos.depth, currentPos.horizontal, currentPos.aim - cmd.distance };
    default:
        return currentPos;
    }
}

int main()
{
    auto commands = ReadInput();
    Position pos{ 0, 0, 0 };
    for (auto const &c : commands)
    {
        pos = ApplyCommand(pos, c);
    }

    std::cout << "Depth * horizontal distance = " << pos.depth * pos.horizontal << "\n";
}
