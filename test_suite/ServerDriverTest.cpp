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
TEST(ServerDriverTest, ConstructorWorks) {
    std::shared_ptr<NiceMock<MockPenSystem>> pen_system = std::make_shared<NiceMock<MockPenSystem>>();
    ServerDriver server_driver(pen_system);
}

TEST(ServerDriverTest, TryAddDevice) {
    testing::NiceMock<MockVRServerDriverHost> host;
    // Setup return value for TrackedDeviceAdded

    ON_CALL(host, TrackedDeviceAdded(_, _, _))
        .WillByDefault(Return(true));
    ServerDriver server_driver(std::make_shared<NiceMock<MockPenSystem>>());
    std::unique_ptr<IDriverDevice> controller = std::make_unique<MockController>();

    std::string controllername = "mock_controller";

    // Expect driver will try to add device
    EXPECT_CALL(host, TrackedDeviceAdded(StrEq(controllername), vr::ETrackedDeviceClass::TrackedDeviceClass_Controller, _))
        .Times(1)
        .WillOnce(Return(true));

    // Expect to have succeeded
    EXPECT_EQ(server_driver.tryAddDevice(std::move(controller), controllername, vr::ETrackedDeviceClass::TrackedDeviceClass_Controller, &host), true);

    // Expect driver will try to add device
    EXPECT_CALL(host, TrackedDeviceAdded(StrEq(controllername), vr::ETrackedDeviceClass::TrackedDeviceClass_Controller, _))
        .Times(1)
        .WillOnce(Return(false));

    // remake controller
    controller = std::make_unique<MockController>();

    // Expect to have failed
    EXPECT_EQ(server_driver.tryAddDevice(std::move(controller), controllername, vr::ETrackedDeviceClass::TrackedDeviceClass_Controller, &host), false);
}
