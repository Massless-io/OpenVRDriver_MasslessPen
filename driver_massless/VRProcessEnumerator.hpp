/* Copyright (C) 2020 Massless Corp. - All Rights Reserved
 * Author: Jacob Hilton.
 * You may use, distribute and modify this code under the
 * terms of the BSD 3-Clause "New" or "Revised" License.
 *
 * You should have received a copy of this license with
 * this file. If not, please email support@massless.io
 */

#pragma once
#define NOMINMAX 
#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <tlhelp32.h>
#include <vector>
#include <array>
#include <regex>
#include <cctype>
#include <sstream>
#include <DriverLog.hpp>
#include <thread>
#include <optional>

class VRProcessEnumerator
{
public:
    static std::vector<PROCESSENTRY32> findVRClientProcesses();

    static bool isProcessRunning(std::regex exeMatcher);
    
    static std::optional<std::wstring> getVRClientProcessName(unsigned long pid);
};

