/* Copyright (C) 2020 Massless Corp. - All Rights Reserved
 * Author: Jacob Hilton.
 * You may use, distribute and modify this code under the
 * terms of the BSD 3-Clause "New" or "Revised" License.
 *
 * You should have received a copy of this license with
 * this file. If not, please email support@massless.io
 */

#include "SettingsManager.hpp"

SettingsManager::SettingsManager(std::unique_ptr<IDriverSettingsLoader> settings_loader) :
    m_driverSettingsLoader(std::move(settings_loader))
{
    m_driverSettings = m_driverSettingsLoader->readSettings();
}

SettingsManager::~SettingsManager()
{
    if(this->m_driverSettings.areAllValid())
        this->m_driverSettingsLoader->writeSettings(this->m_driverSettings);
}

DriverSettings& SettingsManager::getSettings()
{
    return m_driverSettings;
}

void SettingsManager::storeSettings()
{
    m_driverSettingsLoader->writeSettings(m_driverSettings);
}
