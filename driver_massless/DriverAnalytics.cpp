/* Copyright (C) 2020 Massless Corp. - All Rights Reserved
 * Author: Jacob Hilton.
 * You may use, distribute and modify this code under the
 * terms of the BSD 3-Clause "New" or "Revised" License.
 *
 * You should have received a copy of this license with
 * this file. If not, please email support@massless.io
 */

#include "DriverAnalytics.hpp"
#include <iostream>
#include <VRProcessEnumerator.hpp>

using SystemMatcher = std::pair<std::regex, MasslessInterface::TrackingSystemType>;
std::optional<std::wstring> DriverAnalytics::getVRClientProcessName(unsigned long pid)
{
    return VRProcessEnumerator::getVRClientProcessName(pid);
}

//These matchers are for matching the ModelNumber string (more of a name if you ask me)
SystemMatcher vive_lighthouse_v1_matcher{ std::regex("HTC V2-XD/XE", std::regex_constants::ECMAScript | std::regex_constants::icase), MasslessInterface::TrackingSystemType::SystemType::VIVE_BASESTATION_V1 };
SystemMatcher vive_lighthouse_v2_matcher{ std::regex("Valve SR Imp", std::regex_constants::ECMAScript | std::regex_constants::icase), MasslessInterface::TrackingSystemType::SystemType::VIVE_BASESTATION_V2 };
SystemMatcher vive_tracker_v2_matcher{ std::regex("VIVE Tracker Pro MV", std::regex_constants::ECMAScript | std::regex_constants::icase), MasslessInterface::TrackingSystemType::SystemType::VIVE_TRACKER };
SystemMatcher oculus_camera_matcher{ std::regex("Oculus Rift CV1 \\(Camera\\d+\\)", std::regex_constants::ECMAScript | std::regex_constants::icase), MasslessInterface::TrackingSystemType::SystemType::OCULUS_SENSOR };
SystemMatcher oculus_rift_s_right_controller_matcher{ std::regex("Oculus Rift S \\((Right) Controller\\)", std::regex_constants::ECMAScript | std::regex_constants::icase), MasslessInterface::TrackingSystemType::SystemType::OCULUS_RIFT_S_TOUCH_RIGHT };
SystemMatcher oculus_rift_s_left_controller_matcher{ std::regex("Oculus Rift S \\((Left) Controller\\)", std::regex_constants::ECMAScript | std::regex_constants::icase), MasslessInterface::TrackingSystemType::SystemType::OCULUS_RIFT_S_TOUCH_LEFT };
SystemMatcher oculus_quest_right_controller_matcher{ std::regex("Oculus Quest \\((Right) Controller\\)", std::regex_constants::ECMAScript | std::regex_constants::icase), MasslessInterface::TrackingSystemType::SystemType::OCULUS_RIFT_S_TOUCH_RIGHT };
SystemMatcher oculus_quest_left_controller_matcher{ std::regex("Oculus Quest \\((Left) Controller\\)", std::regex_constants::ECMAScript | std::regex_constants::icase), MasslessInterface::TrackingSystemType::SystemType::OCULUS_RIFT_S_TOUCH_LEFT };

//This matcher is for matching the rendermodelname string, which is a fallback, currently only in use for the Vive Tracker V1
SystemMatcher vive_tracker_v1_matcher{ std::regex("\\{htc\\}vr_tracker_vive_1_0", std::regex_constants::ECMAScript | std::regex_constants::icase), MasslessInterface::TrackingSystemType::SystemType::VIVE_TRACKER };

std::vector<SystemMatcher> matchers = { vive_tracker_v2_matcher, vive_lighthouse_v1_matcher, vive_lighthouse_v2_matcher, oculus_camera_matcher, oculus_rift_s_right_controller_matcher, oculus_rift_s_left_controller_matcher, oculus_quest_left_controller_matcher, oculus_quest_right_controller_matcher};

