/* Copyright (C) 2020 Massless Corp. - All Rights Reserved
 * Author: Jacob Hilton.
 * You may use, distribute and modify this code under the
 * terms of the BSD 3-Clause "New" or "Revised" License.
 *
 * You should have received a copy of this license with
 * this file. If not, please email support@massless.io
 */

#include "Testing.hpp"
using namespace testing;

ACTION_P(RPB_CopyStringToBuffer, input_str)
{
    std::string input_as_str(input_str);
    const char* c_str = input_as_str.c_str();
    std::memset(arg1->pvBuffer, 0, strlen(c_str) + 1);
    std::strncpy(static_cast<char*>(arg1->pvBuffer), c_str, strlen(c_str));
}

ACTION(RPB_SetupOtherReturnValues) {
    arg1->eError = vr::ETrackedPropertyError::TrackedProp_Success;
    arg1->unTag = vr::k_unStringPropertyTag;
}

ACTION_P2(GRTDP_CopyPoseToBuffer, idx, example_value) {
    std::array<vr::TrackedDevicePose_t, vr::k_unMaxTrackedDeviceCount> poses = {{0}};
    poses[idx] = example_value;
    std::memcpy(arg1, poses.data(), sizeof(vr::TrackedDevicePose_t) * poses.size());
}

ACTION_P(GRTDP_CopyPoseArrayToBuffer, poseArray) {
    std::memcpy(arg1, poseArray.data(), sizeof(vr::TrackedDevicePose_t) * poseArray.size());
}

