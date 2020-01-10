/* Copyright (C) 2020 Massless Corp. - All Rights Reserved
 * Author: Jacob Hilton.
 * You may use, distribute and modify this code under the
 * terms of the BSD 3-Clause "New" or "Revised" License.
 *
 * You should have received a copy of this license with
 * this file. If not, please email support@massless.io
 */

#include "MasslessInterface.hpp"
#include "MasslessPenSystem.h"
#include "MasslessErrors.h"

using namespace MasslessInterface;

MasslessPenSystem* MasslessPenSystem::m_thiz = nullptr;

MasslessPenSystem::MasslessPenSystem()
{
    if (this->getThis() != nullptr)
        throw std::exception("MasslessPenSystem is already instantiated somewhere else!");
    this->setThis(this);
}

MasslessPenSystem::~MasslessPenSystem() noexcept
{
    this->stopSystem();
    this->setThis(nullptr);
}

std::optional<MasslessPenSystem::ErrorType> MasslessPenSystem::setIntegrationKey(MasslessPenSystem::IntegrationKey integration_key) noexcept
{ 
    this->m_integrationKey = integration_key;
    // Change key now, otherwise postpone till startup
    if (this->isSystemRunning()) {
        if (ErrorType err = PenChangeIntegrationKey(this->m_integrationKey.data()); err != EXIT_SUCCESS) {
            return err;
        }
    }
    return std::nullopt;
}

std::optional<MasslessPenSystem::ErrorType> MasslessPenSystem::startSystem(bool force) noexcept
{
    if (!force && this->m_isRunning) {
        return std::nullopt;
    }

    ErrorType err = EXIT_SUCCESS;

    // Attach default notification listener
    err = PenAttachNotificationListener([](uint32_t length, uint8_t* raw_data)->int {
        MasslessPenSystem* thiz = MasslessPenSystem::getThis();
        if (thiz == nullptr)
            return 0;
        return thiz->handleNotification(length, raw_data);
        });

    if (err != EXIT_SUCCESS) {
        return err;
    }

    // Attach default event listener
    err = PenAttachEventListener([](uint32_t length, uint8_t* raw_data)->int {
        MasslessPenSystem* thiz = MasslessPenSystem::getThis();
        if (thiz == nullptr)
            return 0;
        return thiz->handleEvent(length, raw_data);

        });

    if (err != EXIT_SUCCESS) {
        return err;
    }

    if (ErrorType exit_value = PenStart(this->m_integrationKey.data()); exit_value != EXIT_SUCCESS) {
        return exit_value;
    }

    // Reset pose to blank
    this->setCurrentPose(Pose(0, 0, 0, 1, 0, 0, 0, std::chrono::system_clock::time_point{}));


    // Attach default pose listener
    err = PenAttachFullPoseListener([](uint32_t length, uint8_t* raw_pose)->int {
        MasslessPenSystem* thiz = MasslessPenSystem::getThis();
        if (thiz == nullptr)
            return 0;
        return thiz->handlePose(length, raw_pose);
    });

    if(err != EXIT_SUCCESS) {
        return err;
    }

    // Attach default state listener
    err = PenAttachFullStateListener([](uint32_t length, uint8_t* raw_state)->int {
        MasslessPenSystem* thiz = MasslessPenSystem::getThis();
        if (thiz == nullptr)
            return 0;
        return thiz->handleState(length, raw_state);
    });

    if (err != EXIT_SUCCESS) {
        return err;
    }

    

    this->m_isRunning = true;
    return std::nullopt;
}

std::optional<MasslessPenSystem::ErrorType> MasslessPenSystem::stopSystem(bool force) noexcept
{
    if (!force && !this->m_isRunning) {
        return std::nullopt;
    }
    if (ErrorType exit_value = PenStop(); exit_value != EXIT_SUCCESS) {
        return exit_value;
    }
    this->m_isRunning = false;
    return std::nullopt;
}

bool MasslessPenSystem::isSystemRunning() noexcept
{
    return this->m_isRunning;
}

tl::expected<Pose, MasslessPenSystem::ErrorType> MasslessPenSystem::getMasslessTrackerPoseOffset(TrackingSystemType type) noexcept
{
    Pose out_pose = Pose();
    ErrorType status = PenGetTrackerPose(type.getIntValue(), &out_pose.m_x, &out_pose.m_y, &out_pose.m_z, &out_pose.m_qr, &out_pose.m_qx, &out_pose.m_qy, &out_pose.m_qz);
    if (status != EXIT_SUCCESS) {
        return tl::make_unexpected(status);
    }
    out_pose.m_x *= this->m_unitScale;
    out_pose.m_y *= this->m_unitScale;
    out_pose.m_z *= this->m_unitScale;
    return out_pose;
}

