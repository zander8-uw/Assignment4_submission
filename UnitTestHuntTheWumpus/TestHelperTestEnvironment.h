#pragma once

#include "TestHelperGameState.h"
#include "TestHelperRandomProvider.h"
#include "TestHelperDungeon.h"
#include "UserNotification.h"

#include "Context.h"

namespace TestHuntTheWumpus
{
    struct TestNotificationChecks
    {
        bool observeWumpusCheck = false;
        bool observePitCheck = false;
        bool observeBatCheck = false;
        bool observeMissCheck = false;
        bool observeOutOfArrowsCheck = false;
        bool batTriggeredCheck = false;
        bool pitTriggeredCheck = false;
        bool wumpusTriggeredCheck = false;
        bool wumpusAwokenCheck = false;
        bool wumpusShotCheck = false;
        bool hunterEatenCheck = false;
        bool hunterShotCheck = false;
        bool caveEnteredCheck = false;
        bool reportIllegalMoveCheck = false;
        bool reportNeighboringCavesCheck = false;
    };

    struct TestEnvironment
    {
        TestEnvironment();
        void MakeTestNotifications();

        TestRandomProvider m_provider;
        HuntTheWumpus::UserNotification m_userNotifier;

        TestGameState m_state;

        HuntTheWumpus::Context m_context{ .m_notification = m_userNotifier, .m_random = m_provider, .m_change = m_state };

        TestDungeon m_dungeon;

        TestNotificationChecks m_testNotifications;
    };
}
