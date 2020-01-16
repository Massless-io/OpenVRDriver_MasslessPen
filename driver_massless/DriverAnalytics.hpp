/* Copyright (C) 2020 Massless Corp. - All Rights Reserved
 * Author: Jacob Hilton.
 * You may use, distribute and modify this code under the
 * terms of the BSD 3-Clause "New" or "Revised" License.
 *
 * You should have received a copy of this license with
 * this file. If not, please email support@massless.io
 */

#pragma once

#include <regex>
#include <cstdint>
#include <optional>
#include <string>
#include <array>
#include <future>

#include <openvr_driver.h>
#include <Eigen/eigen>

#include <TrackingSystemType.hpp>
#include <IPenSystem.hpp>
#include <Pose.hpp>

class DriverAnalytics
{
public:

    static std::optional<std::wstring> getVRClientProcessName(unsigned long pid);

    /// <summary>
    /// Finds the most likely connected device to be the tracking reference.
    /// </summary>
    /// <param name="serverdriver_host">IVRServerDriverHost pointer (usually vr::VRServerDriverHost())</param>
    /// <param name="properties">CVRPropertyHelpers pointer (usually vr::VRProperties())</param>
    /// <returns>The index of the device, and the tracking system type of the device, or nullopt on failure to find a device</returns>
    static std::optional<std::pair<vr::TrackedDeviceIndex_t, MasslessInterface::TrackingSystemType>> findLikelyTrackingReferenceIndex(vr::IVRServerDriverHost* serverdriver_host, vr::CVRPropertyHelpers* properties);
   
    /// <summary>
    /// Checks if the device at the given tracked device index is a valid tracking reference
    /// </summary>
    /// <param name="device_index">The index of the device in the SteamVR tracked device list</param>
    /// <param name="properties">CVRPropertyHelpers pointer (usually vr::VRProperties())</param>
    /// <returns>True if device is a valid reference, false if not</returns>
    static bool isTrackingReference(vr::TrackedDeviceIndex_t device_index, vr::CVRPropertyHelpers* properties);

    /// <summary>
    /// Gets the system type of a device by tracked device index
    /// </summary>
    /// <param name="device_index">device index</param>
    /// <param name="properties">CVRPropertyHelpers pointer (usually vr::VRProperties())</param>
    /// <returns>System type of the device, or INVALID_SYSTEM if it is not supported, an error occurred getting the properties, or is not a tracking reference</returns>
    static MasslessInterface::TrackingSystemType getReferenceSystemType(vr::TrackedDeviceIndex_t device_index, vr::CVRPropertyHelpers* properties);

    /// <summary>
    /// Gets the serial of a tracked device
    /// </summary>
    /// <param name="device_index">device index</param>
    /// <param name="properties">CVRPropertyHelpers pointer (usually vr::VRProperties())</param>
    /// <returns>Device serial, or nullopt on error</returns>
    static std::optional<std::string> getDeviceSerial(vr::TrackedDeviceIndex_t device_index, vr::CVRPropertyHelpers* properties);

    /// <summary>
    /// Gets the pose of a tracked device
    /// </summary>
    /// <param name="device_index">device index</param>
    /// <param name="serverdriver_host">IVRServerDriverHost pointer (usually vr::VRServerDriverHost())</param>
    /// <returns>Device pose, or nullopt on error</returns>
    static std::optional<std::pair<Eigen::Quaternionf, Eigen::Vector3f>> getDevicePose(vr::TrackedDeviceIndex_t device_index, vr::IVRServerDriverHost* serverdriver_host);

    /// <summary>
    /// Converts an OpenVR pose to an Eigen pair
    /// </summary>
    /// <param name="mat">OpenVR pose</param>
    /// <returns>Input pose converts to an Eigen pair</returns>
    static std::pair<Eigen::Quaternionf, Eigen::Vector3f> hmdMatrixtoEigenPose(const vr::HmdMatrix34_t& mat);

    /// <summary>
    /// Gets the index of a tracked device based on its serial
    /// </summary>
    /// <param name="device_serial">device serial</param>
    /// <param name="serverdriver_host">IVRServerDriverHost pointer (usually vr::VRServerDriverHost())</param>
    /// <param name="properties">CVRPropertyHelpers pointer (usually vr::VRProperties())</param>
    /// <returns>index of the device, or nullopt on failure to find device</returns>
    static std::optional<vr::TrackedDeviceIndex_t> getDeviceIndex(std::string device_serial, vr::IVRServerDriverHost* serverdriver_host, vr::CVRPropertyHelpers* properties);

    /// <summary>
    /// First pass for finding the tracking reference. This pass finds all of the valid tracking references currently connected, and returns a list of them sorted by priority (0 = highest)
    /// </summary>
    /// <param name="serverdriver_host">IVRServerDriverHost pointer (usually vr::VRServerDriverHost())</param>
    /// <param name="properties">CVRPropertyHelpers pointer (usually vr::VRProperties())</param>
    /// <returns>List of possible tracking references, sorted by priority (0 = highest)</returns>
    static std::vector<vr::TrackedDeviceIndex_t> firstPass(vr::IVRServerDriverHost* serverdriver_host, vr::CVRPropertyHelpers* properties);
    
    /// <summary>
    /// A set of info about a tracking reference
    /// </summary>
    struct TrackingReferencePack {
        TrackingReferencePack(vr::TrackedDeviceIndex_t index, std::string device_serial, MasslessInterface::TrackingSystemType type, std::pair<Eigen::Quaternionf, Eigen::Vector3f> global_pose, MasslessInterface::Pose pose_offset) :
            index(index), device_serial(device_serial), type(type), global_pose(global_pose), pose_offset(pose_offset) {}
        vr::TrackedDeviceIndex_t index;
        std::string device_serial;
        MasslessInterface::TrackingSystemType type;
        std::pair<Eigen::Quaternionf, Eigen::Vector3f> global_pose;
        MasslessInterface::Pose pose_offset;
    };

    /// <summary>
    /// Second pass for finding the tracking reference. This pass gets all of the information about the tracking reference based on its index and returns it packed into the struct.
    /// </summary>
    /// <param name="tracking_reference_index">device index</param>
    /// <param name="serverdriver_host">IVRServerDriverHost pointer (usually vr::VRServerDriverHost())</param>
    /// <param name="properties">CVRPropertyHelpers pointer (usually vr::VRProperties())</param>
    /// <returns>Filled TrackingReferencePack with all of the information set, or nullopt if any property cannot be loaded</returns>
    static std::optional<TrackingReferencePack> secondPass(vr::TrackedDeviceIndex_t tracking_reference_index, vr::IVRServerDriverHost* serverdriver_host, vr::CVRPropertyHelpers* properties, std::shared_ptr<MasslessInterface::IPenSystem> pen_system);

    /// <summary>
    /// Second pass for finding the tracking reference. This pass gets all of the information about the tracking reference based on its serial and returns it packed into the struct.
    /// </summary>
    /// <param name="tracking_reference_index">device index</param>
    /// <param name="serverdriver_host">IVRServerDriverHost pointer (usually vr::VRServerDriverHost())</param>
    /// <param name="properties">CVRPropertyHelpers pointer (usually vr::VRProperties())</param>
    /// <returns>Filled TrackingReferencePack with all of the information set, or nullopt if any property cannot be loaded</returns>
    static std::optional<TrackingReferencePack> secondPass(std::string tracking_reference_serial, vr::IVRServerDriverHost* serverdriver_host, vr::CVRPropertyHelpers* properties, std::shared_ptr<MasslessInterface::IPenSystem> pen_system);
};

