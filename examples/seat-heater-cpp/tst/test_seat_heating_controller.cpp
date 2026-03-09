/**
 * @file test_seat_heating_controller.cpp
 * @brief Unit Tests for Seat Heating Controller (C++)
 *
 * @details
 *   Tests the C++ SeatHeatingController behavior and verifies requirement
 *   coverage for SEAT-79 through SHC-86.
 *
 */

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "../include/seat_heating_controller.h"

using namespace seat_heating;

/**
 * @brief Mock controller to simulate button presses
 */
class MockSeatHeatingController : public SeatHeatingController {
public:
    MOCK_METHOD(tBool, getButtonPressed, (), (override));
};

/**
 * @brief Test fixture for SeatHeatingController
 */
class SeatHeatingControllerTestSuite : public ::testing::Test {
protected:
    MockSeatHeatingController controller;
    void SetUp() override {
        controller.seatOccupied = false;
        controller.powerMgtState = seat_heating::tPowerMgtState::NOK;
        controller.errorStatus = INVALID;
        controller.heatingCoil = 0;
        controller.leds = { false, false, false };
        controller.state = { false, 0 };
        controller.currentTemp = 0;
    }

    void TearDown() override {
        // No specific teardown needed for this test suite
    }
};

/**
 * @brief Verify activation conditions logic
 */
// BTC_LINK_TO_REQ(subterra:data-service:objects:/default/seatheater${WorkItem}SEAT-79)
TEST_F(SeatHeatingControllerTestSuite, Test_CheckConditions_AllConditionsTrue) {
    EXPECT_TRUE(SeatHeatingController::check_conditions(true, OK, NO_ERROR));
}

/**
 * @brief Verify activation conditions fail when seat unoccupied
 */
// BTC_LINK_TO_REQ(subterra:data-service:objects:/default/seatheater${WorkItem}SEAT-79)
TEST_F(SeatHeatingControllerTestSuite, Test_CheckConditions_SeatUnoccupied) {
    EXPECT_FALSE(SeatHeatingController::check_conditions(false, OK, NO_ERROR));
}

// /**
//  * @brief Verify activation conditions fail when power not OK
//  */
//// BTC_LINK_TO_REQ(subterra:data-service:objects:/default/seatheater${WorkItem}SEAT-79)
// TEST_F(SeatHeatingControllerTestSuite, Test_CheckConditions_PowerNotOk) {
//     EXPECT_FALSE(SeatHeatingController::check_conditions(true, NOK, NO_ERROR));
// }

/**
 * @brief Verify activation conditions fail when error flag set
 */
// BTC_LINK_TO_REQ(subterra:data-service:objects:/default/seatheater${WorkItem}SEAT-79)
TEST_F(SeatHeatingControllerTestSuite, Test_CheckConditions_ErrorSet) {
    EXPECT_FALSE(SeatHeatingController::check_conditions(true, OK, INVALID));
}


/**
 * @brief Verify stage cycling and outputs across all heating stages
 *
 * @details
 *   Validates initial NO HEATING state and cycling through HIGH, MEDIUM,
 *   and LOW stages on successive button presses.
 */
