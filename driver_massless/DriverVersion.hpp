/* Copyright (C) 2020 Massless Corp. - All Rights Reserved
 * Author: Jacob Hilton.
 * You may use, distribute and modify this code under the
 * terms of the BSD 3-Clause "New" or "Revised" License.
 *
 * You should have received a copy of this license with
 * this file. If not, please email support@massless.io
 */

#pragma once
#include <semver.hpp>
#include <MasslessPenSystem.h>
#include <tuple>

struct DriverVersion {
    constexpr const static semver::version m_driverVersion = { 0, 7, 10 };

    constexpr const static semver::version m_dllVersion = { dllVersionMajor, dllVersionMinor, dllVersionPatch };

    template <typename T>
    constexpr static semver::version asSemVer(std::tuple<T, T, T> versionTuple) {
        return semver::version{ static_cast<uint8_t>(std::get<0>(versionTuple)), static_cast<uint8_t>(std::get<1>(versionTuple)), static_cast<uint8_t>(std::get<2>(versionTuple)) };
    }
};