/* Copyright (C) 2020 Massless Corp. - All Rights Reserved
 * Author: Jacob Hilton.
 * You may use, distribute and modify this code under the
 * terms of the BSD 3-Clause "New" or "Revised" License.
 *
 * You should have received a copy of this license with
 * this file. If not, please email support@massless.io
 */

#pragma once

#include <memory>
#include <optional>
#include <thread>
#include <atomic>
#include <regex>
#include <mutex>

#include <IPenSystem.hpp>
#include <VRProcessEnumerator.hpp>
#include <DriverLog.hpp>

/// <summary>
/// Manages access to the backend Massless Pen System
/// </summary>
class MasslessManager
{
public:

    struct SystemLock {
        std::unique_lock<std::recursive_mutex> lock;
        std::optional<std::shared_ptr<MasslessInterface::IPenSystem>> pen_system;
    };

    MasslessManager(std::shared_ptr<MasslessInterface::IPenSystem> pen_system);
    ~MasslessManager();

    /// <summary>
    /// Attempts to get an exclusive lock to the pen system (non-blocking)
    /// Pointer can be nullopt in cases where the backend is not available ie. Massless Studio is running
    /// </summary>
    /// <returns>SystemLock with locked lock and valid pen_system, or unlocked lock with nullopt pen_system</returns>
    SystemLock getPenSystem();

private:

    std::shared_ptr<MasslessInterface::IPenSystem> m_penSystem;

    /// <summary>
    /// Recusive mutex so PenController, ServerDriver etc. all on the same thread can 
    /// lock it at the same time, but our internal thread cant lock it until they give 
    /// up access, and vice versa.
    /// </summary>
    std::recursive_mutex m_penSystemLock;

    std::regex m_masslessStudioMatcher{ "^(Massless)*\\s*Studio", std::regex_constants::ECMAScript | std::regex_constants::icase };
    std::thread m_masslessStudioCheckThread;
    std::atomic<bool> m_isMasslessStudioRunning = false;
    std::atomic<bool> m_wasMasslessStudioRunning = false;
    std::atomic<bool> m_shouldRunStudioCheckThread = true;
    std::atomic<bool> m_penSystemRunning = false;
};

