/* Copyright (C) 2020 Massless Corp. - All Rights Reserved
 * Author: Jacob Hilton.
 * You may use, distribute and modify this code under the
 * terms of the BSD 3-Clause "New" or "Revised" License.
 *
 * You should have received a copy of this license with
 * this file. If not, please email support@massless.io
 */

#pragma once
#include <string>
#include <cstdint>
#include <optional>

namespace MasslessInterface {
    class PenNotification {
    public:
        //TODO: Verify these values
        enum NotificationType:uint8_t {
            NONE = 255,
            ERROR = 3,
            WARNING = 2,
            MESSAGE = 1
        };

        PenNotification(NotificationType type = NONE, std::optional<uint16_t> code = std::nullopt, std::string message = "") :
            m_notificationType(type),
            m_notificationCode(code),
            m_notificationMessage(message) 
        {
        }

        /// <summary>
        /// The type of notification this is
        /// </summary>
        NotificationType m_notificationType;

        /// <summary>
        /// The optional notification code
        /// </summary>
        std::optional<uint16_t> m_notificationCode;

        /// <summary>
        /// A message that this notification holds
        /// </summary>
        std::string m_notificationMessage;
    };
};