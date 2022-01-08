#include <iostream>
#include <list>
#include <regex>
#include <string>
#include <vector>

enum class State
{
    Off,
    On
};

struct RebootStep
{
    State state;
    int minX;
    int maxX;
    int minY;
    int maxY;
    int minZ;
    int maxZ;
};

struct Point
{
    int x;
    int y;
    int z;
    auto operator<=>(const Point&) const = default;
};
Point operator-(Point const& a, Point const& b)
{
    return { a.x - b.x, a.y - b.y, a.z - b.z };
}
Point operator+(Point const& a, Point const& b)
{
    return { a.x + b.x, a.y + b.y, a.z + b.z };
}

class Reactor
{
public:
    Reactor()
        : cubes(101, std::vector<std::vector<State>>(101, std::vector<State>(101, State::Off)))
    {}

    void ApplyStep(RebootStep const& step)
    {
        if (step.minX > 50 || step.maxX < -50)
            return;
        if (step.minY > 50 || step.maxY < -50)
            return;
        if (step.minZ > 50 || step.maxZ < -50)
            return;

        for (int x = step.minX; x <= step.maxX; ++x)
        {
            for (int y = step.minY; y <= step.maxY; ++y)
            {
                for (int z = step.minZ; z <= step.maxZ; ++z)
                {
                    SetCubeState(x, y, z, step.state);
                }
            }
        }
    }

    int OnCubeCount() const noexcept
    {
        int onCount = 0;
        for (auto const& plane : cubes)
        {
            for (auto const& row : plane)
            {
                for (auto const& c : row)
                {
                    onCount += c == State::On ? 1 : 0;
                }
            }
        }
        return onCount;
    }

private:
    void SetCubeState(int x, int y, int z, State s)
    {
        if (x < -50 || 50 < x)
            return;
        if (y < -50 || 50 < y)
            return;
        if (z < -50 || 50 < z)
            return;

        cubes[x + 50][y + 50][z + 50] = s;
    }

    std::vector<std::vector<std::vector<State>>> cubes;
};

class Chunk
{
public:
    Chunk(int minX, int maxX, int minY, int maxY, int minZ, int maxZ)
        : origin{ minX, minY, minZ },
        size{ Point{ maxX, maxY, maxZ } - origin }
    {}

    bool Intersects(Chunk const& other) const noexcept
    {
        auto otherPos = other.origin;
        auto otherTop = other.origin + other.size;
        auto myTop = origin + size;

        return (otherTop.x >= origin.x && otherPos.x <= myTop.x) &&
            (otherTop.y >= origin.y && otherPos.y <= myTop.y) &&
            (otherTop.z >= origin.z && otherPos.z <= myTop.z);
    }

    uint64_t BlockSize() const noexcept
    {
        return (static_cast<uint64_t>(size.x) + 1) * (static_cast<uint64_t>(size.y) + 1) * (static_cast<uint64_t>(size.z) + 1);
    }

    friend std::vector<Chunk> CutChunkToFit(Chunk const& toFit, Chunk const& toCut);

private:
    Point origin;
    Point size;
};

std::vector<Chunk> CutChunkToFit(Chunk const& toFit, Chunk const& toCut)
{
    Chunk remaining = toCut;
    std::vector<Chunk> result;

    if (remaining.origin.x < toFit.origin.x)
    {
        auto newSize = (toFit.origin.x - remaining.origin.x) - 1;
        Chunk newChunk(remaining.origin.x, remaining.origin.x + newSize, remaining.origin.y, remaining.origin.y + remaining.size.y, remaining.origin.z, remaining.origin.z + remaining.size.z);
        result.push_back(newChunk);
        remaining.origin.x += newSize + 1;
        remaining.size.x -= newSize + 1;
    }
    if (remaining.origin.x + remaining.size.x > toFit.origin.x + toFit.size.x)
    {
        auto newSize = (remaining.origin.x + remaining.size.x) - (toFit.origin.x + toFit.size.x);
        Chunk newChunk(toFit.origin.x + toFit.size.x + 1, remaining.origin.x + remaining.size.x, remaining.origin.y, remaining.origin.y + remaining.size.y, remaining.origin.z, remaining.origin.z + remaining.size.z);
        result.push_back(newChunk);
        remaining.size.x -= newSize;
    }
    if (remaining.origin.y < toFit.origin.y)
    {
        auto newSize = (toFit.origin.y - remaining.origin.y) - 1;
        Chunk newChunk(remaining.origin.x, remaining.origin.x + remaining.size.x, remaining.origin.y, remaining.origin.y + newSize, remaining.origin.z, remaining.origin.z + remaining.size.z);
        result.push_back(newChunk);
        remaining.origin.y += newSize + 1;
        remaining.size.y -= newSize + 1;
    }
    if (remaining.origin.y + remaining.size.y > toFit.origin.y + toFit.size.y)
    {
        auto newSize = (remaining.origin.y + remaining.size.y) - (toFit.origin.y + toFit.size.y);
        Chunk newChunk(remaining.origin.x, remaining.origin.x + remaining.size.x, toFit.origin.y + toFit.size.y + 1, remaining.origin.y + remaining.size.y, remaining.origin.z, remaining.origin.z + remaining.size.z);
        result.push_back(newChunk);
        remaining.size.y -= newSize;
    }
    if (remaining.origin.z < toFit.origin.z)
    {
        auto newSize = (toFit.origin.z - remaining.origin.z) - 1;
        Chunk newChunk(remaining.origin.x, remaining.origin.x + remaining.size.x, remaining.origin.y, remaining.origin.y + remaining.size.y, remaining.origin.z, remaining.origin.z + newSize);
        result.push_back(newChunk);
        remaining.origin.z += newSize + 1;
        remaining.size.z -= newSize + 1;
    }
    if (remaining.origin.z + remaining.size.z > toFit.origin.z + toFit.size.z)
    {
        auto newSize = (remaining.origin.z + remaining.size.z) - (toFit.origin.z + toFit.size.z);
        Chunk newChunk(remaining.origin.x, remaining.origin.x + remaining.size.x, remaining.origin.y, remaining.origin.y + remaining.size.y, toFit.origin.z + toFit.size.z + 1, remaining.origin.z + remaining.size.z);
        result.push_back(newChunk);
        remaining.size.z -= newSize;
    }

    return result;
}

