#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "../src/seat_heating_controller.h"

using namespace seat_heating;

class MockSeatHeatingController : public SeatHeatingController {
public:
    MOCK_METHOD(tBool, getButtonPressed, (), (override));
};

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
};


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

    // First press: 0 -> 1
    buttonPressed = true;
    controller.step();
    EXPECT_EQ(controller.state.temperatureStage, 1);
    EXPECT_EQ(controller.heatingCoil, 45);

    // Release button
    buttonPressed = false;
    controller.step();

    // Second press: 1 -> 2
    buttonPressed = true;
    controller.step();
    EXPECT_EQ(controller.state.temperatureStage, 2);
    EXPECT_EQ(controller.heatingCoil, 35);

    // Release button
    buttonPressed = false;
    controller.step();

    // Third press: 2 -> 3
    buttonPressed = true;
    controller.step();
    EXPECT_EQ(controller.state.temperatureStage, 3);
    EXPECT_EQ(controller.heatingCoil, 30);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
