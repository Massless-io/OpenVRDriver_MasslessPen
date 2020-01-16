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

TEST(TrackingSystemTypeTest, AllTrackingSystemTypesMapToValidInt) {
    using MasslessInterface::TrackingSystemType;

    auto invalid_int = TrackingSystemType::getIntValue(TrackingSystemType::SystemType::INVALID_SYSTEM);
    auto system_types = TrackingSystemType::getAllSystemTypes();
    for(const auto& type : system_types)
        EXPECT_THAT(TrackingSystemType::getIntValue(type), Not(Eq(invalid_int)));

}

TEST(TrackingSystemTypeTest, AllTrackingSystemTypesMapToValidString) {
    using MasslessInterface::TrackingSystemType;

    auto invalid_string = TrackingSystemType::getStringValue(TrackingSystemType::SystemType::INVALID_SYSTEM);
    auto system_types = TrackingSystemType::getAllSystemTypes();
    for (const auto& type : system_types)
        EXPECT_THAT(TrackingSystemType::getStringValue(type), Not(Eq(invalid_string)));
}

TEST(TrackingSystemTypeTest, InvalidIntMapsToInvalidSystem) {
    using MasslessInterface::TrackingSystemType;

    int obvious_invalid_int = INT_MIN;
    EXPECT_THAT(TrackingSystemType::getSystemType(obvious_invalid_int), Eq(TrackingSystemType::SystemType::INVALID_SYSTEM));
}