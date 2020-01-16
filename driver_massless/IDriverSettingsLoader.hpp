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

#include <DriverSettings.hpp>

/// <summary>
/// Defines an interface for driver settings loaders to implement
/// </summary>
class IDriverSettingsLoader {
public:
    IDriverSettingsLoader() = default;
    virtual ~IDriverSettingsLoader() = default;

    /// <summary>
    /// Loads from the underlying storage mechanism the relevant settings and returns a filled DriverSettings struct
    /// </summary>
    virtual DriverSettings readSettings() = 0;

    /// <summary>
    /// Writes the set values of the given in the DriverSettings struct to the underlying storage mechanism
    /// </summary>
    /// <param name="settings">Filled settings struct</param>
    /// <exception cref="DriverSettingsException">When the settings file could not be written</exception>
    virtual void writeSettings(DriverSettings settings) = 0;
};
