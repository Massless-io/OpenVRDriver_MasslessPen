/* Copyright (C) 2020 Massless Corp. - All Rights Reserved
 * Author: Jacob Hilton.
 * You may use, distribute and modify this code under the
 * terms of the BSD 3-Clause "New" or "Revised" License.
 *
 * You should have received a copy of this license with
 * this file. If not, please email support@massless.io
 */

#include "Testing.hpp"
#include <MasslessOpenVRIntegrationUUIDs.h>

using namespace testing;

#define asIK(x) std::array<uint8_t,SettingsUtilities::m_integrationKeyLen> x // Ignore the odd syntax

TEST(SettingsUtilitiesTest, DefaultIntegrationKeyIsOpenVRDevIntegrationKey) {
    EXPECT_THAT(SettingsUtilities::getDefaultIntegrationKey(), ElementsAreArray(asIK(M_INTEGRATION_OPENVRDEV)));
}

TEST(SettingsUtilitiesTest, IntegrationKeysMatchToCorrectExeName) {

    auto sculptvr_key = SettingsUtilities::getIntegrationKeyForApp("SculptrVR-Win64-Shipping.exe");
    EXPECT_TRUE(sculptvr_key.has_value());
    EXPECT_THAT(*sculptvr_key, ElementsAreArray(asIK(M_INTEGRATION_OPENVR_SCULPTVR)));

    auto masterpiece_key = SettingsUtilities::getIntegrationKeyForApp("Masterpiece_VR.exe");
    EXPECT_TRUE(masterpiece_key.has_value());
    EXPECT_THAT(*masterpiece_key, ElementsAreArray(asIK(M_INTEGRATION_OPENVR_MASTERPIECEVR)));

    auto gravity_key = SettingsUtilities::getIntegrationKeyForApp("GravitySketchVR.exe");
    EXPECT_TRUE(gravity_key.has_value());
    EXPECT_THAT(*gravity_key, ElementsAreArray(asIK(M_INTEGRATION_OPENVR_GRAVITYSKETCH)));

    auto sketchbox_key = SettingsUtilities::getIntegrationKeyForApp("Sketch-Win64-Shipping.exe");
    EXPECT_TRUE(sketchbox_key.has_value());
    EXPECT_THAT(*sketchbox_key, ElementsAreArray(asIK(M_INTEGRATION_OPENVR_SKETCHBOX)));

    auto tiltbrush_key = SettingsUtilities::getIntegrationKeyForApp("TiltBrush.exe");
    EXPECT_TRUE(tiltbrush_key.has_value());
    EXPECT_THAT(*tiltbrush_key, ElementsAreArray(asIK(M_INTEGRATION_OPENVR_TILTBRUSH)));

    auto home_key = SettingsUtilities::getIntegrationKeyForApp("steamtours.exe");
    EXPECT_TRUE(home_key.has_value());
    EXPECT_THAT(*home_key, ElementsAreArray(asIK(M_INTEGRATION_OPENVR_STEAMVRHOME)));

}