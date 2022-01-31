#include <algorithm>
#include <array>
#include <iostream>
#include <optional>
#include <map>
#include <set>
#include <vector>

enum class AmphipodType
{
    Unknown = -1,
    Amber,
    Bronze,
    Copper,
    Desert
};

enum class NodeType
{
    Unknown = -1,
    Corridor,
    AmberBurrow,
    BronzeBurrow,
    CopperBurrow,
    DesertBurrow
};

template<int CritterCount>
struct WorldState
{
    std::array<std::pair<AmphipodType, int>, CritterCount> amphipodPositions;
    auto operator<=>(const WorldState&) const = default;

    std::optional<AmphipodType> TypeInPosition(int position) const noexcept
    {
        for (auto const& [type, pos] : amphipodPositions)
        {
            if (pos == position)
            {
                return type;
            }
        }
        return {};
    }
};

template<int CritterCount>
using WorldStateEnergy = std::map<WorldState<CritterCount>, uint64_t>;


template<int CritterCount>
void InsertWorldConfiguration(WorldStateEnergy<CritterCount>& stateEnergy, WorldState<CritterCount> const& state, uint64_t newEnergy)
{
    if (stateEnergy.contains(state))
    {
        stateEnergy[state] = std::min(stateEnergy[state], newEnergy);
    }
    else
    {
        stateEnergy.insert(std::make_pair(state, newEnergy));
    }
}

bool AmphipodInCorrectBurrow(AmphipodType amphipod, NodeType node)
{
    switch (amphipod)
    {
    case AmphipodType::Amber:
        return node == NodeType::AmberBurrow;
    case AmphipodType::Bronze:
        return node == NodeType::BronzeBurrow;
    case AmphipodType::Copper:
        return node == NodeType::CopperBurrow;
    case AmphipodType::Desert:
        return node == NodeType::DesertBurrow;
    }
    return false;
}

int AmphipodMoveCost(AmphipodType amphipod)
{
    switch (amphipod)
    {
    case AmphipodType::Amber:
        return 1;
    case AmphipodType::Bronze:
        return 10;
    case AmphipodType::Copper:
        return 100;
    case AmphipodType::Desert:
        return 1000;
    }
    return 0;
}

NodeType BurrowForAmphipod(AmphipodType type)
{
    switch (type)
    {
    case AmphipodType::Amber:
        return NodeType::AmberBurrow;
    case AmphipodType::Bronze:
        return NodeType::BronzeBurrow;
    case AmphipodType::Copper:
        return NodeType::CopperBurrow;
    case AmphipodType::Desert:
        return NodeType::DesertBurrow;
    default:
        return NodeType::Unknown;
    }
}

AmphipodType AmphipodForBurrow(NodeType type)
{
    switch (type)
    {
    case NodeType::Corridor:
        return AmphipodType::Unknown;
    case NodeType::AmberBurrow:
        return AmphipodType::Amber;
    case NodeType::BronzeBurrow:
        return AmphipodType::Bronze;
    case NodeType::CopperBurrow:
        return AmphipodType::Copper;
    case NodeType::DesertBurrow:
        return AmphipodType::Desert;
    default:
        return AmphipodType::Unknown;
    }
}

template<int CritterCount>
class World final
{
    const int BurrowSize = CritterCount / 4;
public:
    World()
    {
        for (int i = 0; i < 11; ++i)
        {
            _nodes[i] = NodeType::Corridor;
        }

        for (int i = 0; i < 10; ++i)
        {
            LinkNodes(i, i + 1);
        }

        auto makeBurrow = [this](NodeType type)
        {
            int burrowStart = BurrowEntryNode(type);
            for (int i = burrowStart; i < burrowStart + BurrowSize - 1; ++i)
            {
                _nodes[i] = type;
                _nodes[i + 1] = type;
                LinkNodes(i, i + 1);
            }
            LinkNodes(burrowStart, BurrowLinkNode(type));
        };
        makeBurrow(NodeType::AmberBurrow);
        makeBurrow(NodeType::BronzeBurrow);
        makeBurrow(NodeType::CopperBurrow);
        makeBurrow(NodeType::DesertBurrow);
    }

