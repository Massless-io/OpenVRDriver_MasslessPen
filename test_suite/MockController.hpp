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
#include <IDriverDevice.hpp>

class MockController : public IDriverDevice
{
public:
    MOCK_METHOD1(Activate, vr::EVRInitError(uint32_t unObjectId));
    MOCK_METHOD0(Deactivate, void());
    MOCK_METHOD0(EnterStandby, void());
    MOCK_METHOD1(GetComponent, void* (const char* pchComponentNameAndVersion));
    MOCK_METHOD3(DebugRequest, void(const char* pchRequest, char* pchResponseBuffer, uint32_t unResponseBufferSize));
    MOCK_METHOD0(GetPose, vr::DriverPose_t());
    MOCK_METHOD0(getIndex, vr::TrackedDeviceIndex_t(void));
    MOCK_METHOD1(update, void(std::vector<vr::VREvent_t> events));
};

