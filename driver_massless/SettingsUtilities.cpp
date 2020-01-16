/* Copyright (C) 2020 Massless Corp. - All Rights Reserved
 * Author: Jacob Hilton.
 * You may use, distribute and modify this code under the
 * terms of the BSD 3-Clause "New" or "Revised" License.
 *
 * You should have received a copy of this license with
 * this file. If not, please email support@massless.io
 */

#include "SettingsUtilities.hpp"
#include <MasslessOpenVRIntegrationUUIDs.h>
#include <shlobj.h>

const std::array<uint8_t, SettingsUtilities::m_integrationKeyLen> SettingsUtilities::m_defaultIntegrationKey = M_INTEGRATION_OPENVRDEV;

const std::vector<std::pair<std::string, std::array<uint8_t, SettingsUtilities::m_integrationKeyLen>>> SettingsUtilities::m_integrationKeys = {
    {"steamtours", M_INTEGRATION_OPENVR_STEAMVRHOME},
    {"tiltbrush", M_INTEGRATION_OPENVR_TILTBRUSH},
    {"SculptrVR-Win64-Shipping", M_INTEGRATION_OPENVR_SCULPTVR},
    {"Masterpiece_VR", M_INTEGRATION_OPENVR_MASTERPIECEVR},
    {"Sketch-Win64-Shipping", M_INTEGRATION_OPENVR_SKETCHBOX},
    {"GravitySketchVR", M_INTEGRATION_OPENVR_GRAVITYSKETCH}
};


std::array<uint8_t, SettingsUtilities::m_integrationKeyLen> SettingsUtilities::parseIntegrationKey(std::string string_value) {
    // Check the string is the correct length
    if (string_value.length() != m_integrationKeyLen) {
        throw std::invalid_argument("String needs to be " + std::to_string(static_cast<int>(m_integrationKeyLen)) + " characters long (was " + std::to_string(string_value.length()) + ").");
    }

    // Setup return
    std::array<uint8_t, m_integrationKeyLen> out{ 0 };

    // For each uint value
    for (std::size_t i = 0; i < m_integrationKeyLen; i++) {

        // Get a substring representing the hex value, ie "5A2B33" becomes "5A", "2B", "33"
        std::string byte_str(string_value.begin() + 2 * i, string_value.begin() + 2 * (i + 1));

        // Convert the "5A" to a long as a hex value, and cast to uint8_t
        out[i] = static_cast<uint8_t>(std::strtol(byte_str.c_str(), nullptr, 16));
    }
    return out;
};

std::array<uint8_t, SettingsUtilities::m_integrationKeyLen> SettingsUtilities::getDefaultIntegrationKey() noexcept {
    return SettingsUtilities::m_defaultIntegrationKey;
}

std::filesystem::path SettingsUtilities::getMasslessConfigPath()
{
    std::string appdata;
    char* buf = nullptr;
    size_t sz = 0;
    if (_dupenv_s(&buf, &sz, "appdata") == 0 && buf != nullptr)
    {
        appdata = std::string(buf);
        free(buf);
    }
    else {
        free(buf);
        throw DriverSettingsException(0, "Failed to load the \"appdata\" environment variable");
    }

    std::string config_folder = appdata + "\\Massless\\";

    std::filesystem::path folder_path(config_folder);
    std::filesystem::path file_path(config_folder + "\\driver_massless.json");

    // Create folder if it doesnt exist
    if (!std::filesystem::exists(folder_path)) {
        if (!std::filesystem::create_directory(folder_path)) {
            throw DriverSettingsException(0, "Failed to create the config directory at: " + folder_path.string());
        }
    }

    return file_path;
}

std::optional<std::array<uint8_t, SettingsUtilities::m_integrationKeyLen>> SettingsUtilities::getIntegrationKeyForApp(std::string exeName)
{
    for (auto key : SettingsUtilities::m_integrationKeys) {
        if (std::regex_search(exeName, std::regex(key.first, std::regex_constants::ECMAScript | std::regex_constants::icase))) {
            return key.second;
        }
    }
    return std::nullopt;
}