TEST(DriverAnalyticsTest, FindLikelyTrackingRefIndexFindsCorrectIndices) {
    
    // Setup properties, used to check the device model number
    int properties_id = 1;
    testing::NiceMock<MockVRProperties> properties;
    ON_CALL(properties, TrackedDeviceToPropertyContainer(_))
        .WillByDefault(Return(properties_id));
    vr::CVRPropertyHelpers helper(&properties);

    // Setup a valid tracked device pose
    vr::TrackedDevicePose_t connected_pose;
    connected_pose.bDeviceIsConnected = true;
    connected_pose.bPoseIsValid = true;
    connected_pose.eTrackingResult = vr::ETrackingResult::TrackingResult_Running_OK;

    // Setup an invalid pose
    vr::TrackedDevicePose_t disconnected_pose = { 0 };

    // Setup blank pose return action
    testing::NiceMock<MockVRServerDriverHost> serverdriver_host;
    ON_CALL(serverdriver_host, GetRawTrackedDevicePoses(_, _, _)).WillByDefault(Return());

    {
        // No devices so we should return nullopt
        ASSERT_EQ(DriverAnalytics::findLikelyTrackingReferenceIndex(&serverdriver_host, &helper), std::nullopt) << "Empty device list should return nullopt";
    }
    {
        // Setup return property
        ON_CALL(properties, ReadPropertyBatch(properties_id, _, _))
            .WillByDefault(DoAll(RPB_SetupOtherReturnValues(), RPB_CopyStringToBuffer("Oculus Rift CV1 (Camera0)"), Return(vr::ETrackedPropertyError::TrackedProp_Success)));
        int device_index = 20;

        // nth device will be connected
        ON_CALL(serverdriver_host, GetRawTrackedDevicePoses(_, _, _)).WillByDefault(GRTDP_CopyPoseToBuffer(device_index, connected_pose));

        // Run tests
        auto likelyRiftCamera = DriverAnalytics::findLikelyTrackingReferenceIndex(&serverdriver_host, &helper);
        ASSERT_EQ(likelyRiftCamera.has_value(), true) << "Rift camera should have value";
        ASSERT_EQ(likelyRiftCamera->first, device_index) << "Rift camera should be " << device_index << "th device index";
        ASSERT_EQ(likelyRiftCamera->second.getSystemType(), MasslessInterface::TrackingSystemType::SystemType::OCULUS_SENSOR) << "Rift camera should be of type OCULUS_SENSOR";
    }
    {
        // Setup return property
        ON_CALL(properties, ReadPropertyBatch(properties_id, _, _))
            .WillByDefault(DoAll(RPB_SetupOtherReturnValues(), RPB_CopyStringToBuffer("HTC V2-XD/XE"), Return(vr::ETrackedPropertyError::TrackedProp_Success)));
        int device_index = 20;

        // nth device will be connected
        ON_CALL(serverdriver_host, GetRawTrackedDevicePoses(_, _, _)).WillByDefault(GRTDP_CopyPoseToBuffer(device_index, connected_pose));

        // Run tests
        auto likelyViveLighthouse1 = DriverAnalytics::findLikelyTrackingReferenceIndex(&serverdriver_host, &helper);
        ASSERT_EQ(likelyViveLighthouse1.has_value(), true) << "Vive lighthouse should have value";
        ASSERT_EQ(likelyViveLighthouse1->first, device_index) << "Vive lighthouse should be " << device_index << "th device index";
        ASSERT_EQ(likelyViveLighthouse1->second.getSystemType(), MasslessInterface::TrackingSystemType::SystemType::VIVE_BASESTATION_V1) << "Vive lighthouse should be of type VIVE_BASESTATION_V1";
    }
    {
        // Setup return property
        ON_CALL(properties, ReadPropertyBatch(properties_id, _, _))
            .WillByDefault(DoAll(RPB_SetupOtherReturnValues(), RPB_CopyStringToBuffer("Valve SR Imp"), Return(vr::ETrackedPropertyError::TrackedProp_Success)));
        int device_index = 20;

        // nth device will be connected
        ON_CALL(serverdriver_host, GetRawTrackedDevicePoses(_, _, _)).WillByDefault(GRTDP_CopyPoseToBuffer(device_index, connected_pose));

        // Run tests
        auto likelyViveLighthouse2 = DriverAnalytics::findLikelyTrackingReferenceIndex(&serverdriver_host, &helper);
        ASSERT_EQ(likelyViveLighthouse2.has_value(), true) << "Lighthouse V2 should have value";
        ASSERT_EQ(likelyViveLighthouse2->first, device_index) << "Lighthouse V2 should be " << device_index << "th device index";
        ASSERT_EQ(likelyViveLighthouse2->second.getSystemType(), MasslessInterface::TrackingSystemType::SystemType::VIVE_BASESTATION_V2) << "Lighthouse V2 should be of type VIVE_BASESTATION_V2";
    }
    {
        // Setup return property
        ON_CALL(properties, ReadPropertyBatch(properties_id, _, _))
            .WillByDefault(DoAll(RPB_SetupOtherReturnValues(), RPB_CopyStringToBuffer("VIVE Tracker Pro MV"), Return(vr::ETrackedPropertyError::TrackedProp_Success)));
        int device_index = 20;

        // nth device will be connected
        ON_CALL(serverdriver_host, GetRawTrackedDevicePoses(_, _, _)).WillByDefault(GRTDP_CopyPoseToBuffer(device_index, connected_pose));

        // Run tests
        auto likelyViveTracker = DriverAnalytics::findLikelyTrackingReferenceIndex(&serverdriver_host, &helper);
        ASSERT_EQ(likelyViveTracker.has_value(), true) << "Vive tracker should have value";
        ASSERT_EQ(likelyViveTracker->first, device_index) << "Vive tracker should be " << device_index << "th device index";
        ASSERT_EQ(likelyViveTracker->second.getSystemType(), MasslessInterface::TrackingSystemType::SystemType::VIVE_TRACKER) << "Vive tracker should be of type VIVE_TRACKER";
    }
}

