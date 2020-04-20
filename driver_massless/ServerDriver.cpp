/* Copyright (C) 2020 Massless Corp. - All Rights Reserved
 * Author: Jacob Hilton.
 * You may use, distribute and modify this code under the
 * terms of the BSD 3-Clause "New" or "Revised" License.
 *
 * You should have received a copy of this license with
 * this file. If not, please email support@massless.io
 */

#include <ServerDriver.hpp>
#include <Windows.h>

using namespace vr;

std::shared_ptr<ServerDriver> ServerDriver::m_driverInstance;

const std::shared_ptr<ServerDriver> ServerDriver::instance()
{
    return m_driverInstance;
}

EVRInitError ServerDriver::Init(IVRDriverContext* driver_context)
{
	// Perform driver context initialisation
	if (EVRInitError init_error = InitServerDriverContext(driver_context); init_error != EVRInitError::VRInitError_None) {
		return init_error;
	}

    // Set start time point
    this->m_timeDriverStart = std::chrono::system_clock::now();
    
    // Init driver log
    InitDriverLog(vr::VRDriverLog());
    DriverLog("==========================================================================\n");
    DriverLog("================================ Massless ================================\n");
    DriverLog("==========================================================================\n");

    
	

    std::filesystem::path config_path;
    // Load settings
    try {
        config_path = SettingsUtilities::getMasslessConfigPath();
    }
    catch (const DriverSettingsException & e) {
        DriverLog("[Error] Unable to load config file path due to [%d]: %s\n", e.code().value(), e.what());
        return EVRInitError::VRInitError_Init_NoConfigPath;
    }

    try {
        m_settingsManager = std::make_shared<SettingsManager>(std::make_unique<FileSettingsLoader>(config_path));
    }
    catch (const DriverSettingsException& e) {
        DriverLog("[Error] Unable to load config due to [%d]: %s\n", e.code().value(), e.what());
        return EVRInitError::VRInitError_Init_SettingsInitFailed;
    }

    if (!m_settingsManager->getSettings().areAllValid()) {
        const auto& settingsKeys = DriverSettings::getKeys();
        for (auto it = settingsKeys.begin(); it != settingsKeys.end(); ++it) {
            if (!m_settingsManager->getSettings().isValid(it->first)) {
                DriverLog("[Error] Setting [%s] is invalid\n", DriverSettings::getKeyString(it->first).c_str());
            }
        }
        return EVRInitError::VRInitError_Init_SettingsInitFailed;
    }

    // Attach gizmo if requested
    if (this->m_settingsManager->getSettings().getValue<bool>(DriverSettings::AttachGizmo).value_or(false)) {
        // Setup update function for tracking reference pose gizmo
        auto tracking_ref_gizmo_update_fn = [](DebugGizmo* thiz) {
            vr::DriverPose_t out_pose = { 0 };
            if (auto ref_pose = ServerDriver::instance()->getTrackingReference(); ref_pose.has_value()) {

                // Set up some default values that should be in every pose
                out_pose.deviceIsConnected = true;
                out_pose.poseIsValid = true;
                out_pose.result = vr::ETrackingResult::TrackingResult_Running_OK;
                out_pose.willDriftInYaw = false;
                out_pose.shouldApplyHeadModel = false;
                out_pose.qDriverFromHeadRotation.w = out_pose.qWorldFromDriverRotation.w = out_pose.qRotation.w = 1.0;


                // Tracking reference pose in steamvr space
                const Eigen::Quaternionf& tracking_ref_steamvr_space_rotation = ref_pose->global_pose.first;
                const Eigen::Vector3f& tracking_ref_steamvr_space_translation = ref_pose->global_pose.second;

                out_pose.qRotation.w = tracking_ref_steamvr_space_rotation.w();
                out_pose.qRotation.x = tracking_ref_steamvr_space_rotation.x();
                out_pose.qRotation.y = tracking_ref_steamvr_space_rotation.y();
                out_pose.qRotation.z = tracking_ref_steamvr_space_rotation.z();

                out_pose.vecPosition[0] = tracking_ref_steamvr_space_translation.x();
                out_pose.vecPosition[1] = tracking_ref_steamvr_space_translation.y();
                out_pose.vecPosition[2] = tracking_ref_steamvr_space_translation.z();
            }

            thiz->m_currentGizmoPose = out_pose;
            vr::VRServerDriverHost()->TrackedDevicePoseUpdated(thiz->m_deviceIndex, thiz->m_currentGizmoPose, sizeof(vr::DriverPose_t));
        };

        this->tryAddDevice(std::make_unique<DebugGizmo>(this->m_settingsManager, tracking_ref_gizmo_update_fn), "tracking_ref_gizmo", vr::ETrackedDeviceClass::TrackedDeviceClass_TrackingReference);
    
        // Setup update function for massless tracker pose gizmo (Copied straight out of PenController::makeOpenVRPose with a pen pose of (0,0,0)(1,0,0,0))
        auto massless_tracker_gizmo_update_fn = [](DebugGizmo* thiz) {
            vr::DriverPose_t out_pose = { 0 };
            if (auto ref_pose = ServerDriver::instance()->getTrackingReference(); ref_pose.has_value()) {
                // Set up some default values that should be in every pose
                out_pose.deviceIsConnected = true;
                out_pose.poseIsValid = true;
                out_pose.result = vr::ETrackingResult::TrackingResult_Running_OK;
                out_pose.willDriftInYaw = false;
                out_pose.shouldApplyHeadModel = false;
                out_pose.qDriverFromHeadRotation.w = out_pose.qWorldFromDriverRotation.w = out_pose.qRotation.w = 1.0;

                /*
                    Things to note here:
                    1. The massless tracking space is defined as (looking toward the trackers) X left, Y down, Z toward you, Steamvr is defined as
                       X right, Y up, Z toward you. When looking toward a lighthouse (and perhaps a rift camera also), X is left, Y is up, and z is away from you.
                       (This is probably because the lighthouse model was made with z- pointing out the front of the glass, and it being spun to face you).
                       To make these coordinate systems workable, we need to flip the massless coordinate system around the x axis by 180 degrees, so that now the massless
                       tracker orientation is oriented with X left, Y up, and Z facing away from us (same orientation as the lighthouse)
                    2. The pen is defined as X out of the tip, Z up, Y right (looking down the tip), steamvr defines its controllers orientations
                       as Z- out of the tip, Y up, and X to the left (looking down the controller).
                    3. The Massless API reports the offset (translation, and rotation to the described system above) from the tracking reference, to the Massless tracker's coord system.
                       This is reported as a transformation in the Massless Tracker's frame of reference, to the origin point of the reference system. We then need to rotate this to
                       match the orientation of the reference system.
                    To account for these, we need to apply the transformations as shown below
                */
                constexpr float pi = 3.1415926f;

                // Tracking reference pose in steamvr space
                Eigen::Quaternionf tracking_ref_steamvr_space_rotation = ref_pose->global_pose.first;
                const Eigen::Vector3f& tracking_ref_steamvr_space_translation = ref_pose->global_pose.second;

                // Massless tracker pose (offset) from tracking reference.
                const MasslessInterface::Pose& tracker_pose = ref_pose->pose_offset;

                // Rotation #1
                Eigen::Vector3f tracker_local_x_axis = tracking_ref_steamvr_space_rotation * Eigen::Vector3f(1, 0, 0);
                Eigen::Quaternionf tracker_coordinate_to_steamvr_coordinate;
                if (ref_pose->type.getSystemType() == MasslessInterface::TrackingSystemType::SystemType::VIVE_TRACKER) {
                    tracker_coordinate_to_steamvr_coordinate = Eigen::AngleAxisf(pi / 2.0f, tracker_local_x_axis);
                }
                else {
                    tracker_coordinate_to_steamvr_coordinate = Eigen::AngleAxisf(pi, tracker_local_x_axis);
                }
                //

                Eigen::Quaternionf tracker_ms_space_rotation = Eigen::Quaternion(tracker_pose.m_qr, tracker_pose.m_qx, tracker_pose.m_qy, tracker_pose.m_qz);
                Eigen::Vector3f tracker_ms_space_translation = Eigen::Vector3f(tracker_pose.m_x, tracker_pose.m_y, tracker_pose.m_z);

                //Eigen::Vector3f tracker_steamvr_space_translation = tracking_ref_steamvr_space_rotation * tracker_ms_space_translation;
                //Eigen::Quaternionf tracker_steamvr_space_rotation = tracking_ref_steamvr_space_rotation * tracker_ms_space_rotation;

                // Rotate the massless pen into the tracker's frame in the massless coordinate system, also translate it to the tracking reference's system.
                Eigen::Vector3f pen_ms_space_translation = tracker_ms_space_rotation * Eigen::Vector3f(0, 0, 0) + tracker_ms_space_translation;
                Eigen::Quaternionf pen_ms_space_rotation = tracker_ms_space_rotation * Eigen::Quaternionf(1, 0, 0, 0);

                Eigen::Vector3f pen_steamvr_space_translation = tracking_ref_steamvr_space_rotation * pen_ms_space_translation;
                Eigen::Quaternionf pen_steamvr_space_rotation = tracking_ref_steamvr_space_rotation * pen_ms_space_rotation;

                // Rotation #2
                Eigen::Vector3f local_x_axis = pen_steamvr_space_rotation * Eigen::Vector3f(1, 0, 0);
                Eigen::Vector3f local_z_axis = pen_steamvr_space_rotation * Eigen::Vector3f(0, 0, 1);
                pen_steamvr_space_rotation = tracker_coordinate_to_steamvr_coordinate * Eigen::AngleAxisf(-pi / 2, local_z_axis) * Eigen::AngleAxisf(pi / 2, local_x_axis) * pen_steamvr_space_rotation;
                pen_steamvr_space_translation = tracker_coordinate_to_steamvr_coordinate * pen_steamvr_space_translation;
                //
                pen_steamvr_space_translation += tracking_ref_steamvr_space_translation;

                out_pose.qRotation.w = pen_steamvr_space_rotation.w();
                out_pose.qRotation.x = pen_steamvr_space_rotation.x();
                out_pose.qRotation.y = pen_steamvr_space_rotation.y();
                out_pose.qRotation.z = pen_steamvr_space_rotation.z();

                out_pose.vecPosition[0] = pen_steamvr_space_translation.x();
                out_pose.vecPosition[1] = pen_steamvr_space_translation.y();
                out_pose.vecPosition[2] = pen_steamvr_space_translation.z();

            }

            thiz->m_currentGizmoPose = out_pose;
            vr::VRServerDriverHost()->TrackedDevicePoseUpdated(thiz->m_deviceIndex, thiz->m_currentGizmoPose, sizeof(vr::DriverPose_t));
        };
    
        this->tryAddDevice(std::make_unique<DebugGizmo>(this->m_settingsManager, massless_tracker_gizmo_update_fn), "massless_tracker_gizmo", vr::ETrackedDeviceClass::TrackedDeviceClass_TrackingReference);

    
    
    }

    // Check if we have either the forced_tracking_reference or the auto_tracking_reference set
    if (this->m_settingsManager->getSettings().getValue<std::string>(DriverSettings::ForcedTrackingRefSerial).has_value()) {
        this->m_trackingReferenceSerialHint = this->m_settingsManager->getSettings().getValue<std::string>(DriverSettings::ForcedTrackingRefSerial).value();
        this->m_doLookForever = true;
        this->m_doRetryOnConnect = false;
        this->m_doRetryOnDisconnect = false;
        DriverLog("[Info] Force attaching to tracking reference with serial [%s]\n", (*this->m_trackingReferenceSerialHint).c_str());
    }else if (this->m_settingsManager->getSettings().getValue<std::string>(DriverSettings::AutoTrackingRefSerial).has_value()) {
        this->m_trackingReferenceSerialHint = this->m_settingsManager->getSettings().getValue<std::string>(DriverSettings::AutoTrackingRefSerial).value();
        this->m_doLookForever = false;
        this->m_doRetryOnConnect = true;
        this->m_doRetryOnDisconnect = true;
        DriverLog("[Info] Auto attaching to tracking reference with serial [%s]\n", (*this->m_trackingReferenceSerialHint).c_str());
    }

    DriverLog("[Info] Success!\n");
    this->m_initSuccess = true;
	return EVRInitError::VRInitError_None;
}

