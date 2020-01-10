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
#include <optional>
#include <functional>
#include <string>

#include <expected.hpp>

#include <Pose.hpp>
#include <TrackingSystemType.hpp>
#include <PenState.hpp>
#include <PenNotification.hpp>
#include <PenEvent.hpp>

namespace MasslessInterface {
    class IPenSystem {
    public:
        using IntegrationKey = std::array<uint8_t, 16>;
        using ErrorType = int;

        /// <summary>
        /// Gets the currently set integration key
        /// </summary>
        /// <returns>Current integration key</returns>
        virtual IntegrationKey getIntegrationKey() = 0;

        /// <summary>
        /// Sets the current integration key.
        /// </summary>
        /// <param name="integration_key">Integration key array</param>
        /// <returns>Error code if an error occurred, nullopt on success (can only throw error if system is started)</returns>
        virtual std::optional<ErrorType> setIntegrationKey(IntegrationKey integration_key) = 0;

        /// <summary>
        /// Starts the tracking for the Pen System
        /// </summary>
        /// <param name="force">If true, it will attempt to force start even if isSystemRunning returns true</param>
        /// <returns>Error code if an error occurred, nullopt on success</returns>
        virtual std::optional<ErrorType> startSystem(bool force = false) = 0;

        /// <summary>
        /// Starts the tracking for the Pen System
        /// </summary>
        /// <param name="force">If true, it will attempt to force stop even if isSystemRunning returns false</param>
        /// <returns>Error code if an error occurred, nullopt on success</returns>
        virtual std::optional<ErrorType> stopSystem(bool force = false) = 0;

        /// <summary>
        /// Gets the current running state of the system
        /// </summary>
        /// <returns>True if the system is running, false otherwise</returns>
        virtual bool isSystemRunning() = 0;

        /// <summary>
        /// Gets the pose offset of the massless tracker camera
        /// This pose is relative to the tracking reference of system_type type
        /// </summary>
        /// <returns>Error code if an error occurred, or pose offset the massless tracker on success</returns>
        virtual tl::expected<Pose, ErrorType> getMasslessTrackerPoseOffset(TrackingSystemType system_type) = 0;

        /// <summary>
        /// Gets the serial of the pen
        /// </summary>
        /// <returns>Error code if an error occurred, or serial string on success</returns>
        virtual tl::expected<std::string, ErrorType> getPenSerial() = 0;

        /// <summary>
        /// Checks the connection status of the pen
        /// </summary>
        /// <returns>True if pen is connected, false otherwise. Will return false if system is stopped</returns>
        virtual bool isPenConnected() = 0;

        /// <summary>
        /// Checks if the pen is tracking
        /// </summary>
        /// <returns>True if the pen is trackng, false otherwise</returns>
        virtual bool isPenTracking() = 0;

        /// <summary>
        /// Called when a new pose is posted from the backend system
        /// </summary>
        /// <param name="length">length of the data posted</param>
        /// <param name="raw_data">pointer to the data</param>
        /// <returns>error code, or EXIT_SUCCESS on successful parsing</returns>
        virtual ErrorType handlePose(uint32_t length, uint8_t* raw_data) = 0;

        /// <summary>
        /// Gets the currently set pose callback function
        /// </summary>
        /// <returns>Pose callback function, or nullopt if none is set</returns>
        virtual std::optional<std::function<void(Pose)>> getPoseCallback() = 0;
        
        /// <summary>
        /// Sets the callback function that will be called each time a new pose is posted
        /// </summary>
        /// <param name="callback_fn">Callback function taking a Pose as a parameter</param>
        virtual void setPoseCallback(std::function<void(Pose)> callback_fn) = 0;

        /// <summary>
        /// Clears the currently set pose callback function
        /// </summary>
        virtual void clearPoseCallback() = 0;

        /// <summary>
        /// Gets the most recently posted pen pose
        /// </summary>
        /// <returns>Most recently posted pen pose</returns>
        virtual Pose getCurrentPose() = 0;

        /// <summary>
        /// Sets the latest pose.
        /// </summary>
        /// <param name="pose">New pose data</param>
        virtual void setCurrentPose(Pose pose) = 0;

        /// <summary>
        /// Called when a new state is posted from the backend system
        /// </summary>
        /// <param name="length">length of the data posted</param>
        /// <param name="raw_data">pointer to the data</param>
        /// <returns>error code, or EXIT_SUCCESS on successful parsing</returns>
        virtual ErrorType handleState(uint32_t length, uint8_t* raw_data) = 0;

        /// <summary>
        /// Gets the currently set state callback function
        /// </summary>
        /// <returns>The state callback function, or nullopt when null</returns>
        virtual std::optional<std::function<void(PenState)>> getStateCallback() = 0;

