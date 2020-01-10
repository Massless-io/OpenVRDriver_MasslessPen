/* Copyright (C) 2020 Massless Corp. - All Rights Reserved
 * Author: Jacob Hilton.
 * You may use, distribute and modify this code under the
 * terms of the BSD 3-Clause "New" or "Revised" License.
 *
 * You should have received a copy of this license with
 * this file. If not, please email support@massless.io
 */

#pragma once

#include "gmock/gmock.h"
#include <IPenSystem.hpp>

class MockPenSystem : public MasslessInterface::IPenSystem {
public:
    MOCK_METHOD0(getIntegrationKey, IntegrationKey(void));
    MOCK_METHOD1(setIntegrationKey, std::optional<ErrorType>(IntegrationKey));

    MOCK_METHOD1(startSystem, std::optional<ErrorType>(bool));
    MOCK_METHOD1(stopSystem, std::optional<ErrorType>(bool));
    MOCK_METHOD0(isSystemRunning, bool(void));

    MOCK_METHOD1(getMasslessTrackerPoseOffset, tl::expected<MasslessInterface::Pose, ErrorType>(MasslessInterface::TrackingSystemType));

    MOCK_METHOD0(getPenSerial, tl::expected<std::string, ErrorType>(void));
    MOCK_METHOD0(isPenConnected, bool(void));
    MOCK_METHOD0(isPenTracking, bool(void));

    MOCK_METHOD0(getPoseCallback, std::optional<std::function<void(MasslessInterface::Pose)>>(void));
    MOCK_METHOD1(setPoseCallback, void(std::function<void(MasslessInterface::Pose)>));
    MOCK_METHOD0(clearPoseCallback, void(void));
    MOCK_METHOD0(getCurrentPose, MasslessInterface::Pose(void));
    MOCK_METHOD1(setCurrentPose, void(MasslessInterface::Pose));
    MOCK_METHOD2(handlePose, ErrorType(uint32_t, uint8_t*));

    MOCK_METHOD0(getStateCallback, std::optional<std::function<void(MasslessInterface::PenState)>>(void));
    MOCK_METHOD1(setStateCallback, void(std::function<void(MasslessInterface::PenState)>));
    MOCK_METHOD0(clearStateCallback, void(void));
    MOCK_METHOD0(getCurrentState, MasslessInterface::PenState(void));
    MOCK_METHOD1(setCurrentState, void(MasslessInterface::PenState));
    MOCK_METHOD2(handleState, ErrorType(uint32_t, uint8_t*));

    MOCK_METHOD0(getNotificationCallback, std::optional<std::function<void(MasslessInterface::PenNotification)>>(void));
    MOCK_METHOD1(setNotificationCallback, void(std::function<void(MasslessInterface::PenNotification)>));
    MOCK_METHOD0(clearNotificationCallback, void(void));
    MOCK_METHOD0(popNotification, std::optional<MasslessInterface::PenNotification>(void));
    MOCK_METHOD1(pushNotification, void(MasslessInterface::PenNotification));
    MOCK_METHOD2(handleNotification, ErrorType(uint32_t, uint8_t*));

    MOCK_METHOD0(getEventCallback, std::optional<std::function<void(MasslessInterface::PenEvent)>>(void));
    MOCK_METHOD1(setEventCallback, void(std::function<void(MasslessInterface::PenEvent)>));
    MOCK_METHOD0(clearEventCallback, void(void));
    MOCK_METHOD0(popEvent, std::optional<MasslessInterface::PenEvent>(void));
    MOCK_METHOD1(pushEvent, void(MasslessInterface::PenEvent));
    MOCK_METHOD2(handleEvent, ErrorType(uint32_t, uint8_t*));

    MOCK_METHOD1(sendVibration, std::optional<ErrorType>(uint16_t));

    MOCK_METHOD1(setUnitScale, void(float));
    MOCK_METHOD0(getUnitScale, float(void));

    MOCK_METHOD0(getDllVersionString, std::string(void));
    MOCK_METHOD0(getDllVersion, std::tuple<uint64_t, uint64_t, uint64_t>(void));


};