/* Copyright (C) 2020 Massless Corp. - All Rights Reserved
 * Author: Jacob Hilton.
 * You may use, distribute and modify this code under the
 * terms of the BSD 3-Clause "New" or "Revised" License.
 *
 * You should have received a copy of this license with
 * this file. If not, please email support@massless.io
 */

#pragma once
#include <openvr_driver.h>
#include <IUpdatable.hpp>

/// <summary>
/// Interface for devices this driver exposes
/// </summary>
class IDriverDevice : public vr::ITrackedDeviceServerDriver, public IUpdatable {
public:
    virtual vr::TrackedDeviceIndex_t getIndex() = 0;
    virtual ~IDriverDevice() = default;
};