std::vector<RebootStep> ReadInput()
{
    std::regex stepRegex(R"((on|off) x=(-?\d+)\.\.(-?\d+),y=(-?\d+)\.\.(-?\d+),z=(-?\d+)\.\.(-?\d+))");

    std::vector<RebootStep> steps;

    std::string line;
    while (std::getline(std::cin, line))
    {
        if (line.empty())
            break;

        std::smatch matches;
        if (std::regex_match(line, matches, stepRegex))
        {
            if (matches.size() == 8)
            {
                auto state = matches[1].str() == "on" ? State::On : State::Off;
                auto minX = std::stoi(matches[2].str());
                auto maxX = std::stoi(matches[3].str());
                auto minY = std::stoi(matches[4].str());
                auto maxY = std::stoi(matches[5].str());
                auto minZ = std::stoi(matches[6].str());
                auto maxZ = std::stoi(matches[7].str());
                steps.emplace_back(state, minX, maxX, minY, maxY, minZ, maxZ);
            }
        }
    }
    return steps;
}

void RunPartOne(std::vector<RebootStep> const& rebootSteps)
{
    Reactor r;
    for (auto const& step : rebootSteps)
    {
        r.ApplyStep(step);
    }

    std::cout << "Part one: After initialisation " << r.OnCubeCount() << " cubes are turned on.\n\n";
}

void RunPartTwo(std::vector<RebootStep> const& rebootSteps)
{
    std::vector<Chunk> onChunks;

    for (auto const& step : rebootSteps)
    {
        if (step.state == State::On)
        {
            Chunk newChunk(step.minX, step.maxX, step.minY, step.maxY, step.minZ, step.maxZ);
            std::list<Chunk> toPlace{ newChunk };
            for (auto const& placed : onChunks)
            {
                for (auto it = toPlace.begin(); it != toPlace.end(); )
                {
                    if (placed.Intersects(*it))
                    {
                        auto chunkToCut = *it;
                        it = toPlace.erase(it);
                        auto replacementChunks = CutChunkToFit(placed, chunkToCut);
                        toPlace.insert(toPlace.end(), replacementChunks.begin(), replacementChunks.end());
                    }
                    else
                    {
                        ++it;
                    }
                }
            }
            onChunks.insert(onChunks.end(), toPlace.begin(), toPlace.end());
        }
        else
        {
            Chunk offChunk(step.minX, step.maxX, step.minY, step.maxY, step.minZ, step.maxZ);
            std::vector<Chunk> survivingChunks;
            for (auto const& c : onChunks)
            {
                if (offChunk.Intersects(c))
                {
                    auto left = CutChunkToFit(offChunk, c);
                    survivingChunks.insert(survivingChunks.end(), left.begin(), left.end());
                }
                else
                {
                    survivingChunks.push_back(c);
                }
            }
            onChunks = survivingChunks;
        }
    }


    uint64_t totalSize = 0;
    for (auto const& c : onChunks)
    {
        totalSize += c.BlockSize();
    }
    std::cout << "Part two: After full setup " << totalSize << " cubes are turned on.\n\n";
}

int main()
{
    auto rebootSteps = ReadInput();

    RunPartOne(rebootSteps);
    RunPartTwo(rebootSteps);
}
