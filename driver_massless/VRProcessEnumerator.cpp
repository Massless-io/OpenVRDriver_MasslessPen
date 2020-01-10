/* Copyright (C) 2020 Massless Corp. - All Rights Reserved
 * Author: Jacob Hilton.
 * You may use, distribute and modify this code under the
 * terms of the BSD 3-Clause "New" or "Revised" License.
 *
 * You should have received a copy of this license with
 * this file. If not, please email support@massless.io
 */

#include "VRProcessEnumerator.hpp"
#include <tchar.h>
#include <iostream>
#include <system_error>
#include <algorithm>


std::vector<PROCESSENTRY32> GetProcessList()
{
    std::vector<PROCESSENTRY32> processes;

    HANDLE hProcessSnap;
    HANDLE hProcess;
    PROCESSENTRY32 pe32;

    // Take a snapshot of all processes in the system.
    hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hProcessSnap == INVALID_HANDLE_VALUE)
    {
        throw std::system_error(std::error_code(0, std::system_category()));
    }

    // Set the size of the structure before using it.
    pe32.dwSize = sizeof(PROCESSENTRY32);

    // Retrieve information about the first process,
    // and exit if unsuccessful
    if (!Process32First(hProcessSnap, &pe32))
    {
        CloseHandle(hProcessSnap);          // clean the snapshot object
        throw std::system_error(std::error_code(0, std::system_category()));
    }

    // Now walk the snapshot of processes, and
    // display information about each process in turn
    do
    {
        // Retrieve process info
        hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe32.th32ProcessID);

        if (hProcess == NULL)
            continue;
        else
        {
            CloseHandle(hProcess);
            processes.push_back(pe32);
        }

    } while (Process32Next(hProcessSnap, &pe32));

    CloseHandle(hProcessSnap);
    return processes;
}

std::vector<MODULEENTRY32> GetProcessModules(DWORD dwPID)
{
    std::vector<MODULEENTRY32> modules;

    HANDLE hModuleSnap = INVALID_HANDLE_VALUE;
    MODULEENTRY32 me32;

    // Take a snapshot of all modules in the specified process.
    hModuleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwPID);
    if (hModuleSnap == INVALID_HANDLE_VALUE)
    {
        throw std::system_error(std::error_code(0, std::system_category()));
    }

    // Set the size of the structure before using it.
    me32.dwSize = sizeof(MODULEENTRY32);

    // Retrieve information about the first module,
    // and exit if unsuccessful
    if (!Module32First(hModuleSnap, &me32))
    {
        CloseHandle(hModuleSnap);           // clean the snapshot object
        throw std::system_error(std::error_code(0, std::system_category()));
    }

    // Now walk the module list of the process,
    // and display information about each module
    do
    {
        modules.push_back(me32);

    } while (Module32Next(hModuleSnap, &me32));

    CloseHandle(hModuleSnap);
    return modules;
}

std::vector<PROCESSENTRY32> VRProcessEnumerator::findVRClientProcesses()
{
    std::vector<PROCESSENTRY32> vrProcesses;
    std::vector<PROCESSENTRY32> processes = GetProcessList();
    for (auto& processEntry : processes) {
        try {
            std::vector<MODULEENTRY32> processModules = GetProcessModules(processEntry.th32ProcessID);

            // try to find vrclient (indicating a process that can interact with steamvr)
            auto vrclientModule = std::find_if(processModules.begin(), processModules.end(),
                [](MODULEENTRY32 entry)->bool {
                    std::wstring nameWString(entry.szModule);
                    return nameWString.find(L"vrclient") != std::wstring::npos;
                }
            );

            if (vrclientModule != processModules.end()) {
                vrProcesses.push_back(processEntry);
            }
            
        }
        catch (std::exception e) {
            // skip this process
        }
    }
    return vrProcesses;
}

bool VRProcessEnumerator::isProcessRunning(std::regex exeMatcher)
{
    auto process_list = GetProcessList();
    for (const auto& process : process_list) {
        std::stringstream s;
        std::wstring exeName(process.szExeFile);
        const std::ctype<char>& ctfacet = std::use_facet<std::ctype<char>>(s.getloc());
        for (size_t i = 0; i < exeName.size(); ++i)
            s << ctfacet.narrow(static_cast<char>(exeName[i]), 0);
        if (std::regex_search(s.str(), exeMatcher))
            return true;
    }

    return false;
}

std::optional<std::wstring> VRProcessEnumerator::getVRClientProcessName(unsigned long pid)
{
    if (pid == 0) {
        return L"vrserver.exe";
    }
    auto vr_processes = findVRClientProcesses();
    for (auto process : vr_processes) {
        if (process.th32ProcessID == pid) {
            return std::wstring(process.szExeFile);
        }
    }

    return std::nullopt;
}