void ServerDriver::Cleanup()
{
}

const char* const* ServerDriver::GetInterfaceVersions()
{
	return vr::k_InterfaceVersions;
}

void ServerDriver::RunFrame()
{
    if (!this->m_initSuccess)
        return;

    if (m_TrackingReferenceSearchCountdown > 0)--m_TrackingReferenceSearchCountdown;

    auto events = this->processEvents(vr::VRServerDriverHost(), vr::VRProperties());

    // Get exclusive access to the pen system for this frame
    auto pen_system_lock = this->m_masslessManager->getPenSystem();
    if (pen_system_lock.pen_system.has_value()) {

        auto pen_system = pen_system_lock.pen_system.value();

        // Attempt to do first time setup of backend
        static bool hasSetupBackend = false;
        if (!hasSetupBackend) {

            auto driver_version = DriverVersion::m_driverVersion.to_string();
            DriverLog("[Info] Massless Pen SteamVR Driver version v%s\n", driver_version.c_str());
            DriverLog("[Info] Massless Pen DLL version v%s\n", pen_system->getDllVersionString().c_str());
            if (DriverVersion::m_dllVersion != DriverVersion::asSemVer<uint64_t>(pen_system->getDllVersion())) {
                DriverLog("[Warn] Massless dll version this driver was built against (%s) is different to the currently linked dll version (%s)\n", DriverVersion::m_dllVersion.to_string().c_str(), pen_system->getDllVersionString().c_str());
            }

            pen_system->setIntegrationKey(SettingsUtilities::getDefaultIntegrationKey());
            auto start_result = pen_system->startSystem();
            if (start_result.has_value()) {
                DriverLog("[Warn] Massless Pen System failed to start with error [0x%X], attempting to force restart.\n", *start_result);
                auto restart_result = pen_system->stopSystem(true);
                if (restart_result.has_value()) {
                    DriverLog("[Error] Massless Pen System failed to stop with error [0x%X].\n", *restart_result);
                    this->m_initSuccess = false;
                }
                else {
                    start_result = pen_system->startSystem(true);
                    if (start_result.has_value()) {
                        DriverLog("[Error] Massless Pen System failed to start with error [0x%X].\n", *start_result);
                        this->m_initSuccess = false;
                    }
                    else {
                        this->m_initSuccess = true;
                    }
                }
            }
            else {
                this->m_initSuccess = true;
            }

            if (this->m_initSuccess) {
                pen_system->setUnitScale(0.001);
                DriverLog("[Info] Massless Pen System backend started successfully.\n");
                hasSetupBackend = true;
            }
        }

        if (hasSetupBackend) {

            // If a new device is connected that is a higher priority tracking reference, clear the old one
            //At the end of the count actually do the search (0 means don't count).
            if (m_TrackingReferenceSearchCountdown == 1) {
                DriverLog("Tracking reference search countdown finished, actually searching the list.\n");
                auto indices = DriverAnalytics::firstPass(vr::VRServerDriverHost(), vr::VRProperties());
                if (indices.size() > 0) {
                    // New tracking reference that is different to our old one
                    if ((this->m_trackingReferencePack.has_value() && this->m_trackingReferencePack->index != indices.at(0)) || !this->m_trackingReferencePack.has_value()) {
                        this->setTrackingReference(std::nullopt);
                    }
                }
            }
                
            for (const auto& event : events) {

                // If the application was changed
                if (event.eventType == vr::EVREventType::VREvent_SceneApplicationChanged) {
                    std::wstring processName = DriverAnalytics::getVRClientProcessName(event.data.process.pid).value_or(L"Unknown Process");

                    std::stringstream s;
                    const std::ctype<char>& ctfacet = std::use_facet<std::ctype<char>>(s.getloc());
                    for (size_t i = 0; i < processName.size(); ++i)
                        s << ctfacet.narrow(processName[i], 0);

                    DriverLog("[Application Changed]: (%d) %s\n", event.data.process.pid, s.str().c_str());
                    auto integrationKey = SettingsUtilities::getIntegrationKeyForApp(s.str()).value_or(SettingsUtilities::getDefaultIntegrationKey());

                    std::stringstream key_str;
                    for (auto v : integrationKey)
                        key_str << "0x" << std::uppercase << std::setfill('0') << std::setw(2) << std::hex << static_cast<int>(v) << " ";
                    
                    if (auto err = pen_system->setIntegrationKey(integrationKey); err.has_value()) {
                        DriverLog("Failed to change integration key with error [0x%X]\n", err.value());
                    }
                    else {
                        DriverLog("Integration key changed: %s\n", key_str.str().c_str());
                    }
                }

                //Start a count when we find the event
                if (event.eventType == vr::EVREventType::VREvent_TrackedDeviceActivated
                    || event.eventType == vr::EVREventType::VREvent_Input_TrackerActivated) {
                    if (!this->m_doLookForever) {
                        //Only restart the count, if we're not already counting
                        //Start out with a 30 frame delay
                        if (m_TrackingReferenceSearchCountdown == 0) {
                            m_TrackingReferenceSearchCountdown = 300;
                            DriverLog("Started tracking reference search countdown.\n");
                        }
                    }
                }

                // If a tracking reference was disconnected and we aren't quitting, clear the old one
                if (event.eventType == vr::EVREventType::VREvent_TrackedDeviceDeactivated) {
                    if (!this->m_doLookForever && this->m_trackingReferencePack.has_value()) {
                        auto pose = DriverAnalytics::getDevicePose(this->m_trackingReferencePack->index, vr::VRServerDriverHost());
                        if (!pose.has_value()) {
                            this->m_didReferenceDisconnect = true;
                            this->m_timeReferenceDisconnect = std::chrono::system_clock::now();
                        }
                    }
                }
            }
        }

        // Wait until the pen is connected to poll for serial
        if (pen_system->isPenConnected() && !this->m_hasAddedPen) {
            // Try and get pen serial
            auto serial = pen_system->getPenSerial();

            // Initialise the pen controller
            if (serial.has_value()) {
                this->m_hasAddedPen = this->tryAddDevice(std::make_unique<PenController>(this->m_settingsManager, this->m_masslessManager), "massless_pen_" + *serial, vr::ETrackedDeviceClass::TrackedDeviceClass_Controller);
            }
        }

        if (this->m_hasAddedPen) {
            // Either find tracking reference, or update if vive tracker
            this->updateTrackingReference(vr::VRServerDriverHost(), vr::VRProperties());
        }
    }

    for (auto&& device : this->m_devices)
        device.get()->update(events);
}

