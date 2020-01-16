/* Copyright (C) 2020 Massless Corp. - All Rights Reserved
 * Author: Jacob Hilton.
 * You may use, distribute and modify this code under the
 * terms of the BSD 3-Clause "New" or "Revised" License.
 *
 * You should have received a copy of this license with
 * this file. If not, please email support@massless.io
 */

#include "DebugGizmo.hpp"
#include "DriverLog.hpp"

DebugGizmo::DebugGizmo(std::shared_ptr<SettingsManager> settings_manager, std::function<void(DebugGizmo*)> pose_action):
    m_settingsManager(settings_manager),
    m_poseAction(pose_action)
{
    this->m_currentGizmoPose = this->getNotTrackingOpenVRPose();
}

void DebugGizmo::update(std::vector<vr::VREvent_t> events)
{
    if (this->m_deviceIndex != vr::k_unTrackedDeviceIndexInvalid)
    {
        this->m_poseAction(this);
    }
}

vr::EVRInitError DebugGizmo::Activate(vr::TrackedDeviceIndex_t index)
{
    this->m_deviceIndex = index;
    this->m_propertiesHandle = vr::VRProperties()->TrackedDeviceToPropertyContainer(this->m_deviceIndex);
    vr::VRProperties()->SetUint64Property(this->m_propertiesHandle, vr::Prop_CurrentUniverseId_Uint64, 2);

    vr::VRProperties()->SetStringProperty(this->m_propertiesHandle, vr::Prop_RenderModelName_String, "locator");

    return vr::EVRInitError::VRInitError_None;
}

void DebugGizmo::Deactivate()
{
    this->m_deviceIndex = vr::k_unTrackedDeviceIndexInvalid;
}

void DebugGizmo::EnterStandby()
{
}

void* DebugGizmo::GetComponent(const char* pchComponentNameAndVersion)
{
    return nullptr;
}

void DebugGizmo::DebugRequest(const char* pchRequest, char* pchResponseBuffer, uint32_t unResponseBufferSize)
{
    if (unResponseBufferSize >= 1)
        pchResponseBuffer[0] = 0;
}

vr::DriverPose_t DebugGizmo::GetPose()
{
    return this->m_currentGizmoPose;
}

vr::DriverPose_t DebugGizmo::getNotTrackingOpenVRPose()
{
    vr::DriverPose_t out_pose = { 0 };
    out_pose.deviceIsConnected = false;
    out_pose.poseIsValid = false;
    out_pose.result = vr::TrackingResult_Uninitialized;
    out_pose.willDriftInYaw = false;
    out_pose.shouldApplyHeadModel = false;

    return out_pose;
}

vr::TrackedDeviceIndex_t DebugGizmo::getIndex()
{
    return this->m_deviceIndex;
}
