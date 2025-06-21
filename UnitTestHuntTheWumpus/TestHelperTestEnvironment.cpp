#include "TestHelperTestEnvironment.h"

namespace TestHuntTheWumpus
{
    TestEnvironment::TestEnvironment()
    {
        MakeTestNotifications();
    }

     void TestEnvironment::MakeTestNotifications()
     {
         m_userNotifier.AddCallback(HuntTheWumpus::UserNotification::Notification::ObserveWumpus, [this]() {
             m_testNotifications.observeWumpusCheck = true;
             });

         m_userNotifier.AddCallback(HuntTheWumpus::UserNotification::Notification::ObservePit, [this]() {
             m_testNotifications.observePitCheck = true;
             });

         m_userNotifier.AddCallback(HuntTheWumpus::UserNotification::Notification::ObserveBat, [this]() {
             m_testNotifications.observeBatCheck = true;
             });

         m_userNotifier.AddCallback(HuntTheWumpus::UserNotification::Notification::ObserveMiss, [this]() {
             m_testNotifications.observeMissCheck = true;
             });

         m_userNotifier.AddCallback(HuntTheWumpus::UserNotification::Notification::ObserveOutOfArrows, [this]() {
             m_testNotifications.observeOutOfArrowsCheck = true;
             });

         m_userNotifier.AddCallback(HuntTheWumpus::UserNotification::Notification::BatTriggered, [this]() {
             m_testNotifications.batTriggeredCheck = true;
             });

         m_userNotifier.AddCallback(HuntTheWumpus::UserNotification::Notification::PitTriggered, [this]() {
             m_testNotifications.pitTriggeredCheck = true;             
             });

         m_userNotifier.AddCallback(HuntTheWumpus::UserNotification::Notification::WumpusTriggered, [this]() {
             m_testNotifications.wumpusTriggeredCheck = true;             
             });

         m_userNotifier.AddCallback(HuntTheWumpus::UserNotification::Notification::WumpusAwoken, [this]() {
             m_testNotifications.wumpusAwokenCheck = true;             
             });

         m_userNotifier.AddCallback(HuntTheWumpus::UserNotification::Notification::WumpusShot, [this]() {
             m_testNotifications.wumpusShotCheck = true;             
             });

         m_userNotifier.AddCallback(HuntTheWumpus::UserNotification::Notification::HunterEaten, [this]() {
             m_testNotifications.hunterEatenCheck = true;             
             });

         m_userNotifier.AddCallback(HuntTheWumpus::UserNotification::Notification::HunterShot, [this]() {
             m_testNotifications.hunterShotCheck = true;             
             });

         m_userNotifier.AddCallback(HuntTheWumpus::UserNotification::Notification::CaveEntered, [this]() {
             m_testNotifications.caveEnteredCheck = true;             
             });

         m_userNotifier.AddCallback(HuntTheWumpus::UserNotification::Notification::ReportIllegalMove, [this]() {
             m_testNotifications.reportIllegalMoveCheck = true;
             });

         m_userNotifier.AddCallback(HuntTheWumpus::UserNotification::Notification::ReportNeighboringCaves, [this]() {
             m_testNotifications.reportNeighboringCavesCheck = true;
             });
     }
}
