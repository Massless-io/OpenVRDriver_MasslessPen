/* Copyright (C) 2020 Massless Corp. - All Rights Reserved
 * Author: Jacob Hilton.
 * You may use, distribute and modify this code under the
 * terms of the BSD 3-Clause "New" or "Revised" License.
 *
 * You should have received a copy of this license with
 * this file. If not, please email support@massless.io
 */

#pragma once

#include <map>
#include <algorithm>
#include <string>
#include <cstdint>
#include <optional>
#include <variant>
#include <type_traits>

// https://stackoverflow.com/questions/52303316/get-index-by-type-in-stdvariant
/// <summary>
/// Gets the index of a type in a variant at compile time
/// </summary>
template<typename VariantType, typename T, std::size_t index = 0>
constexpr std::size_t variant_index() {
    if constexpr (index == std::variant_size_v<VariantType>) {
        return index;
    }
    else if constexpr (std::is_same_v<std::variant_alternative_t<index, VariantType>, T>) {
        return index;
    }
    else {
        return variant_index<VariantType, T, index + 1>();
    }
}

/// <summary>
/// Class to hold parsed driver settings
/// </summary>
class DriverSettings {
public:
    /// <summary>
    /// Empty struct to signify an invalid setting value
    /// </summary>
    struct invalid_setting_t {
        // To ensure operator== on DriverSettings works
        bool operator==(const invalid_setting_t& other) const { return true; }
    };

    /// <summary>
    /// Type for holding a setting valid
    /// </summary>
    using SettingValue = std::variant<invalid_setting_t, std::string, int32_t, float, bool>;
    
    /// <summary>
    /// Type for holding a setting key
    /// </summary>
    using SettingKey = std::string;

    DriverSettings() = default;

    /// <summary>
    /// Constructs a DriverSettings instance with default values set
    /// </summary>
    /// <returns>DriverSettings struct with Handedness, AttachGizmo, and EnableDetailedLogging set</returns>
    static DriverSettings make_default() {
        DriverSettings settings;
        settings.setValue<std::string>(Handedness, "right");
        settings.setValue<bool>(AttachGizmo, false);
        settings.setValue<bool>(EnableDetailedLogging, false);
        return settings;
    }

    /// <summary>
    /// Setting key enum
    /// </summary>
    enum Setting {
        AutoTrackingRefSerial,
        ForcedTrackingRefSerial,
        Handedness,
        AttachGizmo,
        EnableDetailedLogging
    };

    /// <summary>
    /// Gets the value of a setting key
    /// </summary>
    /// <param name="key">The setting key</param>
    /// <returns>Value or nullopt if the value isn't stored, is the wrong type, or is invalid_setting_t</returns>
    template<typename T>
    std::optional<T> getValue(Setting key) const{
        if (this->m_values.count(key) == 0)
            return std::nullopt;
        if (!isValid(key))
            return std::nullopt;
        if (variant_index<SettingValue, T>() != this->m_values.at(key).index())
            return std::nullopt;
        return std::get<T>(this->m_values.at(key));
    }

    /// <summary>
    /// Gets the value of a setting key as the plain variant
    /// </summary>
    /// <param name="key">The setting key</param>
    /// <returns>The variant or nullopt if the value isn't stored</returns>
    template <>
    std::optional<SettingValue> getValue(Setting key) const{
        if (this->m_values.count(key) == 0)
            return std::nullopt;
        return this->m_values.at(key);
    }

    /// <summary>
    /// Sets the value of a setting
    /// </summary>
    /// <param name="key">The setting key</param>
    /// <param name="value">The setting value</param>
    template<typename T>
    void setValue(Setting key, typename std::common_type<T>::type value) {
        this->m_values[key] = SettingValue(value);
    }

    /// <summary>
    /// Removes a setting value
    /// </summary>
    /// <param name="key">The setting key</param>
    void clearValue(Setting key) {
        this->m_values.erase(key);
    }

    /// <summary>
    /// Checks if the value of a key is not invalid_setting_t
    /// Will return true if the value is not set
    /// </summary>
    /// <param name="key">The setting key</param>
    /// <returns>True if value is not invalid_setting_t</returns>
    bool isValid(Setting key) const{
        auto value = this->getValue<SettingValue>(key);
        if (!value.has_value())
            return true;
        if (value->index() == variant_index<SettingValue, invalid_setting_t>())
            return false;
        return true;
    }

    /// <summary>
    /// Checks if all values that are currently set are not invalid_setting_t
    /// </summary>
    /// <returns>True if none are invalid_setting_t</returns>
    bool areAllValid() const{
        return std::all_of(m_keys.begin(), m_keys.end(), [&](auto& key) {return this->isValid(key.first); });
    }

    /// <summary>
    /// Gets the string representation of a setting enum value
    /// </summary>
    /// <param name="setting">The setting enum value</param>
    /// <returns>string representation of a setting enum value</returns>
    static SettingKey getKeyString(Setting setting) {
        return DriverSettings::m_keys.at(setting);
    }

    bool operator==(const DriverSettings& other) const {
        
        // Check the sizes are the same
        bool sizeEqual = this->m_values.size() == other.m_values.size();
        if (!sizeEqual)
            return false;
            
        for (auto it = m_keys.begin(); it != m_keys.end(); ++it) {
            bool bothSet = this->getValue<SettingValue>(it->first).has_value() && other.getValue<SettingValue>(it->first).has_value();
            bool bothUnset = !this->getValue<SettingValue>(it->first).has_value() && !other.getValue<SettingValue>(it->first).has_value();
            // If set, check equality between values
            if (bothSet) {
                SettingValue v1 = this->getValue<SettingValue>(it->first).value();
                SettingValue v2 = other.getValue<SettingValue>(it->first).value();
                bool bothEqual = v1 == v2;
                if (bothEqual)
                    continue;
            } // If unset, return true
            else if (bothUnset) {
                continue;
            } // Else they are neither both set or unset, hence unequal
            else {
                return false;
            }
        }
        return true;
    }

    static const std::map<Setting, SettingKey>& getKeys() {
        return m_keys;
    }

private:
    /// <summary>
    /// Storage for converstions between enum to string
    /// </summary>
    static const std::map<Setting, SettingKey> m_keys;

    /// <summary>
    /// Storage for actual setting values
    /// </summary>
    std::map<Setting, SettingValue> m_values;
};