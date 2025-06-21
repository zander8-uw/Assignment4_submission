#include <TestHarness.h>

#include "Dungeon.h"

#include "Cave.h"

#include "Bat.h"
#include "Pit.h"
#include "Wumpus.h"

#include "TestHelperTestEnvironment.h"

namespace TestHuntTheWumpus
{
    TEST(DungeonSuite, Dungeon_Initialization_DoubleRequest_Ignored)
    {
        TestEnvironment env;

        // This will ask for 8 random cave ids because we're
        // going to try to set the 2nd bat into cave #1 again,
        // and try to put the Hunter into an occupied cave.
        env.m_provider.SetCaveSequence({ 1, 1, 2, 3, 4, 5, 1, 6 });

        HuntTheWumpus::Dungeon dungeon(env.m_context);

        // Verify Bats uniquely distributed
        const auto cave1 = dungeon.FindCave(1);
        const auto cave2 = dungeon.FindCave(2);
        CHECK(cave1->HasDenizen({ HuntTheWumpus::Category::Bat, 0 }));
        CHECK(cave2->HasDenizen({ HuntTheWumpus::Category::Bat, 1 }));

        // Verify Hunter in separate cave.
        const auto cave6 = dungeon.FindCave(6);
        CHECK(cave6->HasDenizen({ HuntTheWumpus::Category::Hunter, 0 }));
    }

    TEST(DungeonSuite, TestDenizenPriorities)
    {
        TestEnvironment env;

        const HuntTheWumpus::Bat  bat(0, env.m_context);
        const HuntTheWumpus::Pit  pit(0, env.m_context);
        const HuntTheWumpus::Wumpus wumpus(0, env.m_context);

        CHECK(bat.GetPriority() > pit.GetPriority());
        CHECK(pit.GetPriority() > wumpus.GetPriority());
    }

    TEST(DungeonSuite, Dungeon_MoveRequest_LegalMoveIsProper)
    {
        TestEnvironment env;

        // This will ask for 6 random cave ids.
        env.m_provider.SetCaveSequence({ 1, 2, 3, 4, 5, 6 });

        HuntTheWumpus::Dungeon dungeon(env.m_context);

        // We know the Hunter is in cave 6, which connects to 15.
        dungeon.MakeMove(HuntTheWumpus::DungeonMove::Move, { 15 });

        // Find the hunter, verify it's in cave 15.
        const auto& cave = dungeon.FindCave(15);

        CHECK(cave->HasDenizens());
        CHECK(cave->HasDenizen({ HuntTheWumpus::Category::Hunter, 0 }));
    }

    TEST(DungeonSuite, Dungeon_ShootRequest_MissingTheWumpusReportsMiss)
    {
        TestEnvironment env;

        // This will ask for 6 random cave ids.
        env.m_provider.SetCaveSequence({ 1, 2, 3, 4, 5, 6 });

        HuntTheWumpus::Dungeon dungeon(env.m_context);

        // Make sure the game is playing.
        env.m_state.m_isPlayingResult = true;

        // We know the Hunter is in cave 6, which connects to 5, 7, and 15.
        // Request none of those, so that we a) randomly move, and b) miss.
        dungeon.MakeMove(HuntTheWumpus::DungeonMove::Shoot, { 16 });

        // Show that missing the Wumpus keeps the game going.
        CHECK(env.m_state.m_isPlayingResult);

        // And that the Wumpus moved to a new cave. We know that the
        // Wumpus startred in cave 3, and the random move cave was tunnel #0.
        // However, we don't know the order of the tunnel identifiers,
        // so we have to look it up be cause it's stored in an unordered map,
        // and the ordering out of GetConnectedIds() is undefined.
        const auto cave3 = dungeon.FindCave(3);
        const auto connectedCaves = cave3->GetConnectedIds();
        const auto newCaveId = connectedCaves.front();

        const auto cave = dungeon.FindCave(newCaveId);

        CHECK(cave->HasDenizen({ HuntTheWumpus::Category::Wumpus , 0 }));
    }

    TEST(DungeonSuite, Dungeon_ShootingAll_GameOver)
    {
        TestEnvironment env;

        // This will ask for 6 random cave ids.
        env.m_provider.SetCaveSequence({ 1, 2, 3, 4, 5, 6 });

        HuntTheWumpus::Dungeon dungeon(env.m_context);

        // Make sure the game is playing.
        env.m_state.m_isPlayingResult = true;

        for (auto idx = 0; idx < 4; ++idx)
        {
            // We know the Hunter is in cave 6, which connects to 5, 7, and 15.
            // Request none of those, so that we a) randomly move, and b) miss.
            dungeon.MakeMove(HuntTheWumpus::DungeonMove::Shoot, { 16 });
            CHECK(env.m_state.m_isPlayingResult);
        }

        // Show that shooting one more makes the game over.
        dungeon.MakeMove(HuntTheWumpus::DungeonMove::Shoot, { 16 });
        CHECK(env.m_state.m_gameOverCalled);
        CHECK(!env.m_state.m_gameOverResult);
    }

    TEST(DungeonSuite, Dungeon_ShootRequest_HittingTheWumpusReportsGameOver)
    {
        TestEnvironment env;

        // This will ask for 6 random cave ids. Put the Wumpus
        // in Cave #15
        env.m_provider.SetCaveSequence({ 1, 2, 15, 4, 5, 6 });

        HuntTheWumpus::Dungeon dungeon(env.m_context);

        // Make sure the game is playing.
        env.m_state.m_isPlayingResult = true;

        // We know the Hunter is in cave 6, which connects to 15.
        // We've put the Wumpus in 15.
        dungeon.MakeMove(HuntTheWumpus::DungeonMove::Shoot, { 15 });

        // Show that we hit the Wumpus!
        CHECK(!env.m_state.m_isPlayingResult);
        CHECK(env.m_state.m_gameOverCalled);
        CHECK(env.m_state.m_gameOverResult);
    }
}