TEST(DriverAnalyticsTest, GetReferenceSystemTypeReturnsCorrectType) {
    testing::NiceMock<MockVRProperties> properties;
    ON_CALL(properties, TrackedDeviceToPropertyContainer(_)).WillByDefault(Return(1));
    vr::CVRPropertyHelpers helper(&properties);

    //std::array<vr::TrackedDevicePose_t, vr::k_unMaxTrackedDeviceCount> devices = { 0 };
    ON_CALL(properties, ReadPropertyBatch(1, _, _))
       .WillByDefault(Return(vr::ETrackedPropertyError::TrackedProp_NotYetAvailable));

    EXPECT_EQ(DriverAnalytics::getReferenceSystemType(1, &helper).getSystemType(), MasslessInterface::TrackingSystemType::SystemType::INVALID_SYSTEM) << "First device in list is unset, this should return INVALID_SYSTEM";

    EXPECT_CALL(properties, ReadPropertyBatch(1, _, _)).WillOnce(DoAll(RPB_SetupOtherReturnValues(), RPB_CopyStringToBuffer("Oculus Rift CV1 (Camera0)"), Return(vr::ETrackedPropertyError::TrackedProp_Success)));
    EXPECT_EQ(DriverAnalytics::getReferenceSystemType(1, &helper).getSystemType(), MasslessInterface::TrackingSystemType::SystemType::OCULUS_SENSOR) << "Should be OCULUS_SENSOR";

    EXPECT_CALL(properties, ReadPropertyBatch(1, _, _)).WillOnce(DoAll(RPB_SetupOtherReturnValues(), RPB_CopyStringToBuffer("HTC V2-XD/XE"), Return(vr::ETrackedPropertyError::TrackedProp_Success)));
    EXPECT_EQ(DriverAnalytics::getReferenceSystemType(1, &helper).getSystemType(), MasslessInterface::TrackingSystemType::SystemType::VIVE_BASESTATION_V1) << "Should be VIVE_BASESTATION_V1";

    EXPECT_CALL(properties, ReadPropertyBatch(1, _, _)).WillOnce(DoAll(RPB_SetupOtherReturnValues(), RPB_CopyStringToBuffer("Valve SR Imp"), Return(vr::ETrackedPropertyError::TrackedProp_Success)));
    EXPECT_EQ(DriverAnalytics::getReferenceSystemType(1, &helper).getSystemType(), MasslessInterface::TrackingSystemType::SystemType::VIVE_BASESTATION_V2) << "Should be VIVE_BASESTATION_V2";

    EXPECT_CALL(properties, ReadPropertyBatch(1, _, _)).WillOnce(DoAll(RPB_SetupOtherReturnValues(), RPB_CopyStringToBuffer("VIVE Tracker Pro MV"), Return(vr::ETrackedPropertyError::TrackedProp_Success)));
    EXPECT_EQ(DriverAnalytics::getReferenceSystemType(1, &helper).getSystemType(), MasslessInterface::TrackingSystemType::SystemType::VIVE_TRACKER) << "Should be VIVE_TRACKER";
}

