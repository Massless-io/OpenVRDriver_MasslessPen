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

#define EXPORT extern "C" __declspec(dllexport)

/// <summary>
/// Driver factory function
/// </summary>
/// <param name="interface_name">Requested interface name</param>
/// <param name="return_code">Return code, 0 if no error</param>
/// <returns>Non-owning pointer to the requested interface</returns>
EXPORT void* HmdDriverFactory(const char* interface_name, int* return_code);