std::optional<std::pair<vr::TrackedDeviceIndex_t, MasslessInterface::TrackingSystemType>> DriverAnalytics::findLikelyTrackingReferenceIndex(vr::IVRServerDriverHost* serverdriver_host, vr::CVRPropertyHelpers* properties)
{
    vr::TrackedDevicePose_t tracked_device_poses[vr::k_unMaxTrackedDeviceCount];
    serverdriver_host->GetRawTrackedDevicePoses(0, tracked_device_poses, vr::k_unMaxTrackedDeviceCount);
    for (vr::TrackedDeviceIndex_t device_index = vr::k_unTrackedDeviceIndex_Hmd; device_index < vr::k_unMaxTrackedDeviceCount; device_index++) {
        const auto& device_pose = tracked_device_poses[device_index];
        // Check if this device pose is valid and the device is connected
        if (device_pose.bDeviceIsConnected) {
            if (isTrackingReference(device_index, properties)) {
                return { { device_index, getReferenceSystemType(device_index, properties) } };
            }
        }
    }
    return std::nullopt;
}

bool DriverAnalytics::isTrackingReference(vr::TrackedDeviceIndex_t device_index, vr::CVRPropertyHelpers* properties)
{
    vr::PropertyContainerHandle_t device_props = properties->TrackedDeviceToPropertyContainer(device_index);
    vr::ETrackedPropertyError error = vr::ETrackedPropertyError::TrackedProp_Success;
    std::string device_model_number = properties->GetStringProperty(device_props, vr::ETrackedDeviceProperty::Prop_ModelNumber_String, &error);
    if (error != vr::ETrackedPropertyError::TrackedProp_Success)
    {
        // Try again on a backup property (currently only Vive Tracker V1 doesn't have a model number string).
        std::string device_render_model = properties->GetStringProperty(device_props, vr::ETrackedDeviceProperty::Prop_RenderModelName_String, &error);
        if (error == vr::ETrackedPropertyError::TrackedProp_Success) {
            //DriverLog("[Info] Failed to get system type, falling back to device render model, which is %s.\n", device_render_model);
            if (std::regex_search(device_render_model, vive_tracker_v1_matcher.first)) {
                //DriverLog("[Info] Deciding if a device is a tracking reference or not, it's a Vive Tracker V1.\n");
                //DriverLog("[Info] Found a Vive Tracker V1.");
                return true;
            }
        }
        return false;
    }
    return !std::none_of(matchers.begin(), matchers.end(), [=](const auto& matcher) { return std::regex_search(device_model_number, matcher.first);});
}

MasslessInterface::TrackingSystemType DriverAnalytics::getReferenceSystemType(vr::TrackedDeviceIndex_t device_index, vr::CVRPropertyHelpers* properties)
{
    //DriverLog("[Info] Finding type of device for device with index: %d.\n", device_index);
    vr::PropertyContainerHandle_t device_props = properties->TrackedDeviceToPropertyContainer(device_index);
    vr::ETrackedPropertyError error = vr::ETrackedPropertyError::TrackedProp_Success;
    std::string device_model_number = properties->GetStringProperty(device_props, vr::ETrackedDeviceProperty::Prop_ModelNumber_String, &error);
    if (error != vr::ETrackedPropertyError::TrackedProp_Success) {
        // Try again on a backup property (currently only Vive Tracker V1 doesn't have a model number string).
        std::string device_render_model = properties->GetStringProperty(device_props, vr::ETrackedDeviceProperty::Prop_RenderModelName_String, &error);
        if (error == vr::ETrackedPropertyError::TrackedProp_Success) {
            //DriverLog("[Info] Failed to get system type, falling back to device render model, which is %s.\n", device_render_model);
            if (std::regex_search(device_render_model, vive_tracker_v1_matcher.first)) {
                //DriverLog("[Info] Found a Vive Tracker V1.");
                return vive_tracker_v1_matcher.second;
            }
        }
        DriverLog("[Error] Found a device that didn't match any type with failure to read the model.\n");
        return MasslessInterface::TrackingSystemType::SystemType::INVALID_SYSTEM;
    }
    //DriverLog("[Info] Finding system type where model number string is: %s.\n", device_model_number.c_str());
    for (const auto& matcher : matchers) {
        if (std::regex_search(device_model_number, matcher.first)) return matcher.second;
    }
    //If it hasn't matched anything else yet, try a vive tracker V1
    std::string device_render_model = properties->GetStringProperty(device_props, vr::ETrackedDeviceProperty::Prop_RenderModelName_String, &error);
    if (std::regex_search(device_render_model, vive_tracker_v1_matcher.first)) {
        //DriverLog("[Info] Found a Vive Tracker V1.");
        return vive_tracker_v1_matcher.second;
    }
    DriverLog("[Error] Found a device that didn't match any type.\n");
    return MasslessInterface::TrackingSystemType::SystemType::INVALID_SYSTEM;
}

