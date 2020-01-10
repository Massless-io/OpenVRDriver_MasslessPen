/* Copyright (C) 2020 Massless Corp. - All Rights Reserved
 * Author: Jacob Hilton.
 * You may use, distribute and modify this code under the
 * terms of the BSD 3-Clause "New" or "Revised" License.
 *
 * You should have received a copy of this license with
 * this file. If not, please email support@massless.io
 */

#pragma once
#include <vector>
#include <openvr_driver.h>

/// <summary>
/// Interface for devices that will be updated every frame
/// </summary>
class IUpdatable {
public:
    /// <summary>
    /// Called to update the state of this device.
    /// </summary>
    virtual void update(std::vector<vr::VREvent_t> events) = 0;

    virtual ~IUpdatable() = default;
};