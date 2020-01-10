/* Copyright (C) 2020 Massless Corp. - All Rights Reserved
 * Author: Jacob Hilton.
 * You may use, distribute and modify this code under the
 * terms of the BSD 3-Clause "New" or "Revised" License.
 *
 * You should have received a copy of this license with
 * this file. If not, please email support@massless.io
 */

#pragma once
#include <memory>
#include <string>
#include <cstdlib>
#include <optional>
#include <array>
#include <iostream>
#include <locale>
#include <sstream>

#include <Eigen/eigen>
#include <openvr_driver.h>

#include <DriverVersion.hpp>
#include <DriverLog.hpp>
#include <MasslessInterface.hpp>
#include <PenController.hpp>
#include <DebugGizmo.hpp>
#include <FileSettingsLoader.hpp>
#include <SettingsManager.hpp>
#include <SettingsUtilities.hpp>
#include <IDriverDevice.hpp>
#include <DriverAnalytics.hpp>
#include <Handedness.hpp>
#include <MasslessManager.hpp>


/// <summary>
/// OpenVR ServerDriver for the Massless Pen System
/// </summary>
class ServerDriver : public vr::IServerTrackedDeviceProvider
{
public:

    ServerDriver(std::shared_ptr<MasslessInterface::IPenSystem> pen_system);
    
    static const std::shared_ptr<ServerDriver> create(std::shared_ptr<MasslessInterface::IPenSystem> pen_system);

    /// <summary>
    /// Creates a new instance of the ServerDriver with the supplied pen_system, and returns it.
    /// </summary>
    /// <param name="pen_system">New pen system to use</param>
    /// <returns>Pointer to the instance</returns>
    static const std::shared_ptr<ServerDriver> instance();

    /// <summary>
    /// Called when this driver is initialised
    /// </summary>
    /// <param name="driver_context">Driver context ptr</param>
    /// <returns>Error code if any errors occurred during startup</returns>
	virtual vr::EVRInitError Init(vr::IVRDriverContext* driver_context) override;

    /// <summary>
    /// Cleans up the driver instance
    /// </summary>
	virtual void Cleanup() override;

    /// <summary>
    /// Gets the interface versions this ServerDriver was built for
    /// </summary>
    /// <returns>Interface versions array</returns>
	virtual const char* const* GetInterfaceVersions() override;

    /// <summary>
    /// Is called whenever this ServerDriver is to update its state to OpenVR
    /// </summary>
	virtual void RunFrame() override;

    /// <summary>
    /// Check to whether this ServerDriver should block standby mode
    /// </summary>
    /// <returns>false</returns>
	virtual bool ShouldBlockStandbyMode() override;

    /// <summary>
    /// Called when standby is entered
    /// </summary>
	virtual void EnterStandby() override;

    /// <summary>
    /// Called when standby is left
    /// </summary>
	virtual void LeaveStandby() override;

    /// <summary>
    /// Tries to add a device to the serverdriver_host
    /// </summary>
    /// <param name="device">Pointer to the device, this will be moved into the internal device array</param>
    /// <param name="serial">Serial number of the device</param>
    /// <param name="device_class">Device class (Controller, camera, etc)</param>
    /// <param name="serverdriver_host">ServerDriverHost pointer (usually vr::VRServerDriverHost())</param>
    /// <returns>true on success, false on failure</returns>
    bool tryAddDevice(std::unique_ptr<IDriverDevice> device, std::string serial, vr::ETrackedDeviceClass device_class, vr::IVRServerDriverHost* serverdriver_host = vr::VRServerDriverHost());
     
    /// <summary>
    /// Returns a pointer to the settings manager for this driver
    /// </summary>
    /// <returns>Pointer to the settings manager</returns>
    std::shared_ptr<SettingsManager> getSettingsManager();

    /// <summary>
    /// Gets the current tracking reference information
    /// </summary>
    /// <returns>Current tracking reference information, or nullopt if none is currently set</returns>
    std::optional<DriverAnalytics::TrackingReferencePack> getTrackingReference();

private:

