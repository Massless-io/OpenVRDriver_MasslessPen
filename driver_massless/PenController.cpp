/* Copyright (C) 2020 Massless Corp. - All Rights Reserved
 * Author: Jacob Hilton.
 * You may use, distribute and modify this code under the
 * terms of the BSD 3-Clause "New" or "Revised" License.
 *
 * You should have received a copy of this license with
 * this file. If not, please email support@massless.io
 */

#include <PenController.hpp>
#include <DriverLog.hpp>
#include <iostream>

PenController::PenController(std::shared_ptr<SettingsManager> settings_manager, std::shared_ptr<MasslessManager> massless_manager):
    m_settingsManager(settings_manager),
    m_masslessManager(massless_manager),
    m_currentPenPose(this->makeNotTrackingOpenVRPose())
{
    using namespace Massless::Events;
    // Swipe "tap" event
    m_gestureHandlers.push_back({
        {
            EventType::TouchPadPressed,
            EventType::TouchPadMultiTapNew,
            EventType::TouchPadReleased,
            EventType::TouchPadSwipe,
            EventType::TouchPadMultiTapTotal
        },
        [&](std::vector<MasslessInterface::PenEvent> event_stack, vr::IVRDriverInput* driver_input, std::shared_ptr<MasslessManager> massless_manager) {

            auto pen_system = massless_manager->getPenSystem();
            float swipe_velocity = std::static_pointer_cast<Massless::Events::TouchPadSwipeEvent>(event_stack.at(3).m_eventStruct.value())->Velocity;
            if (swipe_velocity > 0) {
                DriverLog("Firing swipe forwards event\n");
                driver_input->UpdateBooleanComponent(this->m_compSwipeForwards, true, 0);
                /*if (pen_system.pen_system.has_value())
                    pen_system.pen_system.value()->sendVibration(this->m_tapVibrationDuration);*/
            }
            else {
                DriverLog("Firing swipe backwards event\n");
                driver_input->UpdateBooleanComponent(this->m_compSwipeBackwards, true, 0);
                /*if (pen_system.pen_system.has_value())
                    pen_system.pen_system.value()->sendVibration(this->m_tapVibrationDuration);*/
            }
            this->m_unpressAllFrameTimeout = this->m_unpressAllMaxTimeout;
            return true;
        }
        });

    // Double held down event
    m_gestureHandlers.push_back({
        {
            EventType::TouchPadPressed,
            EventType::TouchPadMultiTapNew,
            EventType::TouchPadReleased,
            EventType::TouchPadPressed,
            EventType::TouchPadMultiTapNew,
            EventType::TouchPadHeld,
            EventType::TouchPadMultiTapTotal
        },
        [&, this](std::vector<MasslessInterface::PenEvent> event_stack, vr::IVRDriverInput* driver_input, std::shared_ptr<MasslessManager> massless_manager) {
            uint8_t tap_position_1 = std::static_pointer_cast<Massless::Events::TouchPadPressedEvent>(event_stack.at(0).m_eventStruct.value())->PositionPressed;
            uint8_t tap_position_2 = std::static_pointer_cast<Massless::Events::TouchPadPressedEvent>(event_stack.at(3).m_eventStruct.value())->PositionPressed;
            auto pen_system = massless_manager->getPenSystem();
            if (tap_position_1 < m_touchpadSplitLocation && tap_position_2 < m_touchpadSplitLocation) {
                DriverLog("Firing front double held down event\n");
                driver_input->UpdateBooleanComponent(this->m_compDoubleFront, true, 0);
                /*if (pen_system.pen_system.has_value())
                    pen_system.pen_system.value()->sendVibration(this->m_tapVibrationDuration);*/
                this->m_unpressAllFrameTimeout = this->m_unpressAllStopTimeout;
                return false;
            }
            else if (tap_position_1 > m_touchpadSplitLocation && tap_position_2 > m_touchpadSplitLocation) {
                DriverLog("Firing rear double held down event\n");
                driver_input->UpdateBooleanComponent(this->m_compDoubleRear, true, 0);
                /*if (pen_system.pen_system.has_value())
                    pen_system.pen_system.value()->sendVibration(this->m_tapVibrationDuration);*/
                this->m_unpressAllFrameTimeout = this->m_unpressAllStopTimeout;
                return false;
            }
            return false; // Tapped on front and back, map to nothing
        }
        });

    // Double held up event
    m_gestureHandlers.push_back({
        {
            EventType::TouchPadPressed,
            EventType::TouchPadMultiTapNew,
            EventType::TouchPadReleased,
            EventType::TouchPadPressed,
            EventType::TouchPadMultiTapNew,
            EventType::TouchPadHeld,
            EventType::TouchPadMultiTapTotal,
            EventType::TouchPadReleased
        },
        [&, this](std::vector<MasslessInterface::PenEvent> event_stack, vr::IVRDriverInput* driver_input, std::shared_ptr<MasslessManager> massless_manager) {
            DriverLog("Firing double held up event\n");
            //TODO: If we add the haptics on down and up, doesn't this need an up vibration?
            this->m_unpressAllFrameTimeout = this->m_unpressAllMaxTimeout;
            return true;
        }
        });

    // Single held down event
    m_gestureHandlers.push_back({
        {
            EventType::TouchPadPressed,
            EventType::TouchPadMultiTapNew,
            EventType::TouchPadHeld,
            EventType::TouchPadMultiTapTotal
        },
        [&, this](std::vector<MasslessInterface::PenEvent> event_stack, vr::IVRDriverInput* driver_input, std::shared_ptr<MasslessManager> massless_manager) {
            auto pen_system = massless_manager->getPenSystem();
            uint8_t tap_position = std::static_pointer_cast<Massless::Events::TouchPadPressedEvent>(event_stack.at(0).m_eventStruct.value())->PositionPressed;
            if (tap_position < m_touchpadSplitLocation) {
                DriverLog("Firing front single held down event\n");
                driver_input->UpdateBooleanComponent(this->m_compSingleFront, true, 0); 
                /*if (pen_system.pen_system.has_value())
                    pen_system.pen_system.value()->sendVibration(this->m_tapVibrationDuration);*/
            }
            else {
                DriverLog("Firing rear single held down event\n");
                driver_input->UpdateBooleanComponent(this->m_compSingleRear, true, 0);
                /*if (pen_system.pen_system.has_value())
                    pen_system.pen_system.value()->sendVibration(this->m_tapVibrationDuration);*/
            }
            this->m_unpressAllFrameTimeout = this->m_unpressAllStopTimeout;
            return false;
        }
        });

    // Single held up event
    m_gestureHandlers.push_back({
        {
            EventType::TouchPadPressed,
            EventType::TouchPadMultiTapNew,
            EventType::TouchPadHeld,
            EventType::TouchPadMultiTapTotal,
            EventType::TouchPadReleased
        },
        [&, this](std::vector<MasslessInterface::PenEvent> event_stack, vr::IVRDriverInput* driver_input, std::shared_ptr<MasslessManager> massless_manager) {
            DriverLog("Firing single held up event\n");
            this->m_unpressAllFrameTimeout = this->m_unpressAllMaxTimeout;
            return true;
        }
        });

    // Double tap events
    m_gestureHandlers.push_back({
        {
            EventType::TouchPadPressed,
            EventType::TouchPadMultiTapNew,
            EventType::TouchPadReleased,
            EventType::TouchPadPressed,
            EventType::TouchPadMultiTapNew,
            EventType::TouchPadReleased,
            EventType::TouchPadMultiTapTotal
        },
        [&, this](std::vector<MasslessInterface::PenEvent> event_stack, vr::IVRDriverInput* driver_input, std::shared_ptr<MasslessManager> massless_manager) {
            uint8_t tap_position_1 = std::static_pointer_cast<Massless::Events::TouchPadPressedEvent>(event_stack.at(0).m_eventStruct.value())->PositionPressed;
            uint8_t tap_position_2 = std::static_pointer_cast<Massless::Events::TouchPadPressedEvent>(event_stack.at(3).m_eventStruct.value())->PositionPressed;
            auto pen_system = massless_manager->getPenSystem();
            if (tap_position_1 < m_touchpadSplitLocation && tap_position_2 < m_touchpadSplitLocation) {
                DriverLog("Firing front double tap event\n");
                driver_input->UpdateBooleanComponent(this->m_compDoubleFront, true, 0);
                /*if (pen_system.pen_system.has_value())
                    pen_system.pen_system.value()->sendVibration(this->m_tapVibrationDuration);*/
                this->m_unpressAllFrameTimeout = this->m_unpressAllMaxTimeout;
                return true;
            }
            else if (tap_position_1 > m_touchpadSplitLocation && tap_position_2 > m_touchpadSplitLocation) {
                DriverLog("Firing rear double tap event\n");
                driver_input->UpdateBooleanComponent(this->m_compDoubleRear, true, 0);
                /*if (pen_system.pen_system.has_value())
                    pen_system.pen_system.value()->sendVibration(this->m_tapVibrationDuration);*/
                this->m_unpressAllFrameTimeout = this->m_unpressAllMaxTimeout;
                return true;
            }
            return false; // Tapped on front and back, not mapped to a button
        }
        });

    // Single tap events
    m_gestureHandlers.push_back({
        {
            EventType::TouchPadPressed,
            EventType::TouchPadMultiTapNew,
            EventType::TouchPadReleased,
            EventType::TouchPadMultiTapTotal
        },
        [&, this](std::vector<MasslessInterface::PenEvent> event_stack, vr::IVRDriverInput* driver_input, std::shared_ptr<MasslessManager> massless_manager) {
            auto pen_system = massless_manager->getPenSystem();
            uint8_t tap_position = std::static_pointer_cast<Massless::Events::TouchPadPressedEvent>(event_stack.at(0).m_eventStruct.value())->PositionPressed;
            if (tap_position < m_touchpadSplitLocation) {
                DriverLog("Firing front single tap event\n");
                driver_input->UpdateBooleanComponent(this->m_compSingleFront, true, 0);
                /*if (pen_system.pen_system.has_value())
                    pen_system.pen_system.value()->sendVibration(this->m_tapVibrationDuration);*/
            }
            else {
                DriverLog("Firing rear single tap event\n");
                driver_input->UpdateBooleanComponent(this->m_compSingleRear, true, 0);
                /*if (pen_system.pen_system.has_value())
                    pen_system.pen_system.value()->sendVibration(this->m_tapVibrationDuration);*/
            }
            this->m_unpressAllFrameTimeout = this->m_unpressAllMaxTimeout;
            return true;
        }
        });
}

