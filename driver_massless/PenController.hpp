/* Copyright (C) 2020 Massless Corp. - All Rights Reserved
 * Author: Jacob Hilton.
 * You may use, distribute and modify this code under the
 * terms of the BSD 3-Clause "New" or "Revised" License.
 *
 * You should have received a copy of this license with
 * this file. If not, please email support@massless.io
 */

#pragma once
#include <array>
#include <chrono>
#include <string>

#include <openvr_driver.h>
#include <Eigen/Eigen>

#include <SettingsManager.hpp>
#include <Handedness.hpp>
#include <IPenSystem.hpp>
#include <ServerDriver.hpp>
#include <IDriverDevice.hpp>
#include <MasslessManager.hpp>
#include <GestureHandler.hpp>

/// <summary>
/// OpenVR Massless Pen controller interface
/// </summary>
class PenController : public IDriverDevice
{
public:
    /// <summary>
    /// Constructs a PenController with the given driver settings and pen system
    /// </summary>
    /// <param name="settings_manager">Initialised driver settings</param>
    /// <param name="massless_manager">Massless pen system manager</param>
	PenController(std::shared_ptr<SettingsManager> settings_manager, std::shared_ptr<MasslessManager> massless_manager);

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
    /// Activates this controller
    /// Is called when vr::VRServerDriverHost()->TrackedDeviceAdded is called
    /// </summary>
    /// <param name="index">OpenVR device index</param>
    /// <param name="driver_input">IVRDriverInput pointer (ie. vr::VRDriverInput())</param>
    /// <param name="driver_properties">CVRPropertyHelpers pointer (ie. vr::VRProperties())</param>
    /// <returns>rror code in case the device fails to activate</returns>
    virtual vr::EVRInitError Activate(vr::TrackedDeviceIndex_t index, vr::IVRDriverInput* driver_input, vr::CVRPropertyHelpers* driver_properties);

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
    /// Logs currently waiting notifications from the Massless Pen System
    /// </summary>
    void logNotifications();

    /// <summary>
    /// Processes events from the Massless Pen Driver
    /// </summary>
    void processMasslessEvents(vr::IVRDriverInput* driver_input);

    /// <summary>
    /// Processes events from the OpenVR Server
    /// </summary>
    void processOpenVREvents(std::vector<vr::VREvent_t> events);
        
    /// <summary>
    /// Updates the ServerDriver host with the new pen pose
    /// </summary>
    /// <param name="new_pose">This frame's pen pose</param>
    /// <param name="serverdriver_host">Pointer to the ServerDriverHost (ie. vr::VRServerDriverHost())</param>
    void updatePenPose(const vr::DriverPose_t new_pose, vr::IVRServerDriverHost* serverdriver_host = vr::VRServerDriverHost());


    int m_unpressAllFrameTimeout = 0;
    const int m_unpressAllMaxTimeout = 10;
    const int m_unpressAllStopTimeout = -1;

    /// <summary>
    /// Input component handles
    /// </summary>
    vr::VRInputComponentHandle_t m_compSwipeForwards = vr::k_ulInvalidInputComponentHandle;
    vr::VRInputComponentHandle_t m_compSwipeBackwards = vr::k_ulInvalidInputComponentHandle;
    vr::VRInputComponentHandle_t m_compSingleFront = vr::k_ulInvalidInputComponentHandle;
    vr::VRInputComponentHandle_t m_compDoubleFront = vr::k_ulInvalidInputComponentHandle;
    vr::VRInputComponentHandle_t m_compSingleRear = vr::k_ulInvalidInputComponentHandle;
    vr::VRInputComponentHandle_t m_compDoubleRear = vr::k_ulInvalidInputComponentHandle;

private:
    
    /// <summary>
    /// Converts a Massless Pen Pose to an OpenVR Pose
    /// This conversion is based off of the pen's pose, the massless tracker's pose, and the native openvr base tracker's pose 
    /// </summary>
    /// <param name="pen_pose">Pose of the pen in the Massless coordinate space</param>
    /// <param name="tracking_ref_offset">Pose of the base tracker as an offset from the tracking reference pose</param>
    /// <param name="tracking_reference_pose">Pose in SteamVR global space of the tracking reference</param>
    /// <param name="tracking_ref_type">Type of tracking reference the pose is</param>
    /// <returns>Converted Driver Pose</returns>
    static vr::DriverPose_t makeOpenVRPose(const MasslessInterface::Pose& pen_pose, const MasslessInterface::Pose& tracking_ref_offset, const std::pair<Eigen::Quaternionf, Eigen::Vector3f>& tracking_reference_pose, const MasslessInterface::TrackingSystemType& tracking_ref_type);

    /// <summary>
    /// Returns a DriverPose_t that marks this devices pose as connected but not tracking.
    /// </summary>
    /// <returns></returns>
    static vr::DriverPose_t makeNotTrackingOpenVRPose();

    /// <summary>
    /// Returns a DriverPose_t that marks this devices pose as connected but calibrating (ie. looking for tracking reference).
    /// </summary>
    /// <returns></returns>
    static vr::DriverPose_t makeCalibratingOpenVRPose();

    /// <summary>
    /// Returns a DriverPose_t that marks this devices pose as disconnected.
    /// </summary>
    /// <returns></returns>
    static vr::DriverPose_t makeDisconnectedOpenVRPose();

    /// <summary>
    /// Called when a new gesture event is triggered
    /// </summary>
    /// <param name="event">The event struct</param>
    /// <param name="driver_input">OpenVR driver input pointer (vr::VRDriverInput())</param>
    void onGestureEvent(const MasslessInterface::PenEvent& event, vr::IVRDriverInput* driver_input);

    // Inherited via IDriverDevice
    virtual vr::TrackedDeviceIndex_t getIndex();

    /// <summary>
    /// OpenVR device index
    /// </summary>
	vr::TrackedDeviceIndex_t m_deviceIndex = vr::k_unTrackedDeviceIndexInvalid;

    /// <summary>
    /// OpenVR properties handle
    /// </summary>
	vr::PropertyContainerHandle_t m_propertiesHandle = 0;

    /// <summary>
    /// OpenVR haptic component
    /// </summary>
	vr::VRInputComponentHandle_t m_compHaptic = 0;

    /// <summary>
    /// Duration for a tap to vibrate the pen to simulate a click
    /// </summary>
    const int m_tapVibrationDuration = 50;

    /// <summary>
    /// Current OpenVR controller pose
    /// </summary>
    vr::DriverPose_t m_currentPenPose;

    /// <summary>
    /// Supplied driver settings struct
    /// </summary>
    std::shared_ptr<SettingsManager> m_settingsManager;
    
    /// <summary>
    /// Pointer to the massless manager
    /// </summary>
    std::shared_ptr<MasslessManager> m_masslessManager;

    /// <summary>
    /// Handlers for pen gestures
    /// </summary>
    std::vector<GestureHandler> m_gestureHandlers;

    /// <summary>
    /// Stored stack of pen gestures for matching with handlers
    /// </summary>
    std::vector<MasslessInterface::PenEvent> m_eventStack;
    
};

