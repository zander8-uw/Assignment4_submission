#include <TestHarness.h>

#include "Arrow.h"

#include "TestHelperTestEnvironment.h"

namespace TestHuntTheWumpus
{
    TEST(ArrowSuite, Arrow_HasProperAttributes)
    {
        TestEnvironment env;

        const HuntTheWumpus::Arrow arrow(0, env.m_context);

        const auto &properties = arrow.Properties();

        CHECK(!properties.m_carryableByBats);
        CHECK(!properties.m_isEdible);
        CHECK(!properties.m_reportMovement);
        CHECK(properties.m_fatalToHunter);
        CHECK(properties.m_fatalToWumpus);

        CHECK_EQUAL(2, arrow.GetPriority());

        const auto& id = arrow.GetIdentifier();

        CHECK_EQUAL(HuntTheWumpus::Category::Arrow, id.m_category);
    }

    TEST(ArrowSuite, OutOfArrows_callback_test)
    {
        TestEnvironment env;
        env.m_state.m_isPlayingResult = true;

        env.m_provider.SetCaveSequence({ 1, 2, 3, 4, 5, 6 });

        HuntTheWumpus::Dungeon dungeon(env.m_context);

        // Verify Bats uniquely distributed
        const auto cave1 = dungeon.FindCave(1);
        const auto cave2 = dungeon.FindCave(2);
        const auto cave6 = dungeon.FindCave(6);
        CHECK(cave1->HasDenizen({ HuntTheWumpus::Category::Bat, 0 }));
        CHECK(cave2->HasDenizen({ HuntTheWumpus::Category::Bat, 1 }));
        CHECK(cave6->HasDenizen({ HuntTheWumpus::Category::Hunter, 0 }));
        const std::vector<int>& destinationIds = {15, 16, 17, 18, 19};

        for (int i = 0; i < 5; i++)
        {
            dungeon.MakeMove(HuntTheWumpus::DungeonMove::Shoot, destinationIds); // fire 5 arrows so the hunter is out
        }

        CHECK(env.m_testNotifications.observeOutOfArrowsCheck); // Checks if callback happened
    }

    TEST(ArrowSuite, Arrow_Kills_Hunter)
    {
        TestEnvironment env;
        env.m_state.m_isPlayingResult = true;

        env.m_provider.SetCaveSequence({ 1, 2, 3, 4, 5, 6 });

        HuntTheWumpus::Dungeon dungeon(env.m_context);

        // Verify Bats uniquely distributed
        const auto cave1 = dungeon.FindCave(1);
        const auto cave2 = dungeon.FindCave(2);
        const auto cave6 = dungeon.FindCave(6);
        CHECK(cave1->HasDenizen({ HuntTheWumpus::Category::Bat, 0 }));
        CHECK(cave2->HasDenizen({ HuntTheWumpus::Category::Bat, 1 }));
        CHECK(cave6->HasDenizen({ HuntTheWumpus::Category::Hunter, 0 }));
        const std::vector<int>& destinationIds = { 7, 8, 1, 5, 6 };     // Hunter will shoot an arrow in a circle and hit himself

        dungeon.MakeMove(HuntTheWumpus::DungeonMove::Shoot, destinationIds); 

        CHECK(env.m_testNotifications.hunterShotCheck); // Checks if callback happened
    }

}