void PenController::update(std::vector<vr::VREvent_t> events)
{
    if (this->m_deviceIndex != vr::k_unTrackedDeviceIndexInvalid)
    {
        // Ensure the pen is always highest priority
        vr::VRProperties()->SetInt32Property(this->m_propertiesHandle, vr::Prop_ControllerHandSelectionPriority_Int32, INT32_MAX);
        
        // Log notifications
        if (this->m_settingsManager->getSettings().getValue<bool>(DriverSettings::EnableDetailedLogging).value_or(false))
            this->logNotifications();

        // Process OpenVR events
        this->processOpenVREvents(events);

        // Process Massless events
        this->processMasslessEvents(vr::VRDriverInput());
        
        auto pen_system_lock = this->m_masslessManager->getPenSystem();
        if (pen_system_lock.pen_system.has_value()) {
            auto pen_system = pen_system_lock.pen_system.value();


            // System is not running, or the pen is disconnected
            if (!pen_system->isSystemRunning()) {
                this->m_currentPenPose = this->makeDisconnectedOpenVRPose();
            }
            // System is running but pen is not tracking, make not tracking pose
            else if (!pen_system->isPenTracking() || !pen_system->isPenConnected()) {
                this->m_currentPenPose = this->makeNotTrackingOpenVRPose();
            }
            // System is running and pen is connected, check if we have a tracking reference
            else if (auto ref_pose = ServerDriver::instance()->getTrackingReference(); ref_pose.has_value())
            {
                MasslessInterface::Pose pen_pose = pen_system->getCurrentPose();
                this->m_currentPenPose = this->makeOpenVRPose(pen_pose, ref_pose->pose_offset, ref_pose->global_pose, ref_pose->type);
            }
            // Otherwise we are just waiting to find a tracking reference pose (ie. calibrating in-progress)
            else {
                this->m_currentPenPose = this->makeCalibratingOpenVRPose();
            }
        }
        // Otherwise we cannot access backend, so we are disconnected
        else {
            this->m_currentPenPose = this->makeDisconnectedOpenVRPose();
        }
        this->updatePenPose(this->m_currentPenPose);
    }
}


