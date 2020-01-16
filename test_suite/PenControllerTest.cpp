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

TEST(PenController, ConstructorWorks) {
    std::shared_ptr<SettingsManager> settings_manager = std::make_shared<SettingsManager>(std::make_unique<FileSettingsLoader>(std::make_unique<std::istringstream>("{}"), std::make_unique<std::ostringstream>()));
    std::shared_ptr<NiceMock<MockPenSystem>> pen_system = std::make_shared<NiceMock<MockPenSystem>>();
    std::shared_ptr<MasslessManager> massless_manager = std::make_shared<MasslessManager>(pen_system);
    ASSERT_NO_FATAL_FAILURE({
        PenController controller(settings_manager, massless_manager);
    });
}

TEST(PenController, DefaultPoseIsInvalid) {
    std::shared_ptr<SettingsManager> settings_manager = std::make_shared<SettingsManager>(std::make_unique<FileSettingsLoader>(std::make_unique<std::istringstream>("{}"), std::make_unique<std::ostringstream>()));
    std::shared_ptr<NiceMock<MockPenSystem>> pen_system = std::make_shared<NiceMock<MockPenSystem>>();
    std::shared_ptr<MasslessManager> massless_manager = std::make_shared<MasslessManager>(pen_system);

    PenController controller(settings_manager, massless_manager);

    ASSERT_THAT(controller.GetPose(), Field(&vr::DriverPose_t::poseIsValid, false));
}
TEST(PenController, ActivateRegistersInputsPropertiesAndStartsSystem) {
    std::unique_ptr<vr::IVRSettings> settings = std::make_unique<MockVRSettings>();
    std::shared_ptr<SettingsManager> settings_manager = std::make_shared<SettingsManager>(std::make_unique<FileSettingsLoader>(std::make_unique<std::istringstream>("{}"), std::make_unique<std::ostringstream>()));
    std::shared_ptr<NiceMock<MockPenSystem>> pen_system = std::make_shared<NiceMock<MockPenSystem>>();
    std::shared_ptr<MasslessManager> massless_manager = std::make_shared<MasslessManager>(pen_system);


    // Setup some known values
    const int properties_id = 100;
    const int tracked_device_index = 200;


    // Setup properties
    testing::NiceMock<MockVRProperties> properties;
    EXPECT_CALL(properties, TrackedDeviceToPropertyContainer(tracked_device_index))
        .WillRepeatedly(Return(properties_id));
    vr::CVRPropertyHelpers helper(&properties);

    testing::NiceMock<MockVRInput> input;

    // Create the controller
    PenController controller(settings_manager, massless_manager);

    // Check all boolean inputs have been registered
    EXPECT_CALL(input, CreateBooleanComponent(properties_id, StrEq("/input/front/single/click"), _))
        .Times(1);
    EXPECT_CALL(input, CreateBooleanComponent(properties_id, StrEq("/input/front/double/click"), _))
        .Times(1);
    EXPECT_CALL(input, CreateBooleanComponent(properties_id, StrEq("/input/rear/single/click"), _))
        .Times(1);
    EXPECT_CALL(input, CreateBooleanComponent(properties_id, StrEq("/input/rear/double/click"), _))
        .Times(1);
    EXPECT_CALL(input, CreateBooleanComponent(properties_id, StrEq("/input/swipe/forwards/click"), _))
        .Times(1);
    EXPECT_CALL(input, CreateBooleanComponent(properties_id, StrEq("/input/swipe/backwards/click"), _))
        .Times(1);

    // Check haptic output has been registered
    EXPECT_CALL(input, CreateHapticComponent(properties_id, StrEq("/output/haptic"), _))
        .Times(1);

    // Check universe ID is set to 2
    EXPECT_CALL(properties, WritePropertyBatch(properties_id,
        AllOf(
            Pointee(Field(&vr::PropertyWrite_t::prop, Eq(vr::Prop_CurrentUniverseId_Uint64))),
            Pointee(Field(&vr::PropertyWrite_t::pvBuffer, MatcherCast<uint64_t*>(Pointee(Eq(2)))))
        ), _));

    // Check rendermodel name is set
    EXPECT_CALL(properties, WritePropertyBatch(properties_id, Pointee(Field(&vr::PropertyWrite_t::prop, vr::Prop_ModelNumber_String)), _))
        .Times(1);
    EXPECT_CALL(properties, WritePropertyBatch(properties_id, Pointee(Field(&vr::PropertyWrite_t::prop, vr::Prop_RenderModelName_String)), _))
        .Times(1);
    
    // Expect version string to be set
    EXPECT_CALL(properties, WritePropertyBatch(properties_id, Pointee(Field(&vr::PropertyWrite_t::prop, vr::Prop_DriverVersion_String)), _))
        .Times(1);

    // Check all icon paths are set
    EXPECT_CALL(properties, WritePropertyBatch(properties_id, Pointee(Field(&vr::PropertyWrite_t::prop, vr::Prop_NamedIconPathDeviceOff_String)), _))
        .Times(1);
    EXPECT_CALL(properties, WritePropertyBatch(properties_id, Pointee(Field(&vr::PropertyWrite_t::prop, vr::Prop_NamedIconPathDeviceSearching_String)), _))
        .Times(1);
    EXPECT_CALL(properties, WritePropertyBatch(properties_id, Pointee(Field(&vr::PropertyWrite_t::prop, vr::Prop_NamedIconPathDeviceSearchingAlert_String)), _))
        .Times(1);
    EXPECT_CALL(properties, WritePropertyBatch(properties_id, Pointee(Field(&vr::PropertyWrite_t::prop, vr::Prop_NamedIconPathDeviceReady_String)), _))
        .Times(1);
    EXPECT_CALL(properties, WritePropertyBatch(properties_id, Pointee(Field(&vr::PropertyWrite_t::prop, vr::Prop_NamedIconPathDeviceReadyAlert_String)), _))
        .Times(1);
    EXPECT_CALL(properties, WritePropertyBatch(properties_id, Pointee(Field(&vr::PropertyWrite_t::prop, vr::Prop_NamedIconPathDeviceNotReady_String)), _))
        .Times(1);
    EXPECT_CALL(properties, WritePropertyBatch(properties_id, Pointee(Field(&vr::PropertyWrite_t::prop, vr::Prop_NamedIconPathDeviceStandby_String)), _))
        .Times(1);
    EXPECT_CALL(properties, WritePropertyBatch(properties_id, Pointee(Field(&vr::PropertyWrite_t::prop, vr::Prop_NamedIconPathDeviceAlertLow_String)), _))
        .Times(1);

        
    // Check battery info is set
    EXPECT_CALL(properties, WritePropertyBatch(properties_id, Pointee(Field(&vr::PropertyWrite_t::prop, vr::Prop_DeviceProvidesBatteryStatus_Bool)), _))
        .Times(1);
    EXPECT_CALL(properties, WritePropertyBatch(properties_id, Pointee(Field(&vr::PropertyWrite_t::prop, vr::Prop_DeviceBatteryPercentage_Float)), _))
        .Times(1);

    // Check input profile is set
    EXPECT_CALL(properties, WritePropertyBatch(properties_id, Pointee(Field(&vr::PropertyWrite_t::prop, vr::Prop_InputProfilePath_String)), _))
        .Times(1);

    // Check controller role is set
    EXPECT_CALL(properties, WritePropertyBatch(properties_id, Pointee(Field(&vr::PropertyWrite_t::prop, vr::Prop_ControllerRoleHint_Int32)), _))
        .Times(1);

    // Check controller can be indentified from the steamvr input window
    EXPECT_CALL(properties, WritePropertyBatch(properties_id, Pointee(Field(&vr::PropertyWrite_t::prop, vr::Prop_Identifiable_Bool)), _))
    .Times(1);

    // Run fn
    controller.Activate(tracked_device_index, &input, &helper);

}
TEST(PenController, UpdatePenPosePostsToServerDriverHost) {
    std::unique_ptr<vr::IVRSettings> settings = std::make_unique<MockVRSettings>();
    std::shared_ptr<SettingsManager> settings_manager = std::make_shared<SettingsManager>(std::make_unique<FileSettingsLoader>(std::make_unique<std::istringstream>("{}"), std::make_unique<std::ostringstream>()));
    std::shared_ptr<NiceMock<MockPenSystem>> pen_system = std::make_shared<NiceMock<MockPenSystem>>();
    std::shared_ptr<MasslessManager> massless_manager = std::make_shared<MasslessManager>(pen_system);


    testing::NiceMock<MockVRProperties> properties;
    ON_CALL(properties, TrackedDeviceToPropertyContainer(2))
        .WillByDefault(Return(1));

    vr::CVRPropertyHelpers helper(&properties);
    testing::NiceMock<MockVRInput> input;

    // Create the controller
    PenController controller(settings_manager, massless_manager);
    controller.Activate(1, &input, &helper);

    vr::DriverPose_t test_pose = { 0 };
    test_pose.result = vr::ETrackingResult::TrackingResult_Running_OK;

    testing::NiceMock<MockVRServerDriverHost> host;

    EXPECT_CALL(host, TrackedDevicePoseUpdated(1, Field(&vr::DriverPose_t::result, test_pose.result), sizeof(test_pose)))
        .Times(1);

    controller.updatePenPose(test_pose, &host);
}