MasslessPenSystem::IntegrationKey MasslessPenSystem::getIntegrationKey() noexcept
{
	return this->m_integrationKey;
}

tl::expected<std::string, MasslessPenSystem::ErrorType> MasslessPenSystem::getPenSerial() noexcept
{
    uint8_t serial_raw[16] = { 0 };
    ErrorType error = GetPenDetails(nullptr, nullptr, nullptr, nullptr, serial_raw, nullptr);
    if (error) {
        return tl::make_unexpected(error);
    }
    std::stringstream ss;
    ss << std::hex;
    for (uint8_t element : serial_raw) {
        ss << +element;
    }
    return ss.str();
}

bool MasslessPenSystem::isPenConnected() noexcept
{
    if (!this->isSystemRunning())
        return false;

    return this->m_isPenConnected;

}

bool MasslessInterface::MasslessPenSystem::isPenTracking() noexcept
{
    if (!this->isPenConnected()) {
        return false;
    }
    return this->getCurrentPose().m_timestamp > ( std::chrono::system_clock::now() - this->m_penNotTrackingTimeout);
}

void MasslessPenSystem::setPoseCallback(std::function<void(Pose)> callback_fn) noexcept
{
	this->m_poseCallbackFn = callback_fn;
}

MasslessPenSystem::ErrorType MasslessInterface::MasslessPenSystem::handlePose(uint32_t length, uint8_t* raw_data) noexcept
{
    // Get message version
    uint8_t message_version = *(raw_data);

    // Get offset for pose data
    const float* pose_data = (float*)(raw_data + 1 * sizeof(uint8_t));
    auto pen_pose = Pose(pose_data[0] * this->getUnitScale(), pose_data[1] * this->getUnitScale(), pose_data[2] * this->getUnitScale(), pose_data[3], pose_data[4], pose_data[5], pose_data[6]);

    // Get offset for status 
    const uint8_t status = *(raw_data + 29 * sizeof(uint8_t));

    if (message_version >= 1) {
        // Get offset for position error
        const float* position_error_data = (float*)(raw_data + 30 * sizeof(uint8_t));
        pen_pose.m_ex = position_error_data[0];
        pen_pose.m_ey = position_error_data[1];
        pen_pose.m_ez = position_error_data[2];

        // Get offset for gyro readings
        const float* gyro_data = (float*)(raw_data + 42 * sizeof(uint8_t));
        pen_pose.m_gx = gyro_data[0];
        pen_pose.m_gy = gyro_data[1];
        pen_pose.m_gz = gyro_data[2];
    }

    this->setCurrentPose(pen_pose);
    // See if we have a pose callback
    auto pose_callback = this->getPoseCallback();
    if (pose_callback) {
        // Call pose callback
        (*pose_callback)(pen_pose);
    }

    return EXIT_SUCCESS;
}

void MasslessPenSystem::clearPoseCallback() noexcept
{
    this->m_poseCallbackFn = std::nullopt;
}

std::optional<std::function<void(Pose)>> MasslessPenSystem::getPoseCallback() noexcept
{
	return this->m_poseCallbackFn;
}

Pose MasslessPenSystem::getCurrentPose() noexcept
{
	return this->m_latestPose;
}

void MasslessPenSystem::setStateCallback(std::function<void(PenState)> callback_fn) noexcept
{
    this->m_stateCallbackFn = callback_fn;
}

void MasslessPenSystem::clearStateCallback() noexcept
{
    this->m_stateCallbackFn = std::nullopt;
}

MasslessPenSystem::ErrorType MasslessInterface::MasslessPenSystem::handleState(uint32_t length, uint8_t* raw_data) noexcept
{
    uint8_t message_version = *(raw_data);

    // Get offset for surface reading
    const float* surface_reading_data = (float*)(&raw_data[1]);

    // Get offset for capsense reading
    const uint8_t* capsense_reading_data = (uint8_t*)(&raw_data[5]);

    // Make basic state object
    PenState pen_state = PenState(*surface_reading_data, *capsense_reading_data);

    if (message_version >= 1) {
        // Get offset for surface touched state
        const uint8_t* surface_touched = (uint8_t*)(&raw_data[6]);

        pen_state.m_surfaceFound = (*surface_touched != 0x00);

        // Get offset for tap detected state
        const uint8_t* tap_detected = (uint8_t*)(&raw_data[7]);

        // Add additional data to state object
        // TODO: Update this when format is finalised
        pen_state.m_isTapped = (*tap_detected != 0x00);
    }

    this->setCurrentState(pen_state);

    auto state_callback = this->getStateCallback();
    if (state_callback) {
        // Call state callback
        (*state_callback)(pen_state);
    }

    return EXIT_SUCCESS;
}

