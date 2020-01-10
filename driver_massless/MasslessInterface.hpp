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
#include <functional>
#include <optional>
#include <queue>
#include <mutex>
#include <chrono>

#include <DriverLog.hpp>

#include <IPenSystem.hpp>

namespace MasslessInterface {

	/// <summary>
	/// C++ interface to the Massless Pen C API.
	/// </summary>
	class MasslessPenSystem : public IPenSystem
	{
	public:
        // Will throw a std::exception if this class is instantiated already when called
        MasslessPenSystem() noexcept(false);
        virtual ~MasslessPenSystem() noexcept;

        IntegrationKey getIntegrationKey() noexcept override;
        std::optional<ErrorType> setIntegrationKey(IntegrationKey integration_key) noexcept override;

        std::optional<ErrorType> startSystem(bool force = false) noexcept override;
        std::optional<ErrorType> stopSystem(bool force = false) noexcept override;
        bool isSystemRunning() noexcept override;

        tl::expected<Pose, ErrorType> getMasslessTrackerPoseOffset(TrackingSystemType type) noexcept override;

        tl::expected<std::string, ErrorType> getPenSerial() noexcept override;
        bool isPenConnected() noexcept override;
        bool isPenTracking() noexcept override;

        ErrorType handlePose(uint32_t length, uint8_t* raw_data) noexcept override;
        std::optional<std::function<void(Pose)>> getPoseCallback() noexcept override;
		void setPoseCallback(std::function<void(Pose)> callback_fn) noexcept override;
		void clearPoseCallback() noexcept override;
		Pose getCurrentPose() noexcept override;
        void setCurrentPose(Pose pose) noexcept override;

        ErrorType handleState(uint32_t length, uint8_t* raw_data) noexcept override;
        std::optional<std::function<void(PenState)>> getStateCallback() noexcept override;
		void setStateCallback(std::function<void(PenState)> callback_fn) noexcept override;
		void clearStateCallback() noexcept override;
		PenState getCurrentState() noexcept override;
        void setCurrentState(PenState state) noexcept override;

        ErrorType handleNotification(uint32_t length, uint8_t* raw_data) noexcept override;
        void setNotificationCallback(std::function<void(PenNotification)> callback_fn) noexcept override;
        std::optional<std::function<void(PenNotification)>> getNotificationCallback() noexcept override;
        void clearNotificationCallback() noexcept override;
        std::optional<PenNotification> popNotification() noexcept override;
        void pushNotification(PenNotification notification) noexcept override;

        ErrorType handleEvent(uint32_t length, uint8_t* raw_data) noexcept override;
        void setEventCallback(std::function<void(PenEvent)> callback_fn) noexcept override;
        std::optional<std::function<void(PenEvent)>> getEventCallback() noexcept override;
        void clearEventCallback() noexcept override;
        std::optional<PenEvent> popEvent() noexcept override;
        void pushEvent(PenEvent event) noexcept override;

        std::optional<ErrorType> sendVibration(uint16_t duration = 200) noexcept override;

		void setUnitScale(float scale) noexcept override;
        float getUnitScale() noexcept override;

        std::tuple<uint64_t, uint64_t, uint64_t> getDllVersion() noexcept override;
        std::string getDllVersionString() noexcept override;

        /// <summary>
        /// Sets the "global" reference to the currently instantiated MasslessPenSystem
        /// We need to have some global state we can access within the callbacks, so this is needed
        /// </summary>
        /// <returns>Pointer to "this", can be nullptr</returns>
        static MasslessPenSystem* getThis();


        /// Sets the current reference to "this"
        /// </summary>
        /// <param name="thiz"></param>
        static void setThis(MasslessPenSystem* thiz);

	private:
        // "this" storage for C callbacks
        static MasslessPenSystem* m_thiz /* = nullptr */;
        
		/// <summary>
		/// Stored integration key
		/// </summary>
        IntegrationKey m_integrationKey = { 0 };
        
		/// <summary>
		/// Set to true when start() is called, and false when stop() is called
		/// </summary>
		bool m_isRunning = false;

		/// <summary>
		/// Storage for pose callback fn., std::nullopt by default and if cleared
		/// </summary>
		std::optional<std::function<void(Pose)>> m_poseCallbackFn = std::nullopt;

		/// <summary>
		/// Storage for state callback fn., std::nullopt by default and if cleared
		/// </summary>
		std::optional<std::function<void(PenState)>> m_stateCallbackFn = std::nullopt;

        /// <summary>
        /// Storage for notification callback fn., std::nullopt by default and if cleared
        /// </summary>
        std::optional<std::function<void(PenNotification)>> m_notificationCallbackFn = std::nullopt;

        /// <summary>
        /// Storage for event callback fn., std::nullopt by default and if cleared
        /// </summary>
        std::optional<std::function<void(PenEvent)>> m_eventCallbackFn = std::nullopt;

		/// <summary>
		/// Storage for latest pen state
		/// </summary>
		PenState m_latestState;

		/// <summary>
		/// Storage for latest pose
		/// </summary>
		Pose m_latestPose;

        /// <summary>
        /// Queue for notifications that come while update is not happening
        /// </summary>
        std::queue<MasslessInterface::PenNotification> m_storedNotifications;

        /// <summary>
        /// Lock for notification queue
        /// </summary>
        std::mutex m_notificationQueueLock;

        /// <summary>
        /// Queue for events that come while update is not happening
        /// </summary>
        std::queue<MasslessInterface::PenEvent> m_storedEvents;

        /// <summary>
        /// Lock for event queue
        /// </summary>
        std::mutex m_eventQueueLock;

		/// <summary>
		/// Unit scale for position output
		/// </summary>
		float m_unitScale = 1;

        /// <summary>
        /// How long from the previous pose time should the pen be considered tracking
        /// </summary>
        const std::chrono::seconds m_penNotTrackingTimeout{ 5 };

        /// <summary>
        /// Is the pen connected
        /// </summary>
        bool m_isPenConnected = false;
        
	};

};

