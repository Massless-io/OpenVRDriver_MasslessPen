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

class Handedness {
public:
    enum HandednessType {
        UNKNOWN = 0,
        LEFT = 1,
        RIGHT = 2
    };

    Handedness() = default;

    Handedness(HandednessType handedness_type) {
        this->m_handednessType = handedness_type;
    }

    Handedness(std::string handedness_type) {
        if (handedness_type == std::string("left")) {
            this->m_handednessType = HandednessType::LEFT;
        }
        else if (handedness_type == std::string("right")) {
            this->m_handednessType = HandednessType::RIGHT;
        }
        else {
            this->m_handednessType = HandednessType::UNKNOWN;
        }
    }

    bool operator==(const HandednessType& other) const {
        return other == this->m_handednessType;
    }

    bool operator==(const Handedness& other) const {
        return other.m_handednessType == this->m_handednessType;
    }

    HandednessType value() const {
        return this->m_handednessType;
    }

    std::string stringValue() const {
        switch (this->m_handednessType) {
        case HandednessType::UNKNOWN:
            return "unknown";
        case HandednessType::LEFT:
            return "left";
        case HandednessType::RIGHT:
            return "right";
        }
        return "";
    }

private:

    HandednessType m_handednessType = UNKNOWN;
};