// BTC_LINK_TO_REQ(subterra:data-service:objects:/default/seatheater${WorkItem}SEAT-81)
// BTC_LINK_TO_REQ(subterra:data-service:objects:/default/seatheater${WorkItem}SEAT-82)
// BTC_LINK_TO_REQ(subterra:data-service:objects:/default/seatheater${WorkItem}SEAT-83)
// BTC_LINK_TO_REQ(subterra:data-service:objects:/default/seatheater${WorkItem}SEAT-84)
// BTC_LINK_TO_REQ(subterra:data-service:objects:/default/seatheater${WorkItem}SEAT-85)
// BTC_LINK_TO_REQ(subterra:data-service:objects:/default/seatheater${WorkItem}SEAT-86)
TEST_F(SeatHeatingControllerTestSuite, Test_Controller_CyclesThroughAllStages) {
    controller.seatOccupied = true;
    controller.powerMgtState = seat_heating::OK;
    controller.errorStatus = NO_ERROR;

    // Use a variable to control the button press
    bool buttonPressed = false;
    ON_CALL(controller, getButtonPressed()).WillByDefault([&]() { return buttonPressed; });

    // No press: 0 -> 0
    controller.step();
    EXPECT_EQ(controller.state.temperatureStage, 0);
    EXPECT_EQ(controller.heatingCoil, 0);
    EXPECT_EQ(controller.leds[0], false);
    EXPECT_EQ(controller.leds[1], false);
    EXPECT_EQ(controller.leds[2], false);

    // First press: 0 -> 1
    buttonPressed = true;
    controller.step();
    EXPECT_EQ(controller.state.temperatureStage, 1);
    EXPECT_EQ(controller.heatingCoil, 45);
    EXPECT_EQ(controller.leds[0], true);
    EXPECT_EQ(controller.leds[1], true);
    EXPECT_EQ(controller.leds[2], true);

    // Release button
    buttonPressed = false;
    controller.step();

    // Second press: 1 -> 2
    buttonPressed = true;
    controller.step();
    EXPECT_EQ(controller.state.temperatureStage, 2);
    EXPECT_EQ(controller.heatingCoil, 35);
    EXPECT_EQ(controller.leds[0], true);
    EXPECT_EQ(controller.leds[1], true);
    EXPECT_EQ(controller.leds[2], false);

    // Release button
    buttonPressed = false;
    controller.step();

    // Third press: 2 -> 3
    buttonPressed = true;
    controller.step();
    EXPECT_EQ(controller.state.temperatureStage, 3);
    EXPECT_EQ(controller.heatingCoil, 30);
    EXPECT_EQ(controller.leds[0], true);
    EXPECT_EQ(controller.leds[1], false);
    EXPECT_EQ(controller.leds[2], false);

    // Release button
    buttonPressed = false;
    controller.step();

    // Fourth press: 3 -> 0
    buttonPressed = true;
    controller.step();
    EXPECT_EQ(controller.state.temperatureStage, 0);
    EXPECT_EQ(controller.heatingCoil, 0);
    EXPECT_EQ(controller.leds[0], false);
    EXPECT_EQ(controller.leds[1], false);
    EXPECT_EQ(controller.leds[2], false);
}

/**
 * @brief Verify reactivation restores previous temperature stage
 *
 * @details
 *   The controller should retain the last active stage when it becomes
 *   inactive and later reactivates.
 */
// BTC_LINK_TO_REQ(subterra:data-service:objects:/default/seatheater${WorkItem}SEAT-80)
TEST_F(SeatHeatingControllerTestSuite, Test_Controller_ReactivationRestoresStage) {
    controller.seatOccupied = true;
    controller.powerMgtState = seat_heating::OK;
    controller.errorStatus = NO_ERROR;

    bool buttonPressed = false;
    ON_CALL(controller, getButtonPressed()).WillByDefault([&]() { return buttonPressed; });

    // Move to MEDIUM stage (stage 2)
    buttonPressed = true;
    controller.step();
    buttonPressed = false;
    controller.step();
    buttonPressed = true;
    controller.step();
    EXPECT_EQ(controller.state.temperatureStage, 2);
    EXPECT_EQ(controller.heatingCoil, 35);

    // Deactivate system
    buttonPressed = false;
    controller.seatOccupied = false;
    controller.step();
    EXPECT_EQ(controller.heatingCoil, 0);
    EXPECT_EQ(controller.leds[0], false);
    EXPECT_EQ(controller.leds[1], false);
    EXPECT_EQ(controller.leds[2], false);

    // Reactivate system - stage should be retained
    controller.seatOccupied = true;
    controller.step();
    EXPECT_EQ(controller.state.temperatureStage, 2);
    EXPECT_EQ(controller.heatingCoil, 35);
    EXPECT_EQ(controller.leds[0], true);
    EXPECT_EQ(controller.leds[1], true);
    EXPECT_EQ(controller.leds[2], false);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
