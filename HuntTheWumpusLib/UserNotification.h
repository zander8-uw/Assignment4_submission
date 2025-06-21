#pragma once

#include <functional>
#include <iostream>

namespace HuntTheWumpus
{
    // Inform the user about particular operations happening.
    class UserNotification final
    {
    public:
        friend class Dungeon;

        enum class Notification
        {
            ObserveWumpus,
            ObservePit,
            ObserveBat,
            ObserveMiss,
            ObserveOutOfArrows,
            BatTriggered,
            PitTriggered,
            WumpusTriggered,
            WumpusAwoken,
            WumpusShot,
            HunterEaten,
            HunterShot,
            CaveEntered,
            ReportIllegalMove,
            ReportNeighboringCaves
        };

        UserNotification() = default;
        ~UserNotification() = default;

        void AddCallback(Notification category, std::function<void()>&& callback);

        void Notify(Notification category) const;

        UserNotification(const UserNotification&) = default;
        UserNotification(UserNotification&&) = default;
        UserNotification& operator=(const UserNotification&) = default;
        UserNotification& operator=(UserNotification&&) = default;

    private:
        // holds the callbacks
        std::unordered_map<UserNotification::Notification, std::function<void()>> m_callbacks;
 
    };
}
