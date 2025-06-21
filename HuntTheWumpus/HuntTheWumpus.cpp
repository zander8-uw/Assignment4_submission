
#include "Context.h"
#include "Dungeon.h"
#include "RandomProvider.h"
#include "GameStateObservation.h"
#include "UserNotification.h"

#include <iostream>
#include <random>
#include <string>

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

    HuntTheWumpus::UserNotification MakeUserNotifications()
    {
        return {};
    }

    class GameChange final : public HuntTheWumpus::IGameStateChange
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

int main()
{
    RandomCave sourceOfRandom;
    GameChange change;

    auto observe = MakeUserNotifications();

    HuntTheWumpus::Context gameContext{ .m_notification = observe, .m_random = sourceOfRandom, .m_change = change };

    HuntTheWumpus::Dungeon dungeon(gameContext);

    while (change.IsPlaying())
    {
        // Parse input.
        std::string input;

        std::cout << "Command? ";
        std::cout.flush();

        std::getline(std::cin, input);

        // Split into strings.
        const auto stringTokens = SplitString(input, " \t\n");

        const auto& command = stringTokens[0];

        if (command[0] == 'm')
        {
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
