/* Copyright (C) 2020 Massless Corp. - All Rights Reserved
 * Author: Jacob Hilton.
 * You may use, distribute and modify this code under the
 * terms of the BSD 3-Clause "New" or "Revised" License.
 *
 * You should have received a copy of this license with
 * this file. If not, please email support@massless.io
 */

#pragma once
#include <cstdint>
#include <optional>
#include <vector>
#include <MasslessCallbackEvents.h>

namespace MasslessInterface {
    class PenEvent {
    public:
        PenEvent(uint16_t event_type, std::optional<std::shared_ptr<Massless::Events::BaseEvent>> event_struct = std::nullopt) 
            :m_eventType(event_type), 
            m_eventStruct(event_struct){}
        const uint16_t m_eventType;
        const std::optional<std::shared_ptr<Massless::Events::BaseEvent>> m_eventStruct;
    };
};