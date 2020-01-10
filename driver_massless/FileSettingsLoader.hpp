/* Copyright (C) 2020 Massless Corp. - All Rights Reserved
 * Author: Jacob Hilton.
 * You may use, distribute and modify this code under the
 * terms of the BSD 3-Clause "New" or "Revised" License.
 *
 * You should have received a copy of this license with
 * this file. If not, please email support@massless.io
 */

#pragma once

#include <filesystem>
#include <iostream>
#include <memory>

#include <json.hpp>

#include <IDriverSettingsLoader.hpp>
#include <SettingsUtilities.hpp>

/// <summary>
/// DriverSettings implementation that uses files or streams to load and save Settings
/// </summary>
class FileSettingsLoader : public IDriverSettingsLoader
{
public:

    /// <summary>
    /// Constructs an instance using the file at the specified config_path
    /// </summary>
    /// <param name="config_path">Path to the config file</param>
    /// <exception cref="DriverSettingsException">When the file cannot be created, or the file cannot be opened for reading</exception>
    FileSettingsLoader(std::filesystem::path config_path);

    /// <summary>
    /// Constructs and instance using the read_stream to read the settings in, and write_stream to write settings out
    /// </summary>
    /// <param name="read_stream">Pointer to the stream to read settings</param>
    /// <param name="write_stream">Pointer to the stream to write settings</param>
    /// <exception cref="DriverSettingsException">When the read stream cannot be opened for reading</exception>
    FileSettingsLoader(std::unique_ptr<std::istream> read_stream, std::unique_ptr<std::ostream> write_stream);

    /// <summary>
    /// Converts a DriverSettings object to its json representation
    /// Anything nullopt will not be set
    /// </summary>
    /// <param name="settings">The DriverSettings instance</param>
    /// <returns>A filled json object with set values</returns>
    nlohmann::json toJson(DriverSettings settings) const;

    DriverSettings fromJson(nlohmann::json input_json) const;

    // Inherited via IDriverSettingsLoader

    virtual DriverSettings readSettings() override;

    virtual void writeSettings(DriverSettings settings) override;

private:

    /// <summary>
    /// Stored settings input stream.
    /// </summary>
    std::unique_ptr<std::istream> m_inStream;
    
    /// <summary>
    /// Stored settings output stream.
    /// If this is a filestream this will be null until ~FileSettingsLoader is called.
    /// </summary>
    std::unique_ptr<std::ostream> m_outStream;

    /// <summary>
    /// True if this is acting on a real filestream
    /// </summary>
    bool m_isRealFilestream = false;

    /// <summary>
    /// Path of the file. 
    /// Will be blank if the istream ostream constructor was called
    /// </summary>
    std::filesystem::path m_filePath;
};

