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

class MockVRInput : public vr::IVRDriverInput {
public:
    // Inherited via IVRDriverInput
    MOCK_METHOD3(CreateBooleanComponent, vr::EVRInputError(vr::PropertyContainerHandle_t ulContainer, const char* pchName, vr::VRInputComponentHandle_t* pHandle));
    MOCK_METHOD3(UpdateBooleanComponent, vr::EVRInputError(vr::VRInputComponentHandle_t ulComponent, bool bNewValue, double fTimeOffset));
    MOCK_METHOD5(CreateScalarComponent, vr::EVRInputError(vr::PropertyContainerHandle_t ulContainer, const char* pchName, vr::VRInputComponentHandle_t* pHandle, vr::EVRScalarType eType, vr::EVRScalarUnits eUnits));
    MOCK_METHOD3(UpdateScalarComponent, vr::EVRInputError(vr::VRInputComponentHandle_t ulComponent, float fNewValue, double fTimeOffset));
    MOCK_METHOD3(CreateHapticComponent, vr::EVRInputError(vr::PropertyContainerHandle_t ulContainer, const char* pchName, vr::VRInputComponentHandle_t* pHandle));
    MOCK_METHOD8(CreateSkeletonComponent, vr::EVRInputError(vr::PropertyContainerHandle_t ulContainer, const char* pchName, const char* pchSkeletonPath, const char* pchBasePosePath, vr::EVRSkeletalTrackingLevel eSkeletalTrackingLevel, const vr::VRBoneTransform_t* pGripLimitTransforms, uint32_t unGripLimitTransformCount, vr::VRInputComponentHandle_t* pHandle));
    MOCK_METHOD4(UpdateSkeletonComponent, vr::EVRInputError(vr::VRInputComponentHandle_t ulComponent, vr::EVRSkeletalMotionRange eMotionRange, const vr::VRBoneTransform_t* pTransforms, uint32_t unTransformCount));

};