TEST(DriverAnalyticsTest, IsTrackingReferenceIsCorrect) {
    testing::NiceMock<MockVRProperties> properties;
    ON_CALL(properties, TrackedDeviceToPropertyContainer(_)).WillByDefault(Return(1));
    vr::CVRPropertyHelpers helper(&properties);

    std::array<vr::TrackedDevicePose_t, vr::k_unMaxTrackedDeviceCount> devices = { 0 };
    ON_CALL(properties, ReadPropertyBatch(1, _, _))
        .WillByDefault(Return(vr::ETrackedPropertyError::TrackedProp_NotYetAvailable));

    EXPECT_CALL(properties, ReadPropertyBatch(1, _, _)).WillOnce(DoAll(RPB_SetupOtherReturnValues(), RPB_CopyStringToBuffer("Oculus Rift CV1 (Camera0)"), Return(vr::ETrackedPropertyError::TrackedProp_Success)));
    EXPECT_EQ(DriverAnalytics::isTrackingReference(1, &helper), true) << "'Oculus Rift CV1 (Camera0)' should be a tracking reference";

    EXPECT_CALL(properties, ReadPropertyBatch(1, _, _)).WillOnce(DoAll(RPB_SetupOtherReturnValues(), RPB_CopyStringToBuffer("HTC V2-XD/XE"), Return(vr::ETrackedPropertyError::TrackedProp_Success)));
    EXPECT_EQ(DriverAnalytics::isTrackingReference(1, &helper), true) << "'HTC V2-XD/XE' should be a tracking reference";

    EXPECT_CALL(properties, ReadPropertyBatch(1, _, _)).WillOnce(DoAll(RPB_SetupOtherReturnValues(), RPB_CopyStringToBuffer("Valve SR Imp"), Return(vr::ETrackedPropertyError::TrackedProp_Success)));
    EXPECT_EQ(DriverAnalytics::isTrackingReference(1, &helper), true) << "'Valve SR Imp' should be a tracking reference";

    EXPECT_CALL(properties, ReadPropertyBatch(1, _, _)).WillOnce(DoAll(RPB_SetupOtherReturnValues(), RPB_CopyStringToBuffer("VIVE Tracker Pro MV"), Return(vr::ETrackedPropertyError::TrackedProp_Success)));
    EXPECT_EQ(DriverAnalytics::isTrackingReference(1, &helper), true) << "'VIVE Tracker Pro MV' should be a tracking reference";

    EXPECT_CALL(properties, ReadPropertyBatch(1, _, _)).WillOnce(DoAll(RPB_SetupOtherReturnValues(), RPB_CopyStringToBuffer("Vive. MV"), Return(vr::ETrackedPropertyError::TrackedProp_Success)));
    EXPECT_EQ(DriverAnalytics::isTrackingReference(1, &helper), false) << "'Vive. MV' should not be a tracking reference";

    EXPECT_CALL(properties, ReadPropertyBatch(1, _, _)).WillOnce(DoAll(RPB_SetupOtherReturnValues(), RPB_CopyStringToBuffer("Index"), Return(vr::ETrackedPropertyError::TrackedProp_Success)));
    EXPECT_EQ(DriverAnalytics::isTrackingReference(1, &helper), false) << "'Index' should not be a tracking reference";

    EXPECT_CALL(properties, ReadPropertyBatch(1, _, _)).WillOnce(DoAll(RPB_SetupOtherReturnValues(), RPB_CopyStringToBuffer("Oculus Rift CV1 (Left Controller)"), Return(vr::ETrackedPropertyError::TrackedProp_Success)));
    EXPECT_EQ(DriverAnalytics::isTrackingReference(1, &helper), false) << "'Oculus Rift CV1 (Left Controller)' should not be a tracking reference";

    EXPECT_CALL(properties, ReadPropertyBatch(1, _, _)).WillOnce(DoAll(RPB_SetupOtherReturnValues(), RPB_CopyStringToBuffer("Oculus Rift CV1"), Return(vr::ETrackedPropertyError::TrackedProp_Success)));
    EXPECT_EQ(DriverAnalytics::isTrackingReference(1, &helper), false) << "'Oculus Rift CV1' should not be a tracking reference";

    EXPECT_CALL(properties, ReadPropertyBatch(1, _, _)).WillOnce(DoAll(RPB_SetupOtherReturnValues(), RPB_CopyStringToBuffer("Vive. Controller MV"), Return(vr::ETrackedPropertyError::TrackedProp_Success)));
    EXPECT_EQ(DriverAnalytics::isTrackingReference(1, &helper), false) << "'Vive. Controller MV' should not be a tracking reference";

    EXPECT_CALL(properties, ReadPropertyBatch(1, _, _)).WillOnce(DoAll(RPB_SetupOtherReturnValues(), RPB_CopyStringToBuffer("Knuckles Left"), Return(vr::ETrackedPropertyError::TrackedProp_Success)));
    EXPECT_EQ(DriverAnalytics::isTrackingReference(1, &helper), false) << "'Knuckles Left' should not be a tracking reference";

}

TEST(DriverAnalyticsTest, HmdMatrixtoEigenPoseConversionIsCorrect) {


    // converted using https://www.andre-gaschler.com/rotationconverter/
    vr::HmdMatrix34_t input_matrix;
    input_matrix.m[0][3] = 2;
    input_matrix.m[1][3] = 3;
    input_matrix.m[2][3] = 4;

    input_matrix.m[0][0] = 0.762660979240981;
    input_matrix.m[1][0] = 0.237339020759019;
    input_matrix.m[2][0] = 0.601679665576574;

    input_matrix.m[0][1] = 0.237339020759019;
    input_matrix.m[1][1] = 0.762660979240981;
    input_matrix.m[2][1] = -0.601679665576574;

    input_matrix.m[0][2] = -0.601679665576574;
    input_matrix.m[1][2] = 0.601679665576574;
    input_matrix.m[2][2] = 0.525321958481962;

    Eigen::Matrix3f expected_mat = (Eigen::Matrix3f() << input_matrix.m[0][0], input_matrix.m[0][1], input_matrix.m[0][2], input_matrix.m[1][0], input_matrix.m[1][1], input_matrix.m[1][2], input_matrix.m[2][0], input_matrix.m[2][1], input_matrix.m[2][2]).finished();
    Eigen::Vector3f expected_vec(input_matrix.m[0][3], input_matrix.m[1][3], input_matrix.m[2][3]);

    auto [received_quat, received_vec] = DriverAnalytics::hmdMatrixtoEigenPose(input_matrix);

    Eigen::Matrix3f received_quat_as_mat(received_quat);

    // Expect rotation matrix and position vector are same
    EXPECT_THAT(received_quat_as_mat.isApprox(expected_mat), true) << "Received: " << received_quat_as_mat;
    EXPECT_THAT(received_vec.isApprox(expected_vec), true) << "Received: " << received_vec;

}