std::optional<std::function<void(PenState)>> MasslessPenSystem::getStateCallback() noexcept
{
	return this->m_stateCallbackFn;
}

PenState MasslessPenSystem::getCurrentState() noexcept
{
	return this->m_latestState;
}

MasslessPenSystem::ErrorType MasslessInterface::MasslessPenSystem::handleNotification(uint32_t length, uint8_t* raw_data) noexcept
{
    uint8_t message_version = *(raw_data);

    // TODO verify this is correct
    const uint8_t* notification_type = (uint8_t*)(&raw_data[1]);

    uint16_t* code = nullptr;

    char* message = nullptr;

    if (*notification_type == PenNotification::ERROR || *notification_type == PenNotification::WARNING) {
        code = (uint16_t*)(&raw_data[2]);
        message = (char*)(&raw_data[4]);
    }
    else {
        message = (char*)(&raw_data[2]);
    }

    PenNotification notification(static_cast<PenNotification::NotificationType>(*notification_type), code == nullptr ? std::nullopt : std::make_optional(*code), std::string(message));

    this->pushNotification(notification);

    auto notification_callback = this->getNotificationCallback();
    if (notification_callback) {
        // Call notification callback
        (*notification_callback)(notification);
    }

    return 0;
}

void MasslessPenSystem::setNotificationCallback(std::function<void(PenNotification)> callback_fn) noexcept
{
    this->m_notificationCallbackFn = callback_fn;
}

void MasslessPenSystem::clearNotificationCallback() noexcept
{
    this->m_notificationCallbackFn = std::nullopt;
}

std::optional<std::function<void(PenNotification)>> MasslessPenSystem::getNotificationCallback() noexcept
{
    return this->m_notificationCallbackFn;
}

std::optional<PenNotification> MasslessPenSystem::popNotification() noexcept
{
    // Lock access to the queue
    std::lock_guard<std::mutex> lock(this->m_notificationQueueLock);
    if (this->m_storedNotifications.empty()) {
        // nullopt if empty
        return std::nullopt;
    }
    else {
        // get notification and return
        PenNotification notification = this->m_storedNotifications.front();
        this->m_storedNotifications.pop();
        return notification;
    }
}

MasslessPenSystem::ErrorType MasslessInterface::MasslessPenSystem::handleEvent(uint32_t length, uint8_t* raw_data) noexcept
{
    if (length >= 2) {
        uint16_t EventTypeValue;
        memcpy(&EventTypeValue, raw_data, sizeof(uint16_t));
        Massless::Events::EventType event_type = static_cast<Massless::Events::EventType>(EventTypeValue);

        const uint8_t* struct_begin = raw_data + sizeof(uint16_t);
        const std::size_t struct_size = length - sizeof(uint16_t);
        std::optional<std::shared_ptr<Massless::Events::BaseEvent>> event_struct = std::nullopt;

        switch (event_type) {
            case Massless::Events::EventType::PenConnected:
            {
                this->m_isPenConnected = true;
            } break;
            case Massless::Events::EventType::PenDisconnected:
            {
                this->m_isPenConnected = false;
            } break;
            case Massless::Events::EventType::PenBattery:
            {
                Massless::Events::PenBatteryEvent battery_event;
                std::memcpy(&battery_event, struct_begin, struct_size);
                event_struct = std::make_shared<Massless::Events::PenBatteryEvent>(battery_event);
            } break;
            case Massless::Events::EventType::Error:
            {
                Massless::Events::ErrorEvent error_event;
                std::memcpy(&error_event, struct_begin, struct_size);
                event_struct = std::make_shared<Massless::Events::ErrorEvent>(error_event);
            } break;
            case Massless::Events::EventType::TouchPadHeld:
            {
                Massless::Events::TouchPadHeldEvent held_event;
                std::memcpy(&held_event, struct_begin, struct_size);
                event_struct = std::make_shared<Massless::Events::TouchPadHeldEvent>(held_event);
            } break;
            case Massless::Events::EventType::TouchPadMultiTapNew:
            {
                Massless::Events::TouchPadMultiTapNewEvent multi_new_event;
                std::memcpy(&multi_new_event, struct_begin, struct_size);
                event_struct = std::make_shared<Massless::Events::TouchPadMultiTapNewEvent>(multi_new_event);
            } break;
            case Massless::Events::EventType::TouchPadMultiTapTotal:
            {
                Massless::Events::TouchPadMultiTapNewEvent multi_total_event;
                std::memcpy(&multi_total_event, struct_begin, struct_size);
                event_struct = std::make_shared<Massless::Events::TouchPadMultiTapNewEvent>(multi_total_event);
            } break;
            case Massless::Events::EventType::TouchPadPressed:
            {
                Massless::Events::TouchPadPressedEvent pressed_event;
                std::memcpy(&pressed_event, struct_begin, struct_size);
                event_struct = std::make_shared<Massless::Events::TouchPadPressedEvent>(pressed_event);
            } break;
            case Massless::Events::EventType::TouchPadReleased:
            {
                Massless::Events::TouchPadReleasedEvent released_event;
                std::memcpy(&released_event, struct_begin, struct_size);
                event_struct = std::make_shared<Massless::Events::TouchPadReleasedEvent>(released_event);
            } break;
            case Massless::Events::EventType::TouchPadSwipe:
            {
                Massless::Events::TouchPadSwipeEvent swipe_event;
                std::memcpy(&swipe_event, struct_begin, struct_size);
                event_struct = std::make_shared<Massless::Events::TouchPadSwipeEvent>(swipe_event);
            } break;
            case Massless::Events::EventType::PenOK:
            case Massless::Events::EventType::CameraOK:
            case Massless::Events::EventType::TedOK:
                break;
            default:
                this->pushNotification({ PenNotification::WARNING, std::nullopt, "[Driver] Unknown pen event received : " + std::to_string(event_type) });
        }

        PenEvent event(event_type, event_struct);
        this->pushEvent(event);

        auto event_callback = this->getEventCallback();
        if (event_callback) {
            // Call notification callback
            (*event_callback)(event);
        }
    }

    return EXIT_SUCCESS;
}

