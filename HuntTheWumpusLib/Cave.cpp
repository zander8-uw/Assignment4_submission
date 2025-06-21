#include "Cave.h"
#include "Denizen.h"

// ReSharper disable once CppUnusedIncludeDirective
#include <algorithm>
#include <ranges>

namespace HuntTheWumpus
{
    bool CompareThings::operator()(const std::shared_ptr<Denizen>& thing1, const std::shared_ptr<Denizen>& thing2) const
    {
        return thing1->GetPriority() > thing2->GetPriority();
    }

    Cave::Cave(const int caveId, IDungeon& dungeon)
        : m_caveId(caveId),
        m_dungeon(dungeon)
    {
    }

    void Cave::ConnectTo(const std::shared_ptr<Cave>& destination)
    {
        m_tunnels.emplace(destination->GetCaveId(), destination);
    }

    const std::weak_ptr<Cave>& Cave::GetConnectedCave(const int caveId)
    {
        return m_tunnels.at(caveId);
    }

    void Cave::AddDenizen(const std::shared_ptr<Denizen>& newDenizen, const bool observeEntrance)
    {
        m_denizens.emplace(newDenizen);

        if (observeEntrance)
        {
            for (auto&& denizen : m_denizens)
            {
                if(denizen->ObserveCaveEntrance(newDenizen))
                {
                    // Stop if that denizen affected things.
                    break;
                }
            }
        }
    }

    void Cave::RemoveDenizen(const DenizenIdentifier& identifier)
    {
        m_denizens.erase( std::ranges::find_if(m_denizens, [&](const auto& thing) { return (thing->GetIdentifier() <=> identifier) == std::strong_ordering::equal; }));
    }

    std::vector<int> Cave::GetConnectedIds() const
    {
        std::vector<int> caveIds;
        caveIds.reserve(m_tunnels.size());

        for (const auto& caveId : m_tunnels | std::views::keys)
        {
            caveIds.emplace_back(caveId);
        }

        return caveIds;
    }

    bool Cave::HasDenizen(const DenizenIdentifier &identifier) const
    {
        return std::ranges::find_if(m_denizens, [&](const auto &denizen)
        {
            return denizen->GetIdentifier().m_category == identifier.m_category;
        }) != m_denizens.end();
    }
}
