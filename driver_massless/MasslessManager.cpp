/* Copyright (C) 2020 Massless Corp. - All Rights Reserved
 * Author: Jacob Hilton.
 * You may use, distribute and modify this code under the
 * terms of the BSD 3-Clause "New" or "Revised" License.
 *
 * You should have received a copy of this license with
 * this file. If not, please email support@massless.io
 */

#include "MasslessManager.hpp"

MasslessManager::MasslessManager(std::shared_ptr<MasslessInterface::IPenSystem> pen_system):
    m_penSystem(pen_system)
{
    this->m_masslessStudioCheckThread = std::thread(
        [&isRunning = this->m_isMasslessStudioRunning, 
        &wasRunning = this->m_wasMasslessStudioRunning, 
        &shouldRun = this->m_shouldRunStudioCheckThread, 
        &masslessStudioMatcher = this->m_masslessStudioMatcher,
        &penSystem = this->m_penSystem,
        &penSystemLock = this->m_penSystemLock,
        &penSystemRunning = this->m_penSystemRunning
        ]() {
        std::unique_lock my_lock(penSystemLock, std::defer_lock);
            while (shouldRun) {
                isRunning = VRProcessEnumerator::isProcessRunning(masslessStudioMatcher);
                // If we have changed state since last time we checked
                if (wasRunning != isRunning) {
                    if (isRunning) {
                        // Lock access, stop the system if it was started
                        my_lock.lock();
                        if (penSystem->isSystemRunning()) {
                            penSystemRunning = true;
                            auto res = penSystem->stopSystem();
                            if (res.has_value())
                                DriverLog("[Error] Unable to stop backend for Massless Studio. Threw error code [%d].\n", res.value());
                        }
                    }
                    else {
                        // Restart the system if it was previously started, unlock access
                        if (penSystemRunning) {
                            penSystemRunning = false;
                            auto res = penSystem->startSystem();
                            if (res.has_value())
                                DriverLog("[Error] Unable to restart backend after Massless Studio closed. Threw error code [%d].\n", res.value());
                        }
                        my_lock.unlock();
                    }
                    bool wRunning = isRunning;
                    wasRunning = wRunning;
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
        }
    );
}

MasslessManager::~MasslessManager()
{
    this->m_shouldRunStudioCheckThread = false;
    if(this->m_masslessStudioCheckThread.joinable())
        this->m_masslessStudioCheckThread.join();
}

MasslessManager::SystemLock MasslessManager::getPenSystem()
{
    // Try and get access to the pen system
    std::unique_lock<std::recursive_mutex> lock(this->m_penSystemLock, std::try_to_lock);
    if (!lock.owns_lock())
        return SystemLock{ std::move(lock), std::nullopt };
    return SystemLock{ std::move(lock), this->m_penSystem };
}
