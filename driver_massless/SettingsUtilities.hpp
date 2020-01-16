/* Copyright (C) 2020 Massless Corp. - All Rights Reserved
 * Author: Jacob Hilton.
 * You may use, distribute and modify this code under the
 * terms of the BSD 3-Clause "New" or "Revised" License.
 *
 * You should have received a copy of this license with
 * this file. If not, please email support@massless.io
 */

#pragma once

#include <regex>
#include <string>
#include <array>
#include <filesystem>
#include <fstream>
#include <optional>

#include <Handedness.hpp>
#include <DriverSettingsException.hpp>

class SettingsUtilities {
public:
    /// <summary>
    /// Integration key length
    /// </summary>
    static const constexpr std::size_t m_integrationKeyLen = 16;

    /// <summary>
    /// Parses a raw string integration key into an array to be passed to the Massless C API
    /// </summary>
    /// <param name="string_value">String representation of the integration key</param>
    /// <exception cref="std::invalid_argument">When the input string is not the correct length (m_integrationKeyLen)</exception>
    /// <returns>Parsed array ready to be passed to the Massless C API</returns>
    static std::array<uint8_t, m_integrationKeyLen> parseIntegrationKey(std::string string_value);

    /// <summary>
    /// Gets the default SteamVR Massless integration key
    /// </summary>
    /// <returns>Default SteamVR Massless integration key</returns>
    static std::array<uint8_t, m_integrationKeyLen> getDefaultIntegrationKey() noexcept;

    /// <summary>
    /// Tries to open an fstream to the driver config file
    /// </summary>
    /// <exception cref="DriverSettingsException">When Massless folder or driver_massless.json are unable to be created</exception>
    /// <returns></returns>
    static std::filesystem::path getMasslessConfigPath();

    /// <summary>
    /// Gets the integration key for an application by its executable name
    /// </summary>
    /// <param name="exeName">Name of the exe file</param>
    /// <returns>The integration key for the application, or nullopt if the app doesn't have a key assigned</returns>
    static std::optional<std::array<uint8_t, m_integrationKeyLen>> getIntegrationKeyForApp(std::string exeName);
        
private:
    // Prevent instantiation
    SettingsUtilities() = default;

    /// <summary>
    /// The default SteamVR integration key
    /// </summary>
    static const std::array<uint8_t, m_integrationKeyLen> m_defaultIntegrationKey;

    static const std::vector<std::pair<std::string,std::array<uint8_t, m_integrationKeyLen>>> m_integrationKeys;
};