void PenController::updatePenPose(const vr::DriverPose_t new_pose, vr::IVRServerDriverHost* serverdriver_host)
{
    serverdriver_host->TrackedDevicePoseUpdated(this->m_deviceIndex, new_pose, sizeof(vr::DriverPose_t));
}

void PenController::logNotifications() {
    auto pen_system_lock = this->m_masslessManager->getPenSystem();
    if (!pen_system_lock.pen_system.has_value())
        return;
    auto pen_system = pen_system_lock.pen_system.value();

    // Show receieved notifications
    while (true) {
        if (auto notification = pen_system->popNotification(); notification != std::nullopt) {
            std::string notificationType;
            switch (notification->m_notificationType) {
            case MasslessInterface::PenNotification::WARNING:
                notificationType = "WARNING";
                break;
#undef ERROR
            case MasslessInterface::PenNotification::ERROR:
                notificationType = "ERROR";
                break;
            case MasslessInterface::PenNotification::MESSAGE:
                notificationType = "MESSAGE";
                break;
            }
            if (notification->m_notificationCode.has_value()) {
                DriverLog("[Notification] [%s] %x \"%s\"\n", notificationType.c_str(), notification->m_notificationCode.value(), notification->m_notificationMessage.c_str());
            }
            else {
                DriverLog("[Notification] [%s] \"%s\"\n", notificationType.c_str(), notification->m_notificationMessage.c_str());
            }
        }
        else {
            break;
        }
    }
}