    // are all the amphipods in their correct burrows
    bool IsWorldSolved(WorldState<CritterCount> const& world) const noexcept
    {
        for (auto const& a : world.amphipodPositions)
        {
            if (!AmphipodInCorrectBurrow(a.first, _nodes[a.second]))
            {
                return false;
            }
        }
        return true;
    }

    // returns list of new positions and energy cost to get there
    std::vector<std::pair<int, uint64_t>> MovesForAmphipod(WorldState<CritterCount> const& world, int amphipodIndex) const
    {
        auto const& [type, position] = world.amphipodPositions[amphipodIndex];
        bool const inCorridor = _nodes[position] == NodeType::Corridor;
        if (!AmphipodInCorrectBurrow(type, _nodes[position])
            || AllowOtherAmphipodOut(world, position)
            || inCorridor)
        {
            auto target = TargetNode(world, type);
            if (target)
            {
                auto pathToTarget = FindPath(world, position, *target, {}, {});
                if (pathToTarget)
                {
                    auto pathLength = pathToTarget->size() - 1; // starting point is included in path
                    return { {*target, pathLength * AmphipodMoveCost(type)} };
                }
            }
            if (!inCorridor)
            {
                auto nodes = CorridorNodes(world);
                std::vector<std::pair<int, uint64_t>> result;
                for (auto const& n : nodes)
                {
                    auto pathToTarget = FindPath(world, position, n, {}, {});
                    if (pathToTarget)
                    {
                        auto pathLength = pathToTarget->size() - 1; // starting point is included in path
                        result.emplace_back(n, pathLength * AmphipodMoveCost(type));
                    }
                }
                return result;
            }
        }
        return {};
    }

private:
    void LinkNodes(int n1, int n2)
    {
        _nodeLinks.insert(std::make_pair(n1, n2));
        _nodeLinks.insert(std::make_pair(n2, n1));
    }

    int BurrowEntryNode(NodeType type) const noexcept
    {
        return 11 + ((static_cast<int>(type) - 1) * BurrowSize);
    }
    int BurrowLinkNode(NodeType type) const noexcept
    {
        return static_cast<int>(type) * 2;
    }

    bool AllowOtherAmphipodOut(WorldState<CritterCount> const& world, int position) const noexcept
    {
        auto burrowType = _nodes[position];
        if (burrowType == NodeType::Corridor)
        {
            return false;
        }
        auto amphipodType = AmphipodForBurrow(burrowType);

        for (int i = position + 1; i < BurrowEntryNode(burrowType) + BurrowSize; ++i)
            if (world.TypeInPosition(i) != amphipodType)
                return true;
        return false;
    }

    std::optional<int> TargetNode(WorldState<CritterCount> const& world, AmphipodType type) const noexcept
    {
        auto burrowType = BurrowForAmphipod(type);
        auto burrowStart = BurrowEntryNode(burrowType);
        auto burrowEnd = burrowStart + BurrowSize;
        for (int i = burrowEnd - 1; i >= burrowStart; --i)
        {
            auto occupant = world.TypeInPosition(i);
            if (!occupant)
            {
                return i;
            }
            else if (occupant != type)
            {
                break;
            }
        }
        return {};
    }

    std::vector<int> CorridorNodes(WorldState<CritterCount> const& world) const
    {
        std::vector<int> result;
        for (int i = 0; i < 11; ++i)
        {
            if (!world.TypeInPosition(i) && _nodeLinks.count(i) < 3)
            {
                result.push_back(i);
            }
        }
        return result;
    }

    std::optional<std::vector<int>> FindPath(WorldState<CritterCount> const& world, int currentNode, int targetNode, std::vector<int> pathSoFar, std::set<int> visited) const
    {
        pathSoFar.push_back(currentNode);
        visited.insert(currentNode);

        if (currentNode == targetNode)
        {
            return pathSoFar;
        }

        auto const [nextNodesFirst, nextNodesLast] = _nodeLinks.equal_range(currentNode);
        for (auto it = nextNodesFirst; it != nextNodesLast; ++it)
        {
            if (!visited.contains(it->second) && !world.TypeInPosition(it->second))
            {
                auto nextPath = FindPath(world, it->second, targetNode, pathSoFar, visited);
                if (nextPath)
                {
                    return nextPath;
                }
            }
        }
        return {};
    }

