/* Copyright (C) 2020 Massless Corp. - All Rights Reserved
 * Author: Jacob Hilton.
 * You may use, distribute and modify this code under the
 * terms of the BSD 3-Clause "New" or "Revised" License.
 *
 * You should have received a copy of this license with
 * this file. If not, please email support@massless.io
 */

#pragma once
#include <openvr_driver.h>
#include <DriverSettings.hpp>
#include <any>
#include <map>
#include <string>


class MockVRSettings : public vr::IVRSettings {
public:
    // Inherited via IVRSettings
    virtual const char* vr::IVRSettings::GetSettingsErrorNameFromEnum(vr::EVRSettingsError eError);
    virtual bool vr::IVRSettings::Sync(bool bForce = false, vr::EVRSettingsError* peError = nullptr);
    virtual void vr::IVRSettings::SetBool(const char* pchSection, const char* pchSettingsKey, bool bValue, vr::EVRSettingsError* peError = nullptr);
    virtual void vr::IVRSettings::SetInt32(const char* pchSection, const char* pchSettingsKey, int32_t nValue, vr::EVRSettingsError* peError = nullptr);
    virtual void vr::IVRSettings::SetFloat(const char* pchSection, const char* pchSettingsKey, float flValue, vr::EVRSettingsError* peError = nullptr);
    virtual void vr::IVRSettings::SetString(const char* pchSection, const char* pchSettingsKey, const char* pchValue, vr::EVRSettingsError* peError = nullptr);
    virtual bool vr::IVRSettings::GetBool(const char* pchSection, const char* pchSettingsKey, vr::EVRSettingsError* peError = nullptr);
    virtual int32_t vr::IVRSettings::GetInt32(const char* pchSection, const char* pchSettingsKey, vr::EVRSettingsError* peError = nullptr);
    virtual float vr::IVRSettings::GetFloat(const char* pchSection, const char* pchSettingsKey, vr::EVRSettingsError* peError = nullptr);
    virtual void vr::IVRSettings::GetString(const char* pchSection, const char* pchSettingsKey, VR_OUT_STRING() char* pchValue, uint32_t unValueLen, vr::EVRSettingsError* peError = nullptr);
    virtual void vr::IVRSettings::RemoveSection(const char* pchSection, vr::EVRSettingsError* peError = nullptr);
    virtual void vr::IVRSettings::RemoveKeyInSection(const char* pchSection, const char* pchSettingsKey, vr::EVRSettingsError* peError = nullptr);

    std::map<std::string, std::map<std::string, std::any>> settings_map;
};