void MasslessPenSystem::setEventCallback(std::function<void(PenEvent)> callback_fn) noexcept
{
    this->m_eventCallbackFn = callback_fn;
}

void MasslessPenSystem::clearEventCallback() noexcept
{
    this->m_eventCallbackFn = std::nullopt;
}

std::optional<std::function<void(PenEvent)>> MasslessPenSystem::getEventCallback() noexcept
{
    return this->m_eventCallbackFn;
}

std::optional<PenEvent> MasslessPenSystem::popEvent() noexcept
{
    // Lock access to the queue
    std::lock_guard<std::mutex> lock(this->m_eventQueueLock);
    if (this->m_storedEvents.empty()) {
        // nullopt if empty
        return std::nullopt;
    }
    else {
        // get notification and return
        PenEvent event = this->m_storedEvents.front();
        this->m_storedEvents.pop();
        return event;
    }
}

void MasslessPenSystem::setUnitScale(float scale) noexcept
{
	this->m_unitScale = scale;
}

float MasslessInterface::MasslessPenSystem::getUnitScale() noexcept
{
    return this->m_unitScale;
}

std::tuple<uint64_t, uint64_t, uint64_t> MasslessInterface::MasslessPenSystem::getDllVersion() noexcept
{
    uint64_t version_packed = PenGetDllVersionNumber();
    uint64_t version_major = ((version_packed) >> (16 * 3));
    uint64_t version_minor = (version_packed ^ (version_major) << (16 * 3)) >> (16 * 2);
    uint64_t version_patch = (version_packed << (16 * 3)) >> (16 * 3);
    return { version_major, version_minor, version_patch };
}

std::string MasslessInterface::MasslessPenSystem::getDllVersionString() noexcept
{
    auto dllVersion = MasslessPenSystem::getDllVersion();
    return (std::stringstream() << std::get<0>(dllVersion) << "." << std::get<1>(dllVersion) << "." << std::get<2>(dllVersion)).str();
}

MasslessPenSystem* MasslessInterface::MasslessPenSystem::getThis()
{
    return MasslessPenSystem::m_thiz;
}

void MasslessInterface::MasslessPenSystem::setThis(MasslessPenSystem* thiz)
{
    MasslessPenSystem::m_thiz = thiz;
}

void MasslessPenSystem::setCurrentPose(Pose pose) noexcept
{
	this->m_latestPose = pose;
}

void MasslessPenSystem::setCurrentState(PenState state) noexcept
{
    this->m_latestState = state;
}

void MasslessPenSystem::pushNotification(PenNotification notification) noexcept
{
    std::lock_guard<std::mutex> lock(this->m_notificationQueueLock);
    this->m_storedNotifications.push(notification);
}

void MasslessPenSystem::pushEvent(PenEvent event) noexcept
{
    std::lock_guard<std::mutex> lock(this->m_eventQueueLock);
    this->m_storedEvents.push(event);
}

std::optional<MasslessPenSystem::ErrorType> MasslessPenSystem::sendVibration(uint16_t duration) noexcept
{
    if (ErrorType err = PenSimpleBuzz(duration); err != EXIT_SUCCESS)
        return err;
    return std::nullopt;
}