    std::array<NodeType, 11 + CritterCount> _nodes;
    std::multimap<int, int> _nodeLinks;
};

template<int CritterCount>
WorldStateEnergy<CritterCount> FindEnergyRequired(World<CritterCount> const& world, WorldState<CritterCount> initial)
{
    WorldStateEnergy<CritterCount> currentGeneration;
    currentGeneration.insert({ initial, 0 });
    WorldStateEnergy<CritterCount> solved;

    while (std::any_of(currentGeneration.begin(), currentGeneration.end(), [&world](auto const& config) { return !world.IsWorldSolved(config.first); }))
    {
        WorldStateEnergy<CritterCount> nextGeneration;
        for (auto const& config : currentGeneration)
        {
            if (world.IsWorldSolved(config.first))
            {
                InsertWorldConfiguration(solved, config.first, config.second);
            }
            else
            {
                for (int i = 0; i < CritterCount; ++i)
                {
                    auto newMoves = world.MovesForAmphipod(config.first, i);
                    for (auto const& [position, energy] : newMoves)
                    {
                        WorldState next = config.first;
                        next.amphipodPositions[i].second = position;
                        auto newEnergy = config.second + energy;
                        InsertWorldConfiguration(nextGeneration, next, newEnergy);
                    }
                }
            }
        }
        currentGeneration = nextGeneration;
    }
    return solved;
}

void PartOne()
{
    constexpr static int CritterCount = 8;
    World<CritterCount> theWorld;

    WorldState<CritterCount> initial;
    initial.amphipodPositions[0] = { AmphipodType::Desert, 11 };
    initial.amphipodPositions[1] = { AmphipodType::Copper, 12 };
    initial.amphipodPositions[2] = { AmphipodType::Copper, 13 };
    initial.amphipodPositions[3] = { AmphipodType::Desert, 14 };
    initial.amphipodPositions[4] = { AmphipodType::Amber, 15 };
    initial.amphipodPositions[5] = { AmphipodType::Amber, 16 };
    initial.amphipodPositions[6] = { AmphipodType::Bronze, 17 };
    initial.amphipodPositions[7] = { AmphipodType::Bronze, 18 };

    auto solutions = FindEnergyRequired(theWorld, initial);

    for (auto const& configuration : solutions)
    {
        std::cout << "Part one - Solution using " << configuration.second << " energy.\n";
    }
}

void PartTwo()
{
    constexpr static int CritterCount = 16;
    World<CritterCount> theWorld;

    WorldState<CritterCount> initial;
    initial.amphipodPositions[0] = { AmphipodType::Desert, 11 };
    initial.amphipodPositions[1] = { AmphipodType::Desert, 12 };
    initial.amphipodPositions[2] = { AmphipodType::Desert, 13 };
    initial.amphipodPositions[3] = { AmphipodType::Copper, 14 };

    initial.amphipodPositions[4] = { AmphipodType::Copper, 15 };
    initial.amphipodPositions[5] = { AmphipodType::Copper, 16 };
    initial.amphipodPositions[6] = { AmphipodType::Bronze, 17 };
    initial.amphipodPositions[7] = { AmphipodType::Desert, 18 };

    initial.amphipodPositions[8] = { AmphipodType::Amber, 19 };
    initial.amphipodPositions[9] = { AmphipodType::Bronze, 20 };
    initial.amphipodPositions[10] = { AmphipodType::Amber, 21 };
    initial.amphipodPositions[11] = { AmphipodType::Amber, 22 };

    initial.amphipodPositions[12] = { AmphipodType::Bronze, 23 };
    initial.amphipodPositions[13] = { AmphipodType::Amber, 24 };
    initial.amphipodPositions[14] = { AmphipodType::Copper, 25 };
    initial.amphipodPositions[15] = { AmphipodType::Bronze, 26 };

    auto solutions = FindEnergyRequired(theWorld, initial);

    for (auto const& configuration : solutions)
    {
        std::cout << "Part two - Solution using " << configuration.second << " energy.\n";
    }
}

int main()
{
    PartOne();
    PartTwo();
}
