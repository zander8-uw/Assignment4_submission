
#include "Context.h"
#include "Dungeon.h"
#include "Denizen.h"
#include "Cave.h"
#include "RandomProvider.h"
#include "GameStateObservation.h"
#include "UserNotification.h"
#include <Hunter.h>

#include <iostream>
#include <random>
#include <string>
using namespace HuntTheWumpus;

namespace
{
    class RandomCave final : public HuntTheWumpus::IRandomProvider
    {
    public:

        RandomCave() = default;
        ~RandomCave() override = default;

        int MakeRandomCave() override
        {
            return m_distributionCave(m_generator);
        }

        int MakeRandomTunnel() override
        {
            return m_distributionTunnel(m_generator);
        }

        float MakeRandomNumber() override
        {
            return m_distributionRandom(m_generator);
        }

        RandomCave(const RandomCave&) = delete;
        RandomCave(RandomCave&&) = delete;
        RandomCave& operator=(const RandomCave&) = delete;
        RandomCave& operator=(RandomCave&&) = delete;

    private:

        std::mt19937 m_generator = std::mt19937(static_cast<unsigned int>(time(nullptr)));  // NOLINT(cert-msc51-cpp)
        std::uniform_int_distribution<int> m_distributionCave = std::uniform_int_distribution<int>(1, 20);
        std::uniform_int_distribution<int> m_distributionTunnel = std::uniform_int_distribution<int>(0, 2);
        std::uniform_real_distribution<float> m_distributionRandom = std::uniform_real_distribution<float>(0.0f, 1.0f);
    };

    void MakeUserNotifications(UserNotification& userNotification, Dungeon& dungeon)
    {
        userNotification.AddCallback(UserNotification::Notification::ObserveWumpus, []() {
            std::cout << "I smell a wumpus!\n";
            });

        userNotification.AddCallback(UserNotification::Notification::ObservePit, []() {
            std::cout << "I feel a draft!\n";
            });

        userNotification.AddCallback(UserNotification::Notification::ObserveBat, []() {
            std::cout << "Bats nearby!\n";
            });

        userNotification.AddCallback(UserNotification::Notification::ObserveMiss, [&]() {
            auto hunter = std::dynamic_pointer_cast<Hunter>(dungeon.GetCaveDenizens().at({ Category::Hunter, 0 }));
            std::cout << "The Arrow missed. You have " << hunter->ArrowCount() << " Arrows left.\n";
            });

        userNotification.AddCallback(UserNotification::Notification::ObserveOutOfArrows, []() {
            std::cout << "You are out of arrows...\n";
            });

        userNotification.AddCallback(UserNotification::Notification::BatTriggered, []() {
            std::cout << "Oh no! The Bat has picked you up and carried you to another cave!\n";
            });

        userNotification.AddCallback(UserNotification::Notification::PitTriggered, []() {
            std::cout << "You feel your feet slip beneath you... you have fallen into a pit! You lose.\n";
            });

        userNotification.AddCallback(UserNotification::Notification::WumpusTriggered, []() {
            std::cout << "You have accidently stumbled across the Wumpus!\n";
            });

        userNotification.AddCallback(UserNotification::Notification::WumpusAwoken, []() {
            std::cout << "The Wumpus has been alerted, and it moves to a neighboring cave.\n";
            });

        userNotification.AddCallback(UserNotification::Notification::WumpusShot, []() {
            std::cout << "Your Arrow hit the mark, the Wumpus has been shot!\n";
            });

        userNotification.AddCallback(UserNotification::Notification::HunterEaten, []() {
            std::cout << "The mighty Wumpus has caught and consumed you!\n";
            });

        userNotification.AddCallback(UserNotification::Notification::HunterShot, []() {
            std::cout << "Somehow you shot yourself with your own arrow...\n";
            });

        userNotification.AddCallback(UserNotification::Notification::CaveEntered, [&]() {
            auto hunterCurrentCave = dungeon.GetCaveDenizens().at({ Category::Hunter, 0 })->GetCurrentCave().lock(); // find the Hunter's cave
            std::cout << "You are in cave: ";
            std::cout << hunterCurrentCave->GetCaveId() << "\n";    // print cave number of the current location of the Hunter
            });

        userNotification.AddCallback(UserNotification::Notification::ReportIllegalMove, []() {
            std::cout << "That move is not possible, please try again.\n";
            });

        userNotification.AddCallback(UserNotification::Notification::ReportNeighboringCaves, [&]() {
            auto hunterCurrentCave = dungeon.GetCaveDenizens().at({ Category::Hunter, 0 })->GetCurrentCave().lock();  // find the Hunter's cave
            auto connectedLocations = hunterCurrentCave->GetConnectedIds();                                           // get connected Ids

            std::cout << "The caves connected to yours are: ";
            for (auto cave : connectedLocations)    // print number of each connected cave
            {
                std::cout << cave << " ";
            }
            std::cout << "\n";
            });
    }

    class GameChange final : public IGameStateChange
    {
    public:
        GameChange() = default;
        ~GameChange() override = default;

        void GameOver(const bool won) override
        {
            std::cout << "Game over: you " << (won ? "won" : "lost") << ".\n";
            m_playing = false;
        }