void PenController::processMasslessEvents(vr::IVRDriverInput* driver_input)
{
    auto pen_system_lock = this->m_masslessManager->getPenSystem();
    if (!pen_system_lock.pen_system.has_value())
        return;
    auto pen_system = pen_system_lock.pen_system.value();

    if (this->m_unpressAllFrameTimeout == 0) {
        this->m_unpressAllFrameTimeout = this->m_unpressAllStopTimeout;
        driver_input->UpdateBooleanComponent(this->m_compSingleFront, false, 0);
        driver_input->UpdateBooleanComponent(this->m_compDoubleFront, false, 0);
        driver_input->UpdateBooleanComponent(this->m_compSingleRear, false, 0);
        driver_input->UpdateBooleanComponent(this->m_compDoubleRear, false, 0);
        driver_input->UpdateBooleanComponent(this->m_compSwipeForwards, false, 0);
        driver_input->UpdateBooleanComponent(this->m_compSwipeBackwards, false, 0);
        //pen_system->sendVibration(this->m_tapVibrationDuration);
    }
    else if (this->m_unpressAllFrameTimeout > 0) {
        this->m_unpressAllFrameTimeout--;
    }

    // Update fast inputs
    auto penState = pen_system->getCurrentState();
    bool fastFrontState = false;
    bool fastRearState = false;
    if (penState.m_capsenseValue != penState.CAPSENSE_NOT_TOUCHED) {
        if (penState.m_capsenseValue < m_touchpadSplitLocation) {
            fastFrontState = true;
        }
        else {
            fastRearState = true;
        }
    }
    driver_input->UpdateBooleanComponent(this->m_compFastFront, fastFrontState, 0);
    driver_input->UpdateBooleanComponent(this->m_compFastRear, fastRearState, 0);

    while (true) {
        if (auto event = pen_system->popEvent(); event != std::nullopt) {
            switch (event->m_eventType) {
                case Massless::Events::EventType::PenBattery:
                {
                    const Massless::Events::PenBatteryEvent* battery_event = static_cast<Massless::Events::PenBatteryEvent*>(event->m_eventStruct->get());
                    if (this->m_settingsManager->getSettings().getValue<bool>(DriverSettings::EnableDetailedLogging).value_or(false)) {
                        if (battery_event->Charging) {
                            DriverLog("[Event] Pen battery is charging\n");
                            vr::VRProperties()->SetBoolProperty(this->m_propertiesHandle, vr::Prop_DeviceIsCharging_Bool, true);
                        }
                        else {
                            DriverLog("[Event] Pen battery is not charging\n");
                            if (battery_event->Low) {
                                DriverLog("[Event] Pen battery is low\n");
                                vr::VRProperties()->SetFloatProperty(this->m_propertiesHandle, vr::Prop_DeviceBatteryPercentage_Float, 0.1f);
                            }
                            else if (battery_event->Critical) {
                                DriverLog("[Event] Pen battery is critical\n");
                                vr::VRProperties()->SetFloatProperty(this->m_propertiesHandle, vr::Prop_DeviceBatteryPercentage_Float, 0.05f);
                            }
                            else {
                                vr::VRProperties()->SetFloatProperty(this->m_propertiesHandle, vr::Prop_DeviceBatteryPercentage_Float, 1.0f);
                            }
                        }


                    }
                    else if (battery_event->Critical) {
                        if (this->m_settingsManager->getSettings().getValue<bool>(DriverSettings::EnableDetailedLogging).value_or(false))
                            DriverLog("[Event] Pen battery is critical\n");
                        vr::VRProperties()->SetFloatProperty(this->m_propertiesHandle, vr::Prop_DeviceBatteryPercentage_Float, 0.05f);
                    }
                    else {
                        vr::VRProperties()->SetFloatProperty(this->m_propertiesHandle, vr::Prop_DeviceBatteryPercentage_Float, 1.0f);
                    }
                } break;

                case Massless::Events::EventType::Error:
                {
                    const Massless::Events::ErrorEvent* error_event = static_cast<Massless::Events::ErrorEvent*>(event->m_eventStruct->get());
                    if (this->m_settingsManager->getSettings().getValue<bool>(DriverSettings::EnableDetailedLogging).value_or(false))
                        DriverLog("[Event] [Error] Error code received %u\n", error_event->ErrorNumber);
                } break;


                case Massless::Events::EventType::TouchPadHeld:
                case Massless::Events::EventType::TouchPadMultiTapNew:
                case Massless::Events::EventType::TouchPadMultiTapTotal:
                case Massless::Events::EventType::TouchPadPressed:
                case Massless::Events::EventType::TouchPadReleased:
                case Massless::Events::EventType::TouchPadSwipe:
                {
                    this->onGestureEvent(event.value(), driver_input);
                } break;

            }
        }
        else {
            break;
        }
    }
}

