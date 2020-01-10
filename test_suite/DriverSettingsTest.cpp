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

TEST(DriverSettingsTest, DefaultSetsValues) {
    DriverSettings settings = DriverSettings::make_default();

    EXPECT_EQ(settings.getValue<std::string>(DriverSettings::Handedness).value(), "right");
    EXPECT_EQ(settings.getValue<bool>(DriverSettings::AttachGizmo).value(), false);
    EXPECT_EQ(settings.getValue<bool>(DriverSettings::EnableDetailedLogging).value(), false);
    EXPECT_EQ(settings.getValue<std::string>(DriverSettings::AutoTrackingRefSerial), std::nullopt);
    EXPECT_EQ(settings.getValue<std::string>(DriverSettings::ForcedTrackingRefSerial), std::nullopt);
}

TEST(DriverSettingsTest, BadValueTypeReturnsNullopt) {
    DriverSettings settings = DriverSettings::make_default();
    EXPECT_FALSE(settings.getValue<bool>(DriverSettings::Handedness).has_value());
}

TEST(DriverSettingsTest, GoodValueTypeReturnsValue) {
    DriverSettings settings = DriverSettings::make_default();
    EXPECT_TRUE(settings.getValue<std::string>(DriverSettings::Handedness).has_value());
}

TEST(DriverSettingsTest, InvalidTypeReturnsInvalid) {
    DriverSettings settings = DriverSettings::make_default();
    settings.setValue<DriverSettings::invalid_setting_t>(DriverSettings::AttachGizmo, DriverSettings::invalid_setting_t{});
    EXPECT_FALSE(settings.isValid(DriverSettings::AttachGizmo));
}

TEST(DriverSettingsTest, EqualityOperatorWorks) {
    DriverSettings settings1 = DriverSettings::make_default();
    DriverSettings settings2 = DriverSettings::make_default();

    EXPECT_EQ(settings1, settings2);
}

TEST(DriverSettingsTest, AreAllValidReturnWorks) {

    DriverSettings settings = DriverSettings::make_default();
    settings.clearValue(DriverSettings::AttachGizmo); // Ensure one value is nullopt
    EXPECT_TRUE(settings.areAllValid());

    settings.setValue<DriverSettings::invalid_setting_t>(DriverSettings::AttachGizmo, DriverSettings::invalid_setting_t{});
    EXPECT_FALSE(settings.areAllValid());
}

TEST(DriverSettingsTest, ClearValueWorks) {
    DriverSettings settings = DriverSettings::make_default();
    EXPECT_TRUE(settings.getValue<DriverSettings::SettingValue>(DriverSettings::AttachGizmo).has_value());
    settings.clearValue(DriverSettings::AttachGizmo);
    EXPECT_FALSE(settings.getValue<DriverSettings::SettingValue>(DriverSettings::AttachGizmo).has_value());
}