    /// <summary>
    /// Updates the internal tracking reference state
    /// </summary>
    /// <param name="serverdriver_host">IVRServerDriverHost pointer (usually vr::VRServerDriverHost())</param>
    /// <param name="properties">CVRPropertyHelpers pointer (usually vr::VRProperties())</param>
    void updateTrackingReference(vr::IVRServerDriverHost* serverdriver_host, vr::CVRPropertyHelpers* properties);

    /// <summary>
    /// Sets a new tracking reference and updates the auto tracking reference settings key
    /// <param name="new_tracking_reference_pack">The new tracking reference, can be nullopt to remove the tracking reference</param>
    /// </summary>
    void setTrackingReference(std::optional<DriverAnalytics::TrackingReferencePack> new_tracking_reference_pack);

    /// <summary>
    /// Processes OpenVR events
    /// </summary>
    /// <param name="serverdriver_host">IVRServerDriverHost pointer (usually vr::VRServerDriverHost())</param>
    /// <param name="properties">CVRPropertyHelpers pointer (usually vr::VRProperties())</param>
    /// <returns>List of events processed</returns>
    std::vector<vr::VREvent_t> processEvents(vr::IVRServerDriverHost* serverdriver_host, vr::CVRPropertyHelpers* properties);

	/// <summary>
	/// Static instance to pass to the driver factory.
	/// Pointer is "pinned" here as to last the lifetime of the program
	/// </summary>
	static std::shared_ptr<ServerDriver> m_driverInstance;

	/// <summary>
	/// Devices supplied by this driver
	/// </summary>
    std::vector<std::unique_ptr<IDriverDevice>> m_devices;

    /// <summary>
    /// This driver's settings manager
    /// </summary>
    std::shared_ptr<SettingsManager> m_settingsManager;

    /// <summary>
    /// This driver's massless manager
    /// </summary>
    std::shared_ptr<MasslessManager> m_masslessManager;

    /// <summary>
    /// Time point when the driver started
    /// </summary>
    std::chrono::time_point<std::chrono::system_clock> m_timeDriverStart;
    
    /// <summary>
    /// Look for a new tracking reference when the previous one disconnects?
    /// </summary>
    bool m_doRetryOnDisconnect = true;

    /// <summary>
    /// Look for a new tracking reference when a new device connects?
    /// </summary>
    bool m_doRetryOnConnect = true;

    /// <summary>
    /// Do we look forever for a tracking reference with the given serial?
    /// </summary>
    bool m_doLookForever = false;

    /// <summary>
    /// Do we continuously update the tracking reference pose?
    /// </summary>
    bool m_doContinuousPoseUpdate = false;

    /// <summary>
    /// Did a tracking reference disconnect?
    /// </summary>
    bool m_didReferenceDisconnect = false;

    /// <summary>
    /// Time that the tracking reference disconnected
    /// </summary>
    std::chrono::system_clock::time_point m_timeReferenceDisconnect;

    /// <summary>
    /// Timeout for looking for device with this->m_initialSerial serial
    /// </summary>
    std::chrono::seconds m_searchTimeout{ 5 };

    /// <summary>
    /// Serial number for the forced tracking reference
    /// </summary>
    std::optional<std::string> m_trackingReferenceSerialHint;

    /// <summary>
    /// Information about the tracking reference
    /// </summary>
    std::optional<DriverAnalytics::TrackingReferencePack> m_trackingReferencePack;

    /// <summary>
    /// Has the pen been added yet?
    /// </summary>
    bool m_hasAddedPen = false;

    /// <summary>
    /// Was the driver successfully initialised?
    /// </summary>
    bool m_initSuccess = false;

    /// <summary>
    /// Temporary hacky fix for the Vive Tracker not appearing in the list when its "Tracker Attached" event fires.
    /// </summary>
    int m_TrackingReferenceSearchCountdown = 0;
};
