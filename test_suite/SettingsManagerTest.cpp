/* Copyright (C) 2020 Massless Corp. - All Rights Reserved
 * Author: Jacob Hilton.
 * You may use, distribute and modify this code under the
 * terms of the BSD 3-Clause "New" or "Revised" License.
 *
 * You should have received a copy of this license with
 * this file. If not, please email support@massless.io
 */

#include "Testing.hpp"

using namespace testing;

TEST(SettingsManager, ConstructorLoadsSettings) {
    SettingsManager settingsManager(std::make_unique<FileSettingsLoader>(std::make_unique<std::istringstream>("{}"), std::make_unique<std::ostringstream>()));
    EXPECT_EQ(settingsManager.getSettings(), DriverSettings::make_default());
}

TEST(SettingsManager, DestructorSavesSettings) {
    std::filesystem::path working_path = std::filesystem::current_path().string() + "//TEST_" + testing::UnitTest::GetInstance()->current_test_info()->name() + ".json";
    if (std::filesystem::exists(working_path)) // Remove old file in case it already exists
        std::filesystem::remove(working_path);

    {
        SettingsManager settingsManager(std::make_unique<FileSettingsLoader>(working_path));
        settingsManager.getSettings().setValue<std::string>(DriverSettings::AutoTrackingRefSerial, "example_trackingref");
    }

    // File should exist now
    EXPECT_TRUE(std::filesystem::exists(working_path));
    {
        SettingsManager settingsManager(std::make_unique<FileSettingsLoader>(working_path));
        // Value should be set to our custom value
        EXPECT_EQ(settingsManager.getSettings().getValue<std::string>(DriverSettings::AutoTrackingRefSerial).value(), "example_trackingref");
    }
    // Remove working file
    std::filesystem::remove(working_path);
}