bool ServerDriver::ShouldBlockStandbyMode()
{
	return false;
}

void ServerDriver::EnterStandby()
{
}

void ServerDriver::LeaveStandby()
{
}

bool ServerDriver::tryAddDevice(std::unique_ptr<IDriverDevice> device, std::string serial, vr::ETrackedDeviceClass device_class, vr::IVRServerDriverHost* serverdriver_host)
{
    if (!serverdriver_host->TrackedDeviceAdded(serial.c_str(), device_class, device.get())) {
        return false;
    }
    this->m_devices.push_back(std::move(device));
    return true;
}

std::shared_ptr<SettingsManager> ServerDriver::getSettingsManager()
{
    return m_settingsManager;
}

std::optional<DriverAnalytics::TrackingReferencePack> ServerDriver::getTrackingReference()
{
    return this->m_trackingReferencePack;
}

ServerDriver::ServerDriver(std::shared_ptr<MasslessInterface::IPenSystem> pen_system)
{
    this->m_masslessManager = std::make_shared<MasslessManager>(pen_system);
}

const std::shared_ptr<ServerDriver> ServerDriver::create(std::shared_ptr<MasslessInterface::IPenSystem> pen_system)
{
    m_driverInstance = std::make_shared<ServerDriver>(pen_system);
    return m_driverInstance;
}