void PenController::processOpenVREvents(std::vector<vr::VREvent_t> events) {

    auto pen_system_lock = this->m_masslessManager->getPenSystem();
    if (!pen_system_lock.pen_system.has_value())
        return;
    auto pen_system = pen_system_lock.pen_system.value();

    for(auto event : events)
    {
        // Process haptics
        if (event.eventType == vr::VREvent_Input_HapticVibration) {
            if (event.data.hapticVibration.componentHandle == this->m_compHaptic) {
                pen_system->sendVibration(static_cast<uint16_t>(event.data.hapticVibration.fDurationSeconds * 1000.0f));
                break;
            }
        }
    }
}

vr::EVRInitError PenController::Activate(vr::TrackedDeviceIndex_t index)
{
    return this->Activate(index, vr::VRDriverInput(), vr::VRProperties());
}

vr::EVRInitError PenController::Activate(vr::TrackedDeviceIndex_t index, vr::IVRDriverInput* driver_input, vr::CVRPropertyHelpers* driver_properties)
{
    this->m_deviceIndex = index;

    this->m_propertiesHandle = driver_properties->TrackedDeviceToPropertyContainer(this->m_deviceIndex);
    driver_properties->SetUint64Property(this->m_propertiesHandle, vr::Prop_CurrentUniverseId_Uint64, 2);
    
    driver_input->CreateHapticComponent(this->m_propertiesHandle, "/output/haptic", &this->m_compHaptic);

    //
    driver_input->CreateBooleanComponent(this->m_propertiesHandle, "/input/front/single/click", &this->m_compSingleFront);
    driver_input->CreateBooleanComponent(this->m_propertiesHandle, "/input/front/double/click", &this->m_compDoubleFront);

    driver_input->CreateBooleanComponent(this->m_propertiesHandle, "/input/rear/single/click", &this->m_compSingleRear);
    driver_input->CreateBooleanComponent(this->m_propertiesHandle, "/input/rear/double/click", &this->m_compDoubleRear);

    driver_input->CreateBooleanComponent(this->m_propertiesHandle, "/input/swipe/forwards/click", &this->m_compSwipeForwards);
    driver_input->CreateBooleanComponent(this->m_propertiesHandle, "/input/swipe/backwards/click", &this->m_compSwipeBackwards);

    driver_input->CreateBooleanComponent(this->m_propertiesHandle, "/input/front/fast/click", &this->m_compFastFront);
    driver_input->CreateBooleanComponent(this->m_propertiesHandle, "/input/rear/fast/click", &this->m_compFastRear);
    //

    driver_properties->SetStringProperty(this->m_propertiesHandle, vr::Prop_ModelNumber_String, "Massless Pen");
    driver_properties->SetStringProperty(this->m_propertiesHandle, vr::Prop_RenderModelName_String, "{massless}massless_pen");

    driver_properties->SetStringProperty(this->m_propertiesHandle, vr::Prop_DriverVersion_String, DriverVersion::m_driverVersion.to_string().c_str());

    driver_properties->SetStringProperty(this->m_propertiesHandle, vr::Prop_NamedIconPathDeviceOff_String, "{massless}/icons/pen_not_ready.png");
    driver_properties->SetStringProperty(this->m_propertiesHandle, vr::Prop_NamedIconPathDeviceSearching_String, "{massless}/icons/pen_not_ready.png");
    driver_properties->SetStringProperty(this->m_propertiesHandle, vr::Prop_NamedIconPathDeviceSearchingAlert_String, "{massless}/icons/pen_not_ready.png");
    driver_properties->SetStringProperty(this->m_propertiesHandle, vr::Prop_NamedIconPathDeviceReady_String, "{massless}/icons/pen_ready.png");
    driver_properties->SetStringProperty(this->m_propertiesHandle, vr::Prop_NamedIconPathDeviceReadyAlert_String, "{massless}/icons/pen_not_ready.png");
    driver_properties->SetStringProperty(this->m_propertiesHandle, vr::Prop_NamedIconPathDeviceNotReady_String, "{massless}/icons/pen_not_ready.png");
    driver_properties->SetStringProperty(this->m_propertiesHandle, vr::Prop_NamedIconPathDeviceStandby_String, "{massless}/icons/pen_not_ready.png");
    driver_properties->SetStringProperty(this->m_propertiesHandle, vr::Prop_NamedIconPathDeviceAlertLow_String, "{massless}/icons/pen_not_ready.png");

    driver_properties->SetStringProperty(this->m_propertiesHandle, vr::Prop_InputProfilePath_String, "{massless}/input/massless_pen_profile.json");
    

    driver_properties->SetBoolProperty(this->m_propertiesHandle, vr::Prop_Identifiable_Bool, true);

    driver_properties->SetBoolProperty(this->m_propertiesHandle, vr::Prop_DeviceProvidesBatteryStatus_Bool, true);
    driver_properties->SetFloatProperty(this->m_propertiesHandle, vr::Prop_DeviceBatteryPercentage_Float, 1.0f);

    // Enable this to get SteamVR to show an update is available
    // driver_properties->SetBoolProperty(this->m_propertiesHandle, vr::Prop_Firmware_UpdateAvailable_Bool, true);
    // driver_properties->SetBoolProperty(this->m_propertiesHandle, vr::Prop_Firmware_ManualUpdate_Bool, true);
    // driver_properties->SetStringProperty(this->m_propertiesHandle, vr::Prop_Firmware_ManualUpdateURL_String, "https://massless.io/");
    if (Handedness(this->m_settingsManager->getSettings().getValue<std::string>(DriverSettings::Handedness).value()) == Handedness::LEFT) {
        driver_properties->SetInt32Property(this->m_propertiesHandle, vr::Prop_ControllerRoleHint_Int32, vr::ETrackedControllerRole::TrackedControllerRole_LeftHand);
    }
    else if (Handedness(this->m_settingsManager->getSettings().getValue<std::string>(DriverSettings::Handedness).value()) == Handedness::RIGHT) {
        driver_properties->SetInt32Property(this->m_propertiesHandle, vr::Prop_ControllerRoleHint_Int32, vr::ETrackedControllerRole::TrackedControllerRole_RightHand);
    }

    return vr::EVRInitError::VRInitError_None;
}

