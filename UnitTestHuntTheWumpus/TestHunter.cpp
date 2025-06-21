#include <TestHarness.h>

#include "Hunter.h"

#include "Arrow.h"
#include "Wumpus.h"

#include "TestHelperTestEnvironment.h"

namespace TestHuntTheWumpus
{
    TEST(HunterSuite, Hunter_HasProperAttributes)
    {
        TestEnvironment env;

        const HuntTheWumpus::Hunter hunter(env.m_context);

        const auto &properties = hunter.Properties();

        CHECK(properties.m_carryableByBats);
        CHECK(properties.m_isEdible);
        CHECK(properties.m_reportMovement);
        CHECK(!properties.m_fatalToHunter);
        CHECK(!properties.m_fatalToWumpus);

        CHECK_EQUAL(1, hunter.GetPriority());

        const auto& id = hunter.GetIdentifier();

        CHECK_EQUAL(HuntTheWumpus::Category::Hunter, id.m_category);
    }

    TEST(HunterSuite, Hunter_RetrieveArrow_ReturnsValidArrow)
    {
        TestEnvironment env;

        HuntTheWumpus::Hunter hunter(env.m_context);

        const auto arrow = hunter.GetArrow();

        CHECK_EQUAL(HuntTheWumpus::Category::Arrow, arrow->GetIdentifier().m_category);

        CHECK(!hunter.OutOfArrows());
    }

    TEST(HunterSuite, Hunter_RetrieveAllArrows_ReportsEmpty)
    {
        TestEnvironment env;

        HuntTheWumpus::Hunter hunter(env.m_context);

        for (auto idx = 0; idx < 5; ++idx)
        {
            const auto arrow = hunter.GetArrow();
        }

        CHECK(hunter.OutOfArrows());
    }

    TEST(HunterSuite, Hunter_OnCaveEntrance_EatenByWumpus)
    {
        TestEnvironment env;

        HuntTheWumpus::Hunter hunter(env.m_context);

        const auto wumpus = std::make_shared<HuntTheWumpus::Wumpus>(0, env.m_context);

        // This should return true that there was an action taken.
        CHECK(hunter.ObserveCaveEntrance(wumpus));

        // Show that a state-change happened to a "lost" result.
        CHECK(env.m_state.m_gameOverCalled);
        CHECK(!env.m_state.m_gameOverResult);
    }

    TEST(HunterSuite, Hunter_OnCaveEntrance_ShotByArrow)
    {
        TestEnvironment env;

        HuntTheWumpus::Hunter hunter(env.m_context);

        const auto arrow = std::make_shared<HuntTheWumpus::Arrow>(0, env.m_context);

        // This should return true that there was an action taken.
        CHECK(hunter.ObserveCaveEntrance(arrow));

        // Show that a state-change happened to a "lost" result.
        CHECK(env.m_state.m_gameOverCalled);
        CHECK(!env.m_state.m_gameOverResult);
    }
}
