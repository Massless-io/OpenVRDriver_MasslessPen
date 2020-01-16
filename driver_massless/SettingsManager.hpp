/* Copyright (C) 2020 Massless Corp. - All Rights Reserved
 * Author: Jacob Hilton.
 * You may use, distribute and modify this code under the
 * terms of the BSD 3-Clause "New" or "Revised" License.
 *
 * You should have received a copy of this license with
 * this file. If not, please email support@massless.io
 */

#pragma once

#include <memory>
#include <variant>

#include <IDriverSettingsLoader.hpp>

/// <summary>
/// Class for managing access to the driver settings
/// Construction will load the settings from the given settings loader, and destruction will save the settings
/// </summary>
class SettingsManager
{
public:
    /// <summary>
    /// Constructs a new SettingsManager with the given settings loader
    /// Will attempt to load settings on startup
    /// </summary>
    /// <param name="settings_loader">Settings loader to use to load and save driver settings</param>
    SettingsManager(std::unique_ptr<IDriverSettingsLoader> settings_loader);

    ~SettingsManager();

    /// <summary>
    /// Gets the settings instance
    /// </summary>
    /// <returns></returns>
    DriverSettings& getSettings();

    void storeSettings();

private:
    DriverSettings m_driverSettings;
    std::unique_ptr<IDriverSettingsLoader> m_driverSettingsLoader;
};

