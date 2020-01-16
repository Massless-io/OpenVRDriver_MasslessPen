/* Copyright (C) 2020 Massless Corp. - All Rights Reserved
 * Author: Jacob Hilton.
 * You may use, distribute and modify this code under the
 * terms of the BSD 3-Clause "New" or "Revised" License.
 *
 * You should have received a copy of this license with
 * this file. If not, please email support@massless.io
 */

#pragma once

#include <MasslessPenSystemCallbacks.h>
#include <vector>
#include <map>

namespace MasslessInterface{
    /// <summary>
    /// Defines a supported tracking system type
    /// </summary>
    class TrackingSystemType {
    public:
        /// <summary>
        /// The SystemType enum declares what type of tracking reference device we are using.
        /// We currently support four types of device.
        /// </summary>
        /// <detail>
        /// The reason we start at 3 in the enum is that there were legacy devices that were incorrectly
        /// reported in the other numbers, so to maintain backwards compatibility whilst fixing the issue
        /// we just used the next available set of numbers.
        /// </detail>
        enum class SystemType : int {
            INVALID_SYSTEM,
            OCULUS_SENSOR,
            VIVE_BASESTATION_V1,
            VIVE_TRACKER,
            VIVE_BASESTATION_V2,
            NONE,
            OCULUS_RIFT_S_TOUCH_RIGHT,
            OCULUS_RIFT_S_TOUCH_LEFT
        };

    private:
        /// <summary>
        /// Stored mappings from enum to api int and printable string values
        /// </summary>
        static const inline std::map<SystemType, std::pair<int, std::string>> m_systemTypeInfo{
            {SystemType::INVALID_SYSTEM, {-1, "Invalid System"}},
            {SystemType::OCULUS_SENSOR, {XRTrackingNodeType::OculusSensor, "Oculus Sensor"}},
            {SystemType::VIVE_BASESTATION_V1, {XRTrackingNodeType::ViveBaseStationV1, "Vive Basestation v1"}},
            {SystemType::VIVE_TRACKER, {XRTrackingNodeType::ViveTracker, "Vive Tracker"}},
            {SystemType::VIVE_BASESTATION_V2, {XRTrackingNodeType::ViveBaseStationV2, "Vive Basestation v2"}},
            {SystemType::NONE, {XRTrackingNodeType::None, "No reference system"}},
            {SystemType::OCULUS_RIFT_S_TOUCH_RIGHT, {XRTrackingNodeType::OculusTouchV2Right, "Oculus Rift S Touch Right"}},
            {SystemType::OCULUS_RIFT_S_TOUCH_LEFT, {XRTrackingNodeType::OculusTouchV2Left, "Oculus Rift S Touch Left"}},
        };
    public:

        /// <summary>
        /// Gets a vector of all valid tracking system types (minus INVALID_SYSTEM)
        /// </summary>
        /// <returns>A vector of all valid tracking system types</returns>
        static std::vector<SystemType> getAllSystemTypes() {
            std::vector<SystemType> rtn;
            for (const auto& systemType : m_systemTypeInfo) {
                if(systemType.first != SystemType::INVALID_SYSTEM)
                    rtn.push_back(systemType.first);
            }
            return rtn;
        }

        /// <summary>
        /// Gets the integer value of a tracking system type
        /// </summary>
        /// <param name="system_type">Tracking system type</param>
        /// <returns>The integer value of the system type</returns>
        static int getIntValue(SystemType system_type) {
            if (m_systemTypeInfo.find(system_type) != m_systemTypeInfo.end())
                return m_systemTypeInfo.at(system_type).first;
            return getIntValue(SystemType::INVALID_SYSTEM);
        }

        /// <summary>
        /// Gets the printable string value of a tracking system type
        /// </summary>
        /// <param name="system_type">Tracking system type</param>
        /// <returns>Printable string of the tracking system type</returns>
        static std::string getStringValue(SystemType system_type) {
            if (m_systemTypeInfo.find(system_type) != m_systemTypeInfo.end())
                return m_systemTypeInfo.at(system_type).second;
            return getStringValue(SystemType::INVALID_SYSTEM);
        }

        /// <summary>
        /// Gets the tracking system type from its integer representation
        /// </summary>
        /// <param name="value">Integer value of the tracking system type</param>
        /// <returns>The tracking system type if the integer is valid, SystemType::INVALID_SYSTEM otherwise</returns>
        static SystemType getSystemType(int value) {
            for (const auto& systemType : m_systemTypeInfo) {
                if (systemType.second.first == value)
                    return systemType.first;
            }
            return SystemType::INVALID_SYSTEM;
        }

        /// <summary>
        /// Constructs an instance with INVALID_SYSTEM as the default system type
        /// </summary>
        TrackingSystemType() = default;

        /// <summary>
        /// Constructs an instance from the supplied int. 
        /// If the supplied value isn't a valid tracking system type, it will return with SystemType::INVALID_SYSTEM
        /// </summary>
        /// <param name="system_type">The type of tracking system</param>
        TrackingSystemType(int system_type){
            this->m_systemType = TrackingSystemType::getSystemType(system_type);
        }

        /// <summary>
        /// Constructs an instance from the supplied system type 
        /// </summary>
        /// <param name="system_type">The type of tracking system</param>
        TrackingSystemType(SystemType system_type) {
            this->m_systemType = system_type;
        }

        /// <summary>
        /// Gets the stored tracking reference type
        /// </summary>
        /// <returns>The tracking reference type</returns>
        SystemType getSystemType() const {
            return this->m_systemType;
        }

        /// <summary>
        /// Gets the integer representation of the stored tracking reference type
        /// </summary>
        /// <returns>Integer representation of the tracking reference type</returns>
        int getIntValue() const {
            return TrackingSystemType::getIntValue(this->m_systemType);
        }

        /// <summary>
        /// Gets the printable string representation of the stored tracking reference type
        /// </summary>
        /// <returns>Printable string representation of the tracking reference type</returns>
        std::string getStringValue() const {
            return TrackingSystemType::getStringValue(this->m_systemType);
        }
        
    private:
        /// <summary>
        /// Storage for tracking system type
        /// </summary>
        SystemType m_systemType = SystemType::INVALID_SYSTEM;
    };
}