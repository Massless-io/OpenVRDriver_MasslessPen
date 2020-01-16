/* Copyright (C) 2020 Massless Corp. - All Rights Reserved
 * Author: Jacob Hilton.
 * You may use, distribute and modify this code under the
 * terms of the BSD 3-Clause "New" or "Revised" License.
 *
 * You should have received a copy of this license with
 * this file. If not, please email support@massless.io
 */

#include "Testing.hpp"

TEST(FileSettingsLoaderTest, LoadingWorks) {
    std::filesystem::path working_path = std::filesystem::current_path().string() + "//" + testing::UnitTest::GetInstance()->current_test_info()->name() + ".json";
    if(std::filesystem::exists(working_path)) // Remove old file in case it already exists
        std::filesystem::remove(working_path);
    
    ASSERT_NO_THROW({ 
        FileSettingsLoader settingsLoader(working_path); 
        settingsLoader.readSettings();
    });
    ASSERT_NO_THROW({ 
        FileSettingsLoader settingsLoader(std::make_unique<std::istringstream>("{}"), std::make_unique<std::ostringstream>()); 
        settingsLoader.readSettings();
    });

    ASSERT_THROW({ 
        FileSettingsLoader settingsLoader(std::make_unique<std::istringstream>("invalidjson"), std::make_unique<std::ostringstream>()); 
        settingsLoader.readSettings();
    }, DriverSettingsException);
    
    ASSERT_TRUE(std::filesystem::exists(working_path)) << "File should have been created at path";

    // Cleanup file
    std::filesystem::remove(working_path);
}

TEST(FileSettingsLoaderTest, ReadingSettingsFilePreservesSetSettings) {

    std::filesystem::path working_path = std::filesystem::current_path().string() + "//TEST_" + testing::UnitTest::GetInstance()->current_test_info()->name() + ".json";
    if (std::filesystem::exists(working_path)) // Remove old file in case it already exists
        std::filesystem::remove(working_path);

    DriverSettings expected_settings;
    expected_settings.setValue<bool>(DriverSettings::AttachGizmo, true);
    {
        FileSettingsLoader settingsLoader(working_path);

        // Write settings to file
        settingsLoader.writeSettings(expected_settings);

        // File opened but should not be empty as it has values in it already
        std::ifstream config_file(working_path);
        EXPECT_NE(config_file.peek(), std::ifstream::traits_type::eof());

        float returnValue = settingsLoader.readSettings().getValue<bool>(DriverSettings::AttachGizmo).value_or(false);
        EXPECT_EQ(returnValue, expected_settings.getValue<bool>(DriverSettings::AttachGizmo).value());
    }
    std::filesystem::remove(working_path);
}