        [[nodiscard]] bool IsPlaying() const override { return m_playing; }

        GameChange(const GameChange&) = delete;
        GameChange(GameChange&&) = delete;
        GameChange& operator=(const GameChange&) = delete;
        GameChange& operator=(GameChange&&) = delete;

    private:
        bool m_playing = true;
    };

    std::vector<std::string> SplitString(const std::string& text, const std::string& delims)
    {
        std::vector<std::string> tokens;
        std::size_t start = text.find_first_not_of(delims), end;

        while ((end = text.find_first_of(delims, start)) != std::string::npos)
        {
            tokens.push_back(text.substr(start, end - start));
            start = text.find_first_not_of(delims, end);
        }

        if (start != std::string::npos)
        {
            tokens.push_back(text.substr(start));
        }

        return tokens;
    }

    std::string ToLower(const std::string& input)
    {
        std::string result;
        result.resize(input.size());

        std::ranges::transform(input.begin(), input.end(), result.begin(), [](const auto val) { return static_cast<char>(tolower(val)); });

        return result;
    }
}

bool CheckForNearbyHazard(HuntTheWumpus::Category category, HuntTheWumpus::Dungeon& dungeon)
{
    auto hunterCave = dungeon.GetCaveDenizens().at({ HuntTheWumpus::Category::Hunter, 0 })->GetCurrentCave().lock();
    auto connectedCaves = hunterCave->GetConnectedIds();

    for (auto caveId : connectedCaves)
    {
        auto& cave = dungeon.GetCaves().at(caveId);

        if (cave->HasDenizen({ category, 0 }) || cave->HasDenizen({ category, 1 }))
        {
            return true;
        }
    }

    return false;
}

void NotifyOfNearbyHazards(HuntTheWumpus::UserNotification& userNotification, HuntTheWumpus::Dungeon& dungeon)
{

    if (CheckForNearbyHazard(HuntTheWumpus::Category::Wumpus, dungeon))
    {
        userNotification.Notify(HuntTheWumpus::UserNotification::Notification::ObserveWumpus);
    }

    if (CheckForNearbyHazard(HuntTheWumpus::Category::Bat, dungeon))
    {
        userNotification.Notify(HuntTheWumpus::UserNotification::Notification::ObserveBat);
    }

    if (CheckForNearbyHazard(HuntTheWumpus::Category::Pit, dungeon))
    {
        userNotification.Notify(HuntTheWumpus::UserNotification::Notification::ObservePit);
    }   
}

int main()
{
    RandomCave sourceOfRandom;
    GameChange change;

    auto observe = HuntTheWumpus::UserNotification();

    HuntTheWumpus::Context gameContext{ .m_notification = observe, .m_random = sourceOfRandom, .m_change = change };

    HuntTheWumpus::Dungeon dungeon(gameContext);

    MakeUserNotifications(observe, dungeon);

    // Note: In order to report the connected caves and the hunter's cave, I made MakeUserNotifications() take in a Dungeon arguement.
    // The issue is this creates a chicken and egg problem... 
    // I need Dungeon to exist to register the callbacks properly, but I need the UserNotifications to exist in order to create the Context for Dungeon.
    // My solution is further change MakeUserNotifications() to take in the 'observe' object as a reference.
    // I couldn't think of a better way to get around this, hopefully this is acceptable. 
    
    
    while (change.IsPlaying())
    {
        observe.Notify(HuntTheWumpus::UserNotification::Notification::CaveEntered);
        observe.Notify(HuntTheWumpus::UserNotification::Notification::ReportNeighboringCaves);

        NotifyOfNearbyHazards(observe, dungeon);

        // Parse input.
        std::string input;

        std::cout << "\nCommand? \n";   // the \n was unneeded, but it makes it a little easier to read.
        std::cout.flush();

        std::getline(std::cin, input);

        // Split into strings.
        const auto stringTokens = SplitString(input, " \t\n");

        const auto& command = stringTokens[0];

        if (command[0] == 'm')
        {
            std::cout << "\n";
            if (stringTokens.size() < 2)
            {
                std::cout << "A Move command must be followed by the destination cave id.\n";
                continue;
            }

            // Second token is a destination.
            const auto destCave = std::stoi(stringTokens[1]);

            dungeon.MakeMove(HuntTheWumpus::DungeonMove::Move, { destCave });
        }

        if (command[0] == 's')
        {
            std::cout << "\n";
            if (stringTokens.size() < 2)
            {
                std::cout << "A Shoot command must be followed by a list of caves for the arrow to go through.\n";
                continue;
            }

            // Remaining tokens is the desired arrow path.
            std::vector<int> path;
            auto firstToken = false;

            for (auto&& token : stringTokens)
            {
                if (!firstToken)
                {
                    firstToken = true;
                    continue;
                }

                path.push_back(std::stoi(token));
            }

            path.resize(std::min(path.size(), static_cast<size_t>(5)));

            dungeon.MakeMove(HuntTheWumpus::DungeonMove::Shoot, path);
        }

        if (command[0] == 'q' || command[0] == 'e' || command[0] == 'x')
        {
            std::cout << "Exiting.\n";
            change.GameOver(false);
        }
    }

    return 0;
}
