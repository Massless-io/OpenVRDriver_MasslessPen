/* Copyright (C) 2020 Massless Corp. - All Rights Reserved
 * Author: Jacob Hilton.
 * You may use, distribute and modify this code under the
 * terms of the BSD 3-Clause "New" or "Revised" License.
 *
 * You should have received a copy of this license with
 * this file. If not, please email support@massless.io
 */

#include "MockVRSettings.hpp"

const char* MockVRSettings::GetSettingsErrorNameFromEnum(vr::EVRSettingsError eError) {
    return nullptr;
}
bool MockVRSettings::Sync(bool bForce, vr::EVRSettingsError* peError) {
    return false;
}
void MockVRSettings::SetBool(const char* pchSection, const char* pchSettingsKey, bool bValue, vr::EVRSettingsError* peError) {
    settings_map[pchSection][pchSettingsKey] = bValue;
}
void MockVRSettings::SetInt32(const char* pchSection, const char* pchSettingsKey, int32_t nValue, vr::EVRSettingsError* peError) {
    settings_map[pchSection][pchSettingsKey] = nValue;
}
void MockVRSettings::SetFloat(const char* pchSection, const char* pchSettingsKey, float flValue, vr::EVRSettingsError* peError) {
    settings_map[pchSection][pchSettingsKey] = flValue;
}
void MockVRSettings::SetString(const char* pchSection, const char* pchSettingsKey, const char* pchValue, vr::EVRSettingsError* peError) {
    settings_map[pchSection][pchSettingsKey] = pchValue;
}
bool MockVRSettings::GetBool(const char* pchSection, const char* pchSettingsKey, vr::EVRSettingsError* peError) {
    try {
        return std::any_cast<bool>(settings_map[pchSection][pchSettingsKey]);
    }
    catch (const std::bad_any_cast&) {
        return false;
    }
}
int32_t MockVRSettings::GetInt32(const char* pchSection, const char* pchSettingsKey, vr::EVRSettingsError* peError) {
    try {
        return std::any_cast<int32_t>(settings_map[pchSection][pchSettingsKey]);
    }
    catch (const std::bad_any_cast&) {
        return 0;
    }
}
float MockVRSettings::GetFloat(const char* pchSection, const char* pchSettingsKey, vr::EVRSettingsError* peError) {
    try {
        return std::any_cast<float>(settings_map[pchSection][pchSettingsKey]);
    }
    catch (const std::bad_any_cast&) {
        return 0;
    }
}
void MockVRSettings::GetString(const char* pchSection, const char* pchSettingsKey, VR_OUT_STRING() char* pchValue, uint32_t unValueLen, vr::EVRSettingsError* peError) {
    try {
        const char* strVal = std::any_cast<const char*>(settings_map[pchSection][pchSettingsKey]);
        std::strncpy(pchValue, strVal, unValueLen);
    }
    catch (const std::bad_any_cast&) {
        return;
    }
}
void MockVRSettings::RemoveSection(const char* pchSection, vr::EVRSettingsError* peError) {

}
void MockVRSettings::RemoveKeyInSection(const char* pchSection, const char* pchSettingsKey, vr::EVRSettingsError* peError) {

}
