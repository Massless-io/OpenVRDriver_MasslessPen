/* Copyright (C) 2020 Massless Corp. - All Rights Reserved
 * Author: Jacob Hilton.
 * You may use, distribute and modify this code under the
 * terms of the BSD 3-Clause "New" or "Revised" License.
 *
 * You should have received a copy of this license with
 * this file. If not, please email support@massless.io
 */

#include "FileSettingsLoader.hpp"

#include "DriverLog.hpp"

FileSettingsLoader::FileSettingsLoader(std::filesystem::path config_path)
{
    nlohmann::json default_json = this->toJson(DriverSettings::make_default());
    try {
        // Try to make file if it doesnt exist with default settings
        if (!std::filesystem::exists(config_path)) {
            std::ofstream config_file_stream(config_path);
            config_file_stream << default_json.dump(4, ' ', true);
        }
    }
    catch (const std::system_error& e) {
        throw DriverSettingsException(e.code().value(), e.what());
    }
    catch (const std::exception& e) {
        throw DriverSettingsException(0, e.what());
    }

    // Delay opening ofstream until destructor
    this->m_isRealFilestream = true;
    this->m_filePath = config_path;
    DriverLog("Loading settings from file: %s \n", this->m_filePath.string().c_str());
}


FileSettingsLoader::FileSettingsLoader(std::unique_ptr<std::istream> read_stream, std::unique_ptr<std::ostream> write_stream) :
    m_inStream(std::move(read_stream)),
    m_outStream(std::move(write_stream))
{
}


DriverSettings FileSettingsLoader::readSettings()
{
    using json = nlohmann::json;
    json input_json;
    try {
//#ifndef __INTELLISENSE__  // Intellisense errors on nlohmann::json::parse, but compiles fine and is within spec https://github.com/Microsoft/vscode-cpptools/issues/1901
        if (this->m_isRealFilestream) {
            input_json = nlohmann::json::parse(std::ifstream(this->m_filePath));
        }
        else {
            this->m_inStream->seekg(0);
            input_json = nlohmann::json::parse(*this->m_inStream);
        }
//#endif __INTELLISENSE__
    }
    catch (const std::exception & e) {
        throw DriverSettingsException(0, e.what());
    }

    DriverSettings settings = this->fromJson(input_json);

    return settings;
}

void FileSettingsLoader::writeSettings(DriverSettings settings)
{

    try {
        if (this->m_isRealFilestream) {
            this->m_outStream = std::make_unique<std::ofstream>(this->m_filePath, std::fstream::trunc);
        }
        this->m_outStream->seekp(0);
        std::string json_string = this->toJson(settings).dump(4, ' ', true);
        (*this->m_outStream) << json_string;

        if (this->m_isRealFilestream) {
            this->m_outStream.reset();
        }
    }
    catch (const std::exception&) {/* Failed to write settings */ }
}
DriverSettings FileSettingsLoader::fromJson(nlohmann::json input_json) const {
    DriverSettings settings = DriverSettings::make_default();
    using json = nlohmann::json;
    auto json_to_opt = [&](DriverSettings::Setting setting) -> std::optional<json> {
        if (input_json.find(DriverSettings::getKeyString(setting)) != input_json.end()) {
            return input_json[DriverSettings::getKeyString(setting)];
        }
        return std::nullopt;
    };

    auto load_setting = [&](DriverSettings::Setting setting, std::function<bool(json)> predicate, std::function<DriverSettings::SettingValue(json)> getter) {
        auto settingValue = json_to_opt(setting);
        if (settingValue.has_value()) {
            if (predicate(*settingValue)) {
                settings.setValue<DriverSettings::SettingValue>(setting, getter(*settingValue));
            }
            else {
                settings.setValue<DriverSettings::SettingValue>(setting, DriverSettings::invalid_setting_t());
            }
        }
    };

    load_setting(DriverSettings::AttachGizmo, [](json j) -> bool {return j.is_boolean(); }, [](json j) -> DriverSettings::SettingValue { return j.get<bool>(); });
    load_setting(DriverSettings::EnableDetailedLogging, [](json j) -> bool {return j.is_boolean(); }, [](json j) -> DriverSettings::SettingValue {return j.get<bool>(); });
    load_setting(DriverSettings::Handedness, [](json j) -> bool {return j.is_string(); }, [](json j) -> DriverSettings::SettingValue {return j.get<std::string>(); });
    load_setting(DriverSettings::AutoTrackingRefSerial, [](json j) -> bool {return j.is_string(); }, [](json j) -> DriverSettings::SettingValue {return j.get<std::string>(); });
    load_setting(DriverSettings::ForcedTrackingRefSerial, [](json j) -> bool {return j.is_string(); }, [](json j) -> DriverSettings::SettingValue {return j.get<std::string>(); });

    return settings;
}

nlohmann::json FileSettingsLoader::toJson(DriverSettings settings) const
{
    nlohmann::json json;
 
    if (settings.isValid(DriverSettings::AttachGizmo) && settings.getValue<bool>(DriverSettings::AttachGizmo).has_value()) {
        json[DriverSettings::getKeyString(DriverSettings::AttachGizmo)] = *settings.getValue<bool>(DriverSettings::AttachGizmo);
    }
    if (settings.isValid(DriverSettings::EnableDetailedLogging) && settings.getValue<bool>(DriverSettings::EnableDetailedLogging).has_value()) {
        json[DriverSettings::getKeyString(DriverSettings::EnableDetailedLogging)] = *settings.getValue<bool>(DriverSettings::EnableDetailedLogging);
    }
    if (settings.isValid(DriverSettings::Handedness) && settings.getValue<std::string>(DriverSettings::Handedness).has_value()) {
        json[DriverSettings::getKeyString(DriverSettings::Handedness)] = *settings.getValue<std::string>(DriverSettings::Handedness);
    }
    if (settings.isValid(DriverSettings::AutoTrackingRefSerial) && settings.getValue<std::string>(DriverSettings::AutoTrackingRefSerial).has_value()) {
        json[DriverSettings::getKeyString(DriverSettings::AutoTrackingRefSerial)] = *settings.getValue<std::string>(DriverSettings::AutoTrackingRefSerial);
    }
    if (settings.isValid(DriverSettings::ForcedTrackingRefSerial) && settings.getValue<std::string>(DriverSettings::ForcedTrackingRefSerial).has_value()) {
        json[DriverSettings::getKeyString(DriverSettings::ForcedTrackingRefSerial)] = *settings.getValue<std::string>(DriverSettings::ForcedTrackingRefSerial);
    }

    return json;
}