        /// <summary>
        /// Sets the callback function that will be called each time a new pen state is posted
        /// </summary>
        /// <param name="callback_fn">Callback function taking a PenState</param>
        virtual void setStateCallback(std::function<void(PenState)> callback_fn) = 0;

        /// <summary>
        /// Clears the currently set state callback function
        /// </summary>
        virtual void clearStateCallback() = 0;

        /// <summary>
        /// Gets the most recently posted pen state
        /// </summary>
        /// <returns>Most recently posted pen state</returns>
        virtual PenState getCurrentState() = 0;

        /// <summary>
        /// Sets the current state
        /// </summary>
        /// <param name="state">New state data</param>
        virtual void setCurrentState(PenState state) = 0;

        /// <summary>
        /// Called when a new notification is posted from the backend system
        /// </summary>
        /// <param name="length">length of the data posted</param>
        /// <param name="raw_data">pointer to the data</param>
        /// <returns>error code, or EXIT_SUCCESS on successful parsing</returns>
        virtual ErrorType handleNotification(uint32_t length, uint8_t* raw_data) = 0;

        /// <summary>
        /// Gets the currently set notification callback function
        /// </summary>
        /// <returns>The notification callback function, or nullopt when unset</returns>
        virtual std::optional<std::function<void(PenNotification)>> getNotificationCallback() = 0;
        
        /// <summary>
        /// Sets the callback function that will be called each time a new pen notification is posted
        /// </summary>
        /// <param name="callback_fn">Callback function taking a PenNotification</param>
        virtual void setNotificationCallback(std::function<void(PenNotification)> callback_fn) = 0;
        
        /// <summary>
        /// Clears the currently set notification callback function
        /// </summary>
        virtual void clearNotificationCallback() = 0;

        /// <summary>
        /// Gets the most recently posted pen notification
        /// </summary>
        /// <returns>Latest pen notification, or nullopt when the queue is empty</returns>
        virtual std::optional<PenNotification> popNotification() = 0;

        /// <summary>
        /// Adds a notification
        /// </summary>
        /// <param name="notification">New notification from Massless API</param>
        virtual void pushNotification(PenNotification notification) = 0;
        
        /// <summary>
        /// Called when a new event is posted from the backend system
        /// </summary>
        /// <param name="length">length of the data posted</param>
        /// <param name="raw_data">pointer to the data</param>
        /// <returns>error code, or EXIT_SUCCESS on successful parsing</returns>
        virtual ErrorType handleEvent(uint32_t length, uint8_t* raw_data) = 0;

        /// <summary>
        /// Gets the currently set event callback function
        /// </summary>
        /// <returns>The event callback function, or nullopt when unset</returns>
        virtual std::optional<std::function<void(PenEvent)>> getEventCallback() = 0;

        /// <summary>
        /// Sets the callback function that will be called each time a new pen event is posted
        /// </summary>
        /// <param name="callback_fn">Callback function taking a PenEvent</param>
        virtual void setEventCallback(std::function<void(PenEvent)> callback_fn) = 0;
       
        /// <summary>
        /// Clears the currently set event callback function
        /// </summary>
        virtual void clearEventCallback() = 0;

        /// <summary>
        /// Gets the most recently posted pen event
        /// </summary>
        /// <returns>Latest pen event, or nullopt when the queue is empty</returns>
        virtual std::optional<PenEvent> popEvent() = 0;

        /// <summary>
        /// Adds an event
        /// </summary>
        /// <param name="event">New event from Massless API</param>
        virtual void pushEvent(PenEvent event) = 0;

        /// <summary>
        /// Sends a haptic feedback vibration to the pen
        /// </summary>
        /// <param name="duration">Duration for the vibration in milliseconds</param>
        /// <returns>Error code when an error occurs, or nullopt on success</returns>
        virtual std::optional<ErrorType> sendVibration(uint16_t duration = 200) = 0;
        
        /// /// <summary>
        /// Sets the reported measurement unit scale
        /// </summary>
        /// <param name="scale">Scaling relative to 1m (eg. scale of 1000 would report in kilometres)</param>
        virtual void setUnitScale(float scale) = 0;

        /// <summary>
        /// Gets the reported measurement unit scale
        /// </summary>
        /// <returns>Scaling relative to 1m (eg. scale of 1000 would report in kilometres)</returns>
        virtual float getUnitScale() = 0;

        /// <summary>
        /// Gets the semver version string of the DLL we are currently linked to
        /// </summary>
        /// <returns>DLL version string</returns>
        virtual std::string getDllVersionString() = 0;

        /// <summary>
        /// Gets the semver version number of the DLL we are currently linked to
        /// </summary>
        /// <returns>DLL version number</returns>
        virtual std::tuple<uint64_t, uint64_t, uint64_t> getDllVersion() = 0;

        virtual ~IPenSystem() noexcept = default;
    };
}