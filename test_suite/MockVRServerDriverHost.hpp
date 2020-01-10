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
#include "openvr_driver.h"

class MockVRServerDriverHost : public vr::IVRServerDriverHost
{
public:
    // Inherited via IVRServerDriverHost
    MOCK_METHOD3(TrackedDeviceAdded, bool(const char* pchDeviceSerialNumber, vr::ETrackedDeviceClass eDeviceClass, vr::ITrackedDeviceServerDriver* pDriver));
    MOCK_METHOD3(TrackedDevicePoseUpdated, void(uint32_t unWhichDevice, const vr::DriverPose_t& newPose, uint32_t unPoseStructSize));
    MOCK_METHOD1(VsyncEvent, void(double vsyncTimeOffsetSeconds));
    MOCK_METHOD4(VendorSpecificEvent, void(uint32_t unWhichDevice, vr::EVREventType eventType, const vr::VREvent_Data_t& eventData, double eventTimeOffset));
    MOCK_METHOD0(IsExiting, bool(void));
    MOCK_METHOD2(PollNextEvent, bool(vr::VREvent_t* pEvent, uint32_t uncbVREvent));
    MOCK_METHOD3(GetRawTrackedDevicePoses, void(float fPredictedSecondsFromNow, vr::TrackedDevicePose_t* pTrackedDevicePoseArray, uint32_t unTrackedDevicePoseArrayCount));
    MOCK_METHOD3(TrackedDeviceDisplayTransformUpdated, void(uint32_t unWhichDevice, vr::HmdMatrix34_t eyeToHeadLeft, vr::HmdMatrix34_t eyeToHeadRight));
    MOCK_METHOD4(RequestRestart, void(const char* pchLocalizedReason, const char* pchExecutableToStart, const char* pchArguments, const char* pchWorkingDirectory));
    MOCK_METHOD2(GetFrameTimings, uint32_t(vr::Compositor_FrameTiming*, uint32_t));


};