void ServerDriver::updateTrackingReference(vr::IVRServerDriverHost* serverdriver_host, vr::CVRPropertyHelpers* properties)
{
    if (auto pen_system_lock = this->m_masslessManager->getPenSystem(); pen_system_lock.pen_system.has_value()) {
        auto pen_system = pen_system_lock.pen_system.value();

        // If the tracking reference has been disconnected for too long, reset it
        if (this->m_didReferenceDisconnect && this->m_timeReferenceDisconnect + this->m_searchTimeout < std::chrono::system_clock::now()) {
            this->m_didReferenceDisconnect = false;
            if (!this->m_doLookForever) {
                this->setTrackingReference(std::nullopt);
            }
        }

        // If we have already previously got a tracking reference pack
        if (this->m_trackingReferencePack.has_value()) {
            auto pose = DriverAnalytics::getDevicePose(this->m_trackingReferencePack->index, serverdriver_host);
            if (pose.has_value() && !((this->m_trackingReferencePack->type.getSystemType() == MasslessInterface::TrackingSystemType::SystemType::OCULUS_RIFT_S_TOUCH_RIGHT
                || this->m_trackingReferencePack->type.getSystemType() == MasslessInterface::TrackingSystemType::SystemType::OCULUS_RIFT_S_TOUCH_LEFT)
                && pose.value().second.isApprox(Eigen::Vector3f(0, 0, 0)))) {
                this->m_trackingReferencePack->global_pose = *pose;
                if (this->m_settingsManager->getSettings().getValue<bool>(DriverSettings::EnableDetailedLogging).value_or(false))
                    DriverLog("[Info] Tracking reference pose updated.\n");
            }
        }
        else {
            // Do we already know what we are looking for?
            if (this->m_trackingReferenceSerialHint.has_value()) {

                // Do we need to ignore our old suggestion and look for a new one?
                if (!this->m_doLookForever && (this->m_timeDriverStart + this->m_searchTimeout) < std::chrono::system_clock::now()) {
                    this->m_trackingReferenceSerialHint = std::nullopt;
                    return;
                }

                auto tracking_reference_pack = DriverAnalytics::secondPass(*this->m_trackingReferenceSerialHint, serverdriver_host, properties, pen_system);
                if (tracking_reference_pack.has_value()) {
                    DriverLog("[Info] Found forced tracking reference pose for device with serial [%s].\n", this->m_trackingReferenceSerialHint->c_str());
                    this->setTrackingReference(tracking_reference_pack);
                }
            }
            else {
                // Else we dont know what we are looking for
                auto indices = DriverAnalytics::firstPass(serverdriver_host, properties);
                for (auto index : indices) {
                    // Check if device is a rift S controller
                    auto system_type = DriverAnalytics::getReferenceSystemType(index, properties);
                    if (system_type.getSystemType() == MasslessInterface::TrackingSystemType::SystemType::OCULUS_RIFT_S_TOUCH_RIGHT
                        || system_type.getSystemType() == MasslessInterface::TrackingSystemType::SystemType::OCULUS_RIFT_S_TOUCH_LEFT) {

                        // Check if our pen is left or right handed
                        auto device_props = properties->TrackedDeviceToPropertyContainer(index);
                        auto pen_handedness = Handedness(this->getSettingsManager()->getSettings().getValue<std::string>(DriverSettings::Handedness).value_or("invalid"));

                        if (pen_handedness.value() == Handedness::LEFT) {
                            // We need the left controller
                            vr::ETrackedPropertyError err = vr::ETrackedPropertyError::TrackedProp_Success;
                            auto controller_handedness = properties->GetInt32Property(device_props, vr::ETrackedDeviceProperty::Prop_ControllerRoleHint_Int32, &err);
                            if (err != vr::ETrackedPropertyError::TrackedProp_Success)
                                continue;
                            if (controller_handedness == vr::ETrackedControllerRole::TrackedControllerRole_LeftHand) {
                                auto tracking_reference_pack = DriverAnalytics::secondPass(index, serverdriver_host, properties, pen_system);
                                if (tracking_reference_pack.has_value()) {
                                    DriverLog("[Info] Found auto tracking reference of type %s.\n", tracking_reference_pack->type.getStringValue().c_str());
                                    DriverLog("[Info] Found auto tracking reference pose for left Rift S controller with serial [%s].\n", tracking_reference_pack->device_serial.c_str());
                                    this->setTrackingReference(tracking_reference_pack);
                                    this->m_trackingReferenceSerialHint = tracking_reference_pack->device_serial;
                                    break;
                                }
                            }
                        }
                        else if (pen_handedness.value() == Handedness::RIGHT) {
                            // We need the right controller
                            vr::ETrackedPropertyError err = vr::ETrackedPropertyError::TrackedProp_Success;
                            auto controller_handedness = properties->GetInt32Property(device_props, vr::ETrackedDeviceProperty::Prop_ControllerRoleHint_Int32, &err);
                            if (err != vr::ETrackedPropertyError::TrackedProp_Success)
                                continue;
                            if (controller_handedness == vr::ETrackedControllerRole::TrackedControllerRole_RightHand) {
                                auto tracking_reference_pack = DriverAnalytics::secondPass(index, serverdriver_host, properties, pen_system);
                                if (tracking_reference_pack.has_value()) {
                                    DriverLog("[Info] Found auto tracking reference of type %s.\n", tracking_reference_pack->type.getStringValue().c_str());
                                    DriverLog("[Info] Found auto tracking reference pose for right Rift S controller with serial [%s].\n", tracking_reference_pack->device_serial.c_str());
                                    this->setTrackingReference(tracking_reference_pack);
                                    this->m_trackingReferenceSerialHint = tracking_reference_pack->device_serial;
                                    break;
                                }
                            }
                        }

                        // Otherwise we dont know the handedness of this controller, cant do anything
                    }
                    else { // otherwise do regular tracking reference logic
                        auto tracking_reference_pack = DriverAnalytics::secondPass(index, serverdriver_host, properties, pen_system);
                        if (tracking_reference_pack.has_value()) {
                            DriverLog("[Info] Found auto tracking reference of type %s.\n", tracking_reference_pack->type.getStringValue().c_str());
                            DriverLog("[Info] Found auto tracking reference pose for device with serial [%s].\n", tracking_reference_pack->device_serial.c_str());
                            this->setTrackingReference(tracking_reference_pack);
                            this->m_trackingReferenceSerialHint = tracking_reference_pack->device_serial;
                            break;
                        }
                    }
                }
            }
        }
    }

}

void ServerDriver::setTrackingReference(std::optional<DriverAnalytics::TrackingReferencePack> new_tracking_reference_pack)
{
    if (new_tracking_reference_pack.has_value()) {
        this->m_settingsManager->getSettings().setValue<std::string>(DriverSettings::AutoTrackingRefSerial, new_tracking_reference_pack->device_serial);
    }
    else {
        this->m_settingsManager->getSettings().clearValue(DriverSettings::AutoTrackingRefSerial);
    }
    this->m_trackingReferencePack = new_tracking_reference_pack;
    this->m_settingsManager->storeSettings();
}

std::vector<vr::VREvent_t> ServerDriver::processEvents(vr::IVRServerDriverHost* serverdriver_host, vr::CVRPropertyHelpers* properties)
{
    // Process events
    vr::VREvent_t event;
    std::vector<vr::VREvent_t> events;
    while (serverdriver_host->PollNextEvent(&event, sizeof(event)))
    {
        events.push_back(event);
    }

    return events;
}