std::optional<std::string> DriverAnalytics::getDeviceSerial(vr::TrackedDeviceIndex_t device_index, vr::CVRPropertyHelpers* properties)
{
    vr::PropertyContainerHandle_t device_props = properties->TrackedDeviceToPropertyContainer(device_index);
    vr::ETrackedPropertyError error = vr::ETrackedPropertyError::TrackedProp_Success;
    std::string device_serial_number = properties->GetStringProperty(device_props, vr::ETrackedDeviceProperty::Prop_SerialNumber_String, &error);
    if (error != vr::ETrackedPropertyError::TrackedProp_Success) {
        return std::nullopt;
    }
    return device_serial_number;
}

std::optional<std::pair<Eigen::Quaternionf, Eigen::Vector3f>> DriverAnalytics::getDevicePose(vr::TrackedDeviceIndex_t device_index, vr::IVRServerDriverHost* serverdriver_host)
{
    vr::TrackedDevicePose_t poses[vr::k_unMaxTrackedDeviceCount];
    serverdriver_host->GetRawTrackedDevicePoses(0, poses, vr::k_unMaxTrackedDeviceCount);
    vr::TrackedDevicePose_t device_pose = poses[device_index];
    if (device_pose.bDeviceIsConnected && device_pose.bPoseIsValid) {
        return hmdMatrixtoEigenPose(device_pose.mDeviceToAbsoluteTracking);
    }
    return std::nullopt;
}

std::pair<Eigen::Quaternionf, Eigen::Vector3f> DriverAnalytics::hmdMatrixtoEigenPose(const vr::HmdMatrix34_t& mat) {

    Eigen::Vector3f device_translation(mat.m[0][3], mat.m[1][3], mat.m[2][3]);
    Eigen::Matrix3f device_rotmat = (Eigen::Matrix3f() << mat.m[0][0], mat.m[0][1], mat.m[0][2], mat.m[1][0], mat.m[1][1], mat.m[1][2], mat.m[2][0], mat.m[2][1], mat.m[2][2]).finished();
    Eigen::Quaternionf device_rotation(device_rotmat);

    return std::make_pair(device_rotation, device_translation);
}

std::optional<vr::TrackedDeviceIndex_t> DriverAnalytics::getDeviceIndex(std::string device_serial, vr::IVRServerDriverHost* serverdriver_host, vr::CVRPropertyHelpers* properties)
{
    // Get all currently tracked devices
    vr::TrackedDevicePose_t poses[vr::k_unMaxTrackedDeviceCount];
    serverdriver_host->GetRawTrackedDevicePoses(0, poses, vr::k_unMaxTrackedDeviceCount);
    for (vr::TrackedDeviceIndex_t index = 0; index < vr::k_unMaxTrackedDeviceCount; index++) {

        // Check it is actually a connected and working device
        vr::TrackedDevicePose_t device_pose = poses[index];
        if (device_pose.bDeviceIsConnected) {

            // Check the serial number
            auto serial = getDeviceSerial(index, properties);
            if (serial.has_value() && *serial == device_serial) {
                return index;
            }
        }
    }
    return std::nullopt;
}