TEST(FileSettingsLoaderTest, ReadSettingsReadsAllSettings) {
    std::filesystem::path working_path = std::filesystem::current_path().string() + "//TEST_" + testing::UnitTest::GetInstance()->current_test_info()->name() + ".json";
    if (std::filesystem::exists(working_path)) // Remove old file in case it already exists
        std::filesystem::remove(working_path);

    // Setup expected
    DriverSettings expected_settings;
    expected_settings.setValue<bool>(DriverSettings::AttachGizmo, true);
    expected_settings.setValue<bool>(DriverSettings::EnableDetailedLogging, true);
    expected_settings.setValue<std::string>(DriverSettings::Handedness, "right");
    expected_settings.setValue<std::string>(DriverSettings::AutoTrackingRefSerial, "example_trackingref_serial");
    expected_settings.setValue<std::string>(DriverSettings::ForcedTrackingRefSerial, "forced_trackingref_serial");

    // Write and read
    FileSettingsLoader settingsLoader(working_path);
    settingsLoader.writeSettings(expected_settings);
    DriverSettings loaded_settings = settingsLoader.readSettings();

    // Check expected
    EXPECT_EQ(loaded_settings.getValue<bool>(DriverSettings::AttachGizmo).value(), expected_settings.getValue<bool>(DriverSettings::AttachGizmo).value());
    EXPECT_EQ(loaded_settings.getValue<bool>(DriverSettings::EnableDetailedLogging).value(), expected_settings.getValue<bool>(DriverSettings::EnableDetailedLogging).value());
    EXPECT_EQ(loaded_settings.getValue<std::string>(DriverSettings::Handedness).value(), expected_settings.getValue<std::string>(DriverSettings::Handedness).value());
    EXPECT_EQ(loaded_settings.getValue<std::string>(DriverSettings::AutoTrackingRefSerial).value(), expected_settings.getValue<std::string>(DriverSettings::AutoTrackingRefSerial).value());
    EXPECT_EQ(loaded_settings.getValue<std::string>(DriverSettings::ForcedTrackingRefSerial).value(), expected_settings.getValue<std::string>(DriverSettings::ForcedTrackingRefSerial).value());
    
    std::filesystem::remove(working_path);
}
TEST(FileSettingsLoaderTest, ReadingBlankSettingsUsesDefaults) {
    
    FileSettingsLoader settingsLoader(std::make_unique<std::istringstream>("{}"), std::make_unique<std::ostringstream>());

    DriverSettings loaded_settings = settingsLoader.readSettings();
    DriverSettings default_settings = DriverSettings::make_default();
    EXPECT_EQ(loaded_settings, default_settings);


}
TEST(FileSettingsLoaderTest, WriteAndReadBool) {
    
    // Just use this key for testing
    DriverSettings::Setting key1 = DriverSettings::AttachGizmo;

    for (std::size_t i = 0; i < 10; i++) {
        bool k1_value = rand() % 2 == 0;
        nlohmann::json obj;
        obj[DriverSettings::getKeyString(key1)] = k1_value;

        FileSettingsLoader settingsLoader(std::make_unique<std::istringstream>(obj.dump()), std::make_unique<std::ostringstream>());
        DriverSettings settings = settingsLoader.readSettings();

        ASSERT_EQ(settings.getValue<bool>(key1).value(), k1_value);
    }
}
/* Dont have any int settings keys so we cannot test this
TEST(FileSettingsLoaderTest, WriteAndReadInt) {

    // Just use this key for testing
    DriverSettings::Setting key1 = DriverSettings::AttachGizmo;

    for (std::size_t i = 0; i < 10; i++) {
        int32_t k1_value = rand() % 200 - 100;
        nlohmann::json obj;
        obj[DriverSettings::getKeyString(key1)] = k1_value;

        FileSettingsLoader settingsLoader(std::make_unique<std::istringstream>(obj.dump()), std::make_unique<std::ostringstream>());

        DriverSettings settings = settingsLoader.readSettings();

        ASSERT_EQ(settings.getValue<int32_t>(key1).value(), k1_value);
    }
}
*/
TEST(FileSettingsLoaderTest, WriteAndReadString) {

    // Just use this key for testing
    DriverSettings::Setting key1 = DriverSettings::AutoTrackingRefSerial;

    for (std::size_t i = 0; i < 10; i++) {
        std::string k1_value = "AAAABBBBCCCCDDDD";
        nlohmann::json obj;
        obj[DriverSettings::getKeyString(key1)] = k1_value;

        FileSettingsLoader settingsLoader(std::make_unique<std::istringstream>(obj.dump()), std::make_unique<std::ostringstream>());

        DriverSettings settings = settingsLoader.readSettings();

        ASSERT_EQ(settings.getValue<std::string>(key1).value(), k1_value);
    }
}
/* Dont have any float settings keys so we cannot test this 
TEST(FileSettingsLoaderTest, WriteAndReadFloat) {

    // Just use this key for testing
    DriverSettings::Setting key1 = DriverSettings::AttachGizmo;

    for (std::size_t i = 0; i < 10; i++) {
        float k1_value = (rand() % 200 - 100) / 100.f;
        nlohmann::json obj;
        obj[DriverSettings::getKeyString(key1)] = k1_value;

        FileSettingsLoader settingsLoader(std::make_unique<std::istringstream>(obj.dump()), std::make_unique<std::ostringstream>());

        DriverSettings settings = settingsLoader.readSettings();

        ASSERT_EQ(settings.getValue<float>(key1).value(), k1_value);
    }
}
*/