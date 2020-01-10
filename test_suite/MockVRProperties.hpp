/* Copyright (C) 2020 Massless Corp. - All Rights Reserved
 * Author: Jacob Hilton.
 * You may use, distribute and modify this code under the
 * terms of the BSD 3-Clause "New" or "Revised" License.
 *
 * You should have received a copy of this license with
 * this file. If not, please email support@massless.io
 */

#pragma once

#include "gmock/gmock.h"
#include "openvr_driver.h"

class MockVRProperties : public vr::IVRProperties {
public:
    MOCK_METHOD3(ReadPropertyBatch, vr::ETrackedPropertyError(vr::PropertyContainerHandle_t ulContainerHandle, vr::PropertyRead_t* pBatch, uint32_t unBatchEntryCount));
    MOCK_METHOD3(WritePropertyBatch, vr::ETrackedPropertyError(vr::PropertyContainerHandle_t ulContainerHandle, vr::PropertyWrite_t* pBatch, uint32_t unBatchEntryCount));
    MOCK_METHOD1(GetPropErrorNameFromEnum, const char* (vr::ETrackedPropertyError error));
    MOCK_METHOD1(TrackedDeviceToPropertyContainer, vr::PropertyContainerHandle_t(vr::TrackedDeviceIndex_t nDevice));

};