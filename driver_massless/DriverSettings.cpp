/* Copyright (C) 2020 Massless Corp. - All Rights Reserved
 * Author: Jacob Hilton.
 * You may use, distribute and modify this code under the
 * terms of the BSD 3-Clause "New" or "Revised" License.
 *
 * You should have received a copy of this license with
 * this file. If not, please email support@massless.io
 */

#include "DriverSettings.hpp"

const std::map<DriverSettings::Setting, DriverSettings::SettingKey> DriverSettings::m_keys = std::map<DriverSettings::Setting, DriverSettings::SettingKey>({
    { AutoTrackingRefSerial, "auto_tracking_reference_serial" },
    { ForcedTrackingRefSerial, "forced_tracking_reference_serial" },
    { Handedness, "pen_handedness"},
    { AttachGizmo, "attach_gizmo"},
    { EnableDetailedLogging, "enable_detailed_logging"}
});