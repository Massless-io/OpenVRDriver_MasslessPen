/* Copyright (C) 2020 Massless Corp. - All Rights Reserved
 * Author: Jacob Hilton.
 * You may use, distribute and modify this code under the
 * terms of the BSD 3-Clause "New" or "Revised" License.
 *
 * You should have received a copy of this license with
 * this file. If not, please email support@massless.io
 */

#include <DriverFactory.hpp>
#include <ServerDriver.hpp>
#include <MasslessInterface.hpp>

static bool hasInitializedDriver = false;

void* HmdDriverFactory(const char* interface_name, int* return_code) {

	if (std::string(interface_name) == std::string(vr::IServerTrackedDeviceProvider_Version)) {
		if (!hasInitializedDriver) {
			hasInitializedDriver = true;
			// Create the serverdriver with the MasslessPenSystem impl
			return static_cast<void*>(ServerDriver::create(std::make_shared<MasslessInterface::MasslessPenSystem>()).get());
		}
		// Return the existing driver instance
		return static_cast<void*>(ServerDriver::instance().get());
	}

	if (return_code)
		*return_code = vr::VRInitError_Init_InterfaceNotFound;

	return nullptr;
}