TEST(DriverAnalyticsTest, GetDeviceSerialReturnsCorrectSerial) {
    // Setup properties, used to check the device serial number
    int properties_id = 1;
    testing::NiceMock<MockVRProperties> properties;
    ON_CALL(properties, TrackedDeviceToPropertyContainer(_))
        .WillByDefault(Return(properties_id));
    vr::CVRPropertyHelpers helper(&properties);
    ON_CALL(properties, ReadPropertyBatch(properties_id, _, _))
        .WillByDefault(Return(vr::ETrackedPropertyError::TrackedProp_NotYetAvailable));

    std::string example_serial = "example_device_serial";

    EXPECT_CALL(properties, ReadPropertyBatch(properties_id, _, _))
        .WillOnce(DoAll(RPB_SetupOtherReturnValues(), RPB_CopyStringToBuffer(example_serial), Return(vr::ETrackedPropertyError::TrackedProp_Success)));
    
    EXPECT_THAT(DriverAnalytics::getDeviceSerial(1, &helper), Eq(example_serial));

}

TEST(DriverAnalyticsTest, GetDeviceIndexReturnsCorrectIndex) {
    // Setup properties, used to check the device model number
    int properties_id = 1;
    testing::NiceMock<MockVRProperties> properties;
    ON_CALL(properties, TrackedDeviceToPropertyContainer(_))
        .WillByDefault(Return(properties_id));
    vr::CVRPropertyHelpers helper(&properties);

    // Setup a valid tracked device pose
    vr::TrackedDevicePose_t connected_pose;
    connected_pose.bDeviceIsConnected = true;
    connected_pose.bPoseIsValid = true;
    connected_pose.eTrackingResult = vr::ETrackingResult::TrackingResult_Running_OK;

    std::string example_serial = "example_device_serial";

    // Setup blank pose return action
    testing::NiceMock<MockVRServerDriverHost> serverdriver_host;

    // Setup return property
    ON_CALL(properties, ReadPropertyBatch(properties_id, _, _))
        .WillByDefault(DoAll(RPB_SetupOtherReturnValues(), RPB_CopyStringToBuffer(example_serial), Return(vr::ETrackedPropertyError::TrackedProp_Success)));
    int device_index = 20;

    // nth device will be connected
    ON_CALL(serverdriver_host, GetRawTrackedDevicePoses(_, _, _)).WillByDefault(GRTDP_CopyPoseToBuffer(device_index, connected_pose));

    auto possible_device_index = DriverAnalytics::getDeviceIndex(example_serial, &serverdriver_host, &helper);
    EXPECT_TRUE(possible_device_index.has_value()) << "The " << device_index << "th device should be the device with this serial";
    EXPECT_THAT(*possible_device_index, Eq(device_index));

}

