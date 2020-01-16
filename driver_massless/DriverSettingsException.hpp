/* Copyright (C) 2020 Massless Corp. - All Rights Reserved
 * Author: Jacob Hilton.
 * You may use, distribute and modify this code under the
 * terms of the BSD 3-Clause "New" or "Revised" License.
 *
 * You should have received a copy of this license with
 * this file. If not, please email support@massless.io
 */

#pragma once

#include <system_error>

/// <summary>
/// Exception to be thrown within IDriverSettingsLoader implementations
/// </summary>
class DriverSettingsException : public std::system_error {
public:

    /// <summary>
    /// Constructs a DriverSettingsException with the specified parameters
    /// </summary>
    /// <param name="code">Error code, IDriverSettingsLoader implementation specific</param>
    /// <param name="message">Message to be passed to the handler</param>
    DriverSettingsException(int code, std::string message = "Generic DriverSettingsException") : std::system_error(code, std::generic_category(), message) {}
};