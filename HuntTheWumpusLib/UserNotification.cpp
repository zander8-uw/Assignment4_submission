#include "UserNotification.h"
#include "Dungeon.h"

namespace HuntTheWumpus
{
    void UserNotification::AddCallback([[maybe_unused]] const Notification category, [[maybe_unused]] std::function<void()>&& callback)
    {
        m_callbacks[category] = std::move(callback);
    }

    void UserNotification::Notify(Notification category) const
    {
        m_callbacks.at(category)(); 
    }
}
