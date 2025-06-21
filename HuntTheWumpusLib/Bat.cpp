#include "Bat.h"

#include "Cave.h"
#include "Context.h"
#include "Dungeon.h"
#include "RandomProvider.h"
#include "GameStateObservation.h"


class GameChange;

namespace HuntTheWumpus
{
    Bat::Bat(const int batInstance, Context& providers)
        : Denizen(
            { .m_category = Category::Bat, .m_instance = batInstance },
            { .m_carryableByBats = false, .m_fatalToWumpus = false, .m_fatalToHunter = false, .m_isEdible = false, .m_reportMovement = false },
            providers)
    {
    }

    bool Bat::ObserveCaveEntrance(const std::shared_ptr<Denizen>& trigger)
    {
        if (trigger->Properties().m_carryableByBats && m_providers.m_change.IsPlaying())
        {
            m_providers.m_notification.Notify(HuntTheWumpus::UserNotification::Notification::BatTriggered);

            const auto cave = m_cave.lock();

            // Carry to another spot.

            const auto caveId = cave->GetCaveId();

            auto newCaveFound = false;
            auto newCaveId = 0;

            while (!newCaveFound)
            {
                newCaveId = m_providers.m_random.MakeRandomCave();
                newCaveFound = newCaveId != caveId;
            }

            cave->GetDungeon().Move(trigger->GetIdentifier(), newCaveId);

            return true;
        }

        return false;
    }
}
