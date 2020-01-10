/* Copyright (C) 2020 Massless Corp. - All Rights Reserved
 * Author: Jacob Hilton.
 * You may use, distribute and modify this code under the
 * terms of the BSD 3-Clause "New" or "Revised" License.
 *
 * You should have received a copy of this license with
 * this file. If not, please email support@massless.io
 */

#pragma once
#include <functional>

#include <openvr_driver.h>
#include <DriverSettings.hpp>
#include <Eigen/eigen>
#include <ServerDriver.hpp>
#include <IDriverDevice.hpp>

class DebugGizmo : public IDriverDevice
{
public:
    DebugGizmo(std::shared_ptr<SettingsManager> settings_manager, std::function<void(DebugGizmo*)> pose_action);

    /// <summary>
    /// Updates the internal state of this device.
    /// Will be called every time ServerDriver::RunFrame is called
    /// </summary>
    virtual void update(std::vector<vr::VREvent_t> events) override;

    /// <summary>
    /// Activates this controller
    /// Is called when vr::VRServerDriverHost()->TrackedDeviceAdded is called
    /// </summary>
    /// <param name="index">OpenVR device index</param>
    /// <returns>Error code in case the device fails to activate</returns>
    virtual vr::EVRInitError Activate(vr::TrackedDeviceIndex_t index) override;

    /// <summary>
    /// Deactivates the controller
    /// </summary>
    virtual void Deactivate() override;

    /// <summary>
    /// Tells the controller to enter stand-by mode
    /// </summary>
    virtual void EnterStandby() override;

    /// <summary>
    /// Gets a specific component from this device
    /// </summary>
    /// <param name="pchComponentNameAndVersion">Requested component</param>
    /// <returns>Non-owning(?) pointer to the component</returns>
    virtual void* GetComponent(const char* pchComponentNameAndVersion) override;

    /// <summary>
    /// Handles a debug request
    /// </summary>
    /// <param name="pchRequest">Request type</param>
    /// <param name="pchResponseBuffer">Response buffer</param>
    /// <param name="unResponseBufferSize">Response buffer size</param>
    virtual void DebugRequest(const char* pchRequest, char* pchResponseBuffer, uint32_t unResponseBufferSize) override;

    /// <summary>
    /// Gets the current controller pose
    /// </summary>
    /// <returns>Device Pose</returns>
    virtual vr::DriverPose_t GetPose() override;

    /// <summary>
    /// Current OpenVR controller pose
    /// </summary>
    vr::DriverPose_t m_currentGizmoPose;

    /// <summary>
    /// The settings manager
    /// </summary>
    std::shared_ptr<SettingsManager> m_settingsManager;

    /// <summary>
    /// Returns a DriverPose_t that marks this devices pose as connected but not tracking.
    /// </summary>
    /// <returns></returns>
    vr::DriverPose_t getNotTrackingOpenVRPose();

    /// <summary>
    /// OpenVR device index
    /// </summary>
    vr::TrackedDeviceIndex_t m_deviceIndex = vr::k_unTrackedDeviceIndexInvalid;

    /// <summary>
    /// OpenVR properties handle
    /// </summary>
    vr::PropertyContainerHandle_t m_propertiesHandle = 0;

    /// <summary>
    /// Function that is run every frame to update the pose
    /// </summary>
    std::function<void(DebugGizmo*)> m_poseAction;
    
    // Inherited via IDriverDevice
    virtual vr::TrackedDeviceIndex_t getIndex() override;
};

