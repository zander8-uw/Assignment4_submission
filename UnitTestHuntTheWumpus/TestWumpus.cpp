#include <TestHarness.h>

#include "Wumpus.h"

#include "Arrow.h"
#include "Bat.h"
#include "Cave.h"
#include "Hunter.h"

#include "TestHelperTestEnvironment.h"

namespace TestHuntTheWumpus
{
    TEST(WumpusSuite, Wumpus_HasProperAttributes)
    {
        TestEnvironment env;

        const HuntTheWumpus::Wumpus wumpus(0, env.m_context);

        const auto &properties = wumpus.Properties();

        CHECK(!properties.m_carryableByBats);
        CHECK(!properties.m_isEdible);
        CHECK(!properties.m_reportMovement);
        CHECK(properties.m_fatalToHunter);
        CHECK(!properties.m_fatalToWumpus);

        CHECK_EQUAL(3, wumpus.GetPriority());

        const auto& id = wumpus.GetIdentifier();

        CHECK_EQUAL(HuntTheWumpus::Category::Wumpus, id.m_category);
    }

    TEST(WumpusSuite, Wumpus_CanBeShotByArrow)
    {
        TestEnvironment env;

        HuntTheWumpus::Wumpus wumpus(0, env.m_context);

        const auto arrow = std::make_shared<HuntTheWumpus::Arrow>(0, env.m_context);

        // This should return true that there was an action taken.
        CHECK(wumpus.ObserveCaveEntrance(arrow));

        // Show that a state-change happened to a "won" result.
        CHECK(env.m_state.m_gameOverCalled);
        CHECK(env.m_state.m_gameOverResult);
    }

    TEST(WumpusSuite, Wumpus_EatsHunter)
    {
        TestEnvironment env;

        HuntTheWumpus::Wumpus wumpus(0, env.m_context);

        const auto hunter = std::make_shared<HuntTheWumpus::Hunter>(env.m_context);

        // Set the probability to be eaten.
        env.m_provider.m_desiredRandomNumber = 0.1f;

        // This should return true that there was an action taken.
        CHECK(wumpus.ObserveCaveEntrance(hunter));

        // Show that a state-change happened to a "lost" result.
        CHECK(env.m_state.m_gameOverCalled);
        CHECK(!env.m_state.m_gameOverResult);
    }

    TEST(WumpusSuite, Wumpus_FleesHunter)
    {
        TestEnvironment env;

        const auto wumpus = std::make_shared<HuntTheWumpus::Wumpus>(0, env.m_context);

        // In order to do this test, the Wumpus needs to be in a cave so that it can move.
        const auto cave = std::make_shared<HuntTheWumpus::Cave>(58, env.m_dungeon);

        wumpus->EnterCave(cave);

        const auto hunter = std::make_shared<HuntTheWumpus::Hunter>(env.m_context);

        // Set the probability to be eaten.
        env.m_provider.m_desiredRandomNumber = 0.5f;

        // This should return true that there was an action taken.
        constexpr HuntTheWumpus::DenizenIdentifier expectedMover{ .m_category = HuntTheWumpus::Category::Wumpus, .m_instance = 0 };

        CHECK(wumpus->ObserveCaveEntrance(hunter));
        CHECK(env.m_dungeon.m_moveDenizenRandomlyTriggered);
        CHECK_EQUAL(expectedMover, env.m_dungeon.m_thingToMove);
        CHECK(!env.m_state.m_gameOverCalled);
    }

    TEST(WumpusSuite, Wumpus_IgnoresBat)
    {
        TestEnvironment env;

        HuntTheWumpus::Wumpus wumpus(0, env.m_context);

        const auto bat = std::make_shared<HuntTheWumpus::Bat>(0, env.m_context);

        // This should return false for no action taken.
        CHECK(!wumpus.ObserveCaveEntrance(bat));
    }
}
