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
#include <Eigen/Eigen>

const auto tr_to_str = [](Eigen::Translation3f translation) {
    std::stringstream s;
    s << translation.x() << ",\t" << translation.y() << ",\t" << translation.z();
    return s.str();
};

const auto qu_to_str = [](Eigen::Quaternionf quat) {
    std::stringstream s;
    s << quat.w() << ",\t" << quat.x() << ",\t" << quat.y() << ",\t" << quat.z();
    return s.str();
};

void DriverLog(const char* pchFormat, ...);
void DebugDriverLog(const char* pchFormat, ...);
bool InitDriverLog(vr::IVRDriverLog* pDriverLog);
void CleanupDriverLog();