void PenController::Deactivate()
{
	this->m_deviceIndex = vr::k_unTrackedDeviceIndexInvalid;
    auto pen_system_lock = this->m_masslessManager->getPenSystem();
    if (pen_system_lock.pen_system.has_value())
        pen_system_lock.pen_system.value()->stopSystem();
}

void PenController::EnterStandby()
{
}

void* PenController::GetComponent(const char* pchComponentNameAndVersion)
{
	return nullptr;
}

void PenController::DebugRequest(const char* pchRequest, char* pchResponseBuffer, uint32_t unResponseBufferSize)
{
	if (unResponseBufferSize >= 1)
		pchResponseBuffer[0] = 0;
}

vr::DriverPose_t PenController::GetPose()
{
	return this->m_currentPenPose;
}

vr::DriverPose_t PenController::makeOpenVRPose(const MasslessInterface::Pose& pen_pose, const MasslessInterface::Pose& tracking_ref_offset, const std::pair<Eigen::Quaternionf, Eigen::Vector3f>& tracking_reference_pose, const MasslessInterface::TrackingSystemType& tracking_ref_type)
{
	vr::DriverPose_t out_pose = { 0 };
    
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
    Eigen::Quaternionf tracking_ref_steamvr_space_rotation = tracking_reference_pose.first;
    const Eigen::Vector3f& tracking_ref_steamvr_space_translation = tracking_reference_pose.second;

    // Massless tracker pose (offset) from tracking reference.
    const MasslessInterface::Pose& tracker_pose = tracking_ref_offset;

    // Rotation #1
	Eigen::Vector3f tracker_local_x_axis = tracking_ref_steamvr_space_rotation * Eigen::Vector3f(1, 0, 0);
	Eigen::Quaternionf tracker_coordinate_to_steamvr_coordinate;
	if (tracking_ref_type.getSystemType() == MasslessInterface::TrackingSystemType::SystemType::VIVE_TRACKER) {
		tracker_coordinate_to_steamvr_coordinate = Eigen::AngleAxisf(pi/2.0f, tracker_local_x_axis);
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
    Eigen::Vector3f pen_ms_space_translation = tracker_ms_space_rotation * Eigen::Vector3f(pen_pose.m_x, pen_pose.m_y, pen_pose.m_z) + tracker_ms_space_translation;
    Eigen::Quaternionf pen_ms_space_rotation = tracker_ms_space_rotation * Eigen::Quaternionf(pen_pose.m_qr, pen_pose.m_qx, pen_pose.m_qy, pen_pose.m_qz);
       
    Eigen::Vector3f pen_steamvr_space_translation = tracking_ref_steamvr_space_rotation * pen_ms_space_translation;
    Eigen::Quaternionf pen_steamvr_space_rotation = tracking_ref_steamvr_space_rotation * pen_ms_space_rotation;

    // Rotation #2
    Eigen::Vector3f local_x_axis = pen_steamvr_space_rotation * Eigen::Vector3f(1, 0, 0);
    Eigen::Vector3f local_z_axis = pen_steamvr_space_rotation * Eigen::Vector3f(0, 0, 1);
    pen_steamvr_space_rotation = tracker_coordinate_to_steamvr_coordinate * Eigen::AngleAxisf(-pi /2, local_z_axis) * Eigen::AngleAxisf(pi / 2, local_x_axis) * pen_steamvr_space_rotation;
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
    
	return out_pose;
}

vr::DriverPose_t PenController::makeNotTrackingOpenVRPose()
{
    vr::DriverPose_t out_pose = { 0 };
    out_pose.deviceIsConnected = true;
    out_pose.poseIsValid = false;
    out_pose.result = vr::TrackingResult_Calibrating_OutOfRange;
    out_pose.willDriftInYaw = false;
    out_pose.shouldApplyHeadModel = false;

    return out_pose;
}

vr::DriverPose_t PenController::makeCalibratingOpenVRPose()
{
    vr::DriverPose_t out_pose = { 0 };
    out_pose.deviceIsConnected = true;
    out_pose.poseIsValid = false;
    out_pose.result = vr::TrackingResult_Calibrating_InProgress;
    out_pose.willDriftInYaw = false;
    out_pose.shouldApplyHeadModel = false;

    return out_pose;
}

vr::DriverPose_t PenController::makeDisconnectedOpenVRPose()
{
    vr::DriverPose_t out_pose = { 0 };
    out_pose.deviceIsConnected = false;
    out_pose.poseIsValid = false;
    out_pose.result = vr::ETrackingResult::TrackingResult_Uninitialized;
    out_pose.willDriftInYaw = false;
    out_pose.shouldApplyHeadModel = false;

    return out_pose;
}

void PenController::onGestureEvent(const MasslessInterface::PenEvent& event, vr::IVRDriverInput* driver_input)
{
    m_eventStack.push_back(event);
         
    bool do_clear_stack = false;
    for (auto gesture : m_gestureHandlers) {
        if (gesture.match(m_eventStack)) {
            do_clear_stack = gesture.performAction(m_eventStack, driver_input, this->m_masslessManager);
            break;
        }
    }
    if (do_clear_stack)
        m_eventStack.clear();
}

vr::TrackedDeviceIndex_t PenController::getIndex()
{
    return this->m_deviceIndex;
}