std::vector<vr::TrackedDeviceIndex_t> DriverAnalytics::firstPass(vr::IVRServerDriverHost* serverdriver_host, vr::CVRPropertyHelpers* properties)
{
    vr::TrackedDevicePose_t tracked_device_poses[vr::k_unMaxTrackedDeviceCount];
    serverdriver_host->GetRawTrackedDevicePoses(0, tracked_device_poses, vr::k_unMaxTrackedDeviceCount);
    std::vector<vr::TrackedDeviceIndex_t> indices;
    for (vr::TrackedDeviceIndex_t idx = 0; idx < vr::k_unMaxTrackedDeviceCount; idx++) {
        if (tracked_device_poses[idx].bDeviceIsConnected && isTrackingReference(idx, properties)) {
            indices.push_back(idx);
        }
    }
    std::sort(std::begin(indices), std::end(indices), 
        [&, properties](vr::TrackedDeviceIndex_t a, vr::TrackedDeviceIndex_t b) {
            int priorityA = getReferenceSystemType(a, properties).getSystemType() == MasslessInterface::TrackingSystemType::SystemType::VIVE_TRACKER ? 0 : 1;
            int priorityB = getReferenceSystemType(b, properties).getSystemType() == MasslessInterface::TrackingSystemType::SystemType::VIVE_TRACKER ? 0 : 1;
            return priorityA < priorityB;
        }
    );

    return indices;
}

std::optional<DriverAnalytics::TrackingReferencePack> DriverAnalytics::secondPass(vr::TrackedDeviceIndex_t tracking_reference_index, vr::IVRServerDriverHost* serverdriver_host, vr::CVRPropertyHelpers* properties, std::shared_ptr<MasslessInterface::IPenSystem> pen_system)
{

    auto system_type = getReferenceSystemType(tracking_reference_index, properties);
    if (system_type.getSystemType() == MasslessInterface::TrackingSystemType::SystemType::INVALID_SYSTEM)
    {
        return std::nullopt;
    }

    auto device_serial = getDeviceSerial(tracking_reference_index, properties);
    if (!device_serial.has_value())
        return std::nullopt;

    auto pose_offset = pen_system->getMasslessTrackerPoseOffset(system_type);
    if (!pose_offset.has_value())
    {
        return std::nullopt;
    }

    auto global_pose = getDevicePose(tracking_reference_index, serverdriver_host);
    if (!global_pose.has_value())
    {
        return std::nullopt;
    }

    return DriverAnalytics::TrackingReferencePack(tracking_reference_index, *device_serial, system_type, *global_pose, *pose_offset);
    
}

std::optional<DriverAnalytics::TrackingReferencePack> DriverAnalytics::secondPass(std::string tracking_reference_serial, vr::IVRServerDriverHost* serverdriver_host, vr::CVRPropertyHelpers* properties, std::shared_ptr<MasslessInterface::IPenSystem> pen_system)
{
    auto tracking_reference_index = getDeviceIndex(tracking_reference_serial, serverdriver_host, properties);
    if (!tracking_reference_index.has_value())
        return std::nullopt;

    auto system_type = getReferenceSystemType(*tracking_reference_index, properties);
    if (system_type.getSystemType() == MasslessInterface::TrackingSystemType::SystemType::INVALID_SYSTEM)
    {
        return std::nullopt;
    }

    auto pose_offset = pen_system->getMasslessTrackerPoseOffset(system_type);
    if (!pose_offset.has_value())
    {
        return std::nullopt;
    }

    auto global_pose = getDevicePose(*tracking_reference_index, serverdriver_host);
    if (!global_pose.has_value())
    {
        return std::nullopt;
    }

    return DriverAnalytics::TrackingReferencePack(*tracking_reference_index, tracking_reference_serial, system_type, *global_pose, *pose_offset);
}