TEST(DriverAnalyticsTest, GetDevicePoseReturnsCorrectPose) {
    // Setup properties, used to check the device model number

    // Setup a valid tracked device pose
    vr::TrackedDevicePose_t connected_pose;
    connected_pose.bDeviceIsConnected = true;
    connected_pose.bPoseIsValid = true;
    connected_pose.eTrackingResult = vr::ETrackingResult::TrackingResult_Running_OK;

    vr::HmdMatrix34_t input_matrix;
    input_matrix.m[0][3] = 2;
    input_matrix.m[1][3] = 3;
    input_matrix.m[2][3] = 4;

    input_matrix.m[0][0] = 0.762660979240981;
    input_matrix.m[1][0] = 0.237339020759019;
    input_matrix.m[2][0] = 0.601679665576574;

    input_matrix.m[0][1] = 0.237339020759019;
    input_matrix.m[1][1] = 0.762660979240981;
    input_matrix.m[2][1] = -0.601679665576574;

    input_matrix.m[0][2] = -0.601679665576574;
    input_matrix.m[1][2] = 0.601679665576574;
    input_matrix.m[2][2] = 0.525321958481962;

    connected_pose.mDeviceToAbsoluteTracking = input_matrix;

    // Setup blank pose return action
    testing::NiceMock<MockVRServerDriverHost> serverdriver_host;

    int device_index = 20;

    // nth device will be connected
    ON_CALL(serverdriver_host, GetRawTrackedDevicePoses(_, _, _)).WillByDefault(GRTDP_CopyPoseToBuffer(device_index, connected_pose));

    auto return_pose = DriverAnalytics::getDevicePose(device_index, &serverdriver_host);

    Eigen::Matrix3f expected_mat = (Eigen::Matrix3f() << input_matrix.m[0][0], input_matrix.m[0][1], input_matrix.m[0][2], input_matrix.m[1][0], input_matrix.m[1][1], input_matrix.m[1][2], input_matrix.m[2][0], input_matrix.m[2][1], input_matrix.m[2][2]).finished();
    Eigen::Vector3f expected_vec(input_matrix.m[0][3], input_matrix.m[1][3], input_matrix.m[2][3]);

    Eigen::Matrix3f received_quat_as_mat(return_pose->first);

    // Expect rotation matrix and position vector are same
    EXPECT_TRUE(return_pose.has_value());
    EXPECT_THAT(received_quat_as_mat.isApprox(expected_mat), true) << "Received: " << received_quat_as_mat;
    EXPECT_THAT(return_pose->second.isApprox(expected_vec), true) << "Received: " << return_pose->second;

}

TEST(DriverAnalyticsTest, FirstPassFindsCorrectTrackingReferences) {
    // Setup properties, used to check the device model number
    int properties_id = 1;
    testing::NiceMock<MockVRProperties> properties;
    ON_CALL(properties, TrackedDeviceToPropertyContainer(_))
        .WillByDefault(Invoke([&](int device_index)->int {return device_index; }));
    vr::CVRPropertyHelpers helper(&properties);

    testing::NiceMock<MockVRServerDriverHost> serverdriver_host;

    // Setup a valid tracked device pose
    vr::TrackedDevicePose_t connected_pose;
    connected_pose.bDeviceIsConnected = true;
    connected_pose.bPoseIsValid = true;
    connected_pose.eTrackingResult = vr::ETrackingResult::TrackingResult_Running_OK;

    // Setup an invalid pose
    vr::TrackedDevicePose_t disconnected_pose = { 0 };

    // Setup return property
    ON_CALL(properties, ReadPropertyBatch(1, _, _)).WillByDefault(DoAll(RPB_SetupOtherReturnValues(), RPB_CopyStringToBuffer("HTC V2-XD/XE"), Return(vr::ETrackedPropertyError::TrackedProp_Success)));
    ON_CALL(properties, ReadPropertyBatch(2, _, _)).WillByDefault(DoAll(RPB_SetupOtherReturnValues(), RPB_CopyStringToBuffer("VIVE Tracker Pro MV"), Return(vr::ETrackedPropertyError::TrackedProp_Success)));
    
    std::array<vr::TrackedDevicePose_t, vr::k_unMaxTrackedDeviceCount> device_poses = { {0} };
    device_poses[1] = connected_pose;
    device_poses[2] = connected_pose;
    ON_CALL(serverdriver_host, GetRawTrackedDevicePoses(_, _, _)).WillByDefault(GRTDP_CopyPoseArrayToBuffer(device_poses));

    // Second device is vive tracker, but should be first in the list
    auto reference_list = DriverAnalytics::firstPass(&serverdriver_host, &helper);
    EXPECT_EQ(reference_list.size(), 2) << "2 possible tracking references should be connected";
    EXPECT_EQ(reference_list.at(0), 2) << "First tracking reference should be device id 2";
    EXPECT_EQ(reference_list.at(1), 1) << "Second tracking reference should be device id 1";

}

TEST(DriverAnalyticsTest, SecondPassGetsAllRequiredInformation) {
    // TODO: Cannot be run yet as it relies on Massless code
}