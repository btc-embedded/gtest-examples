#include <gtest/gtest.h>

extern "C" {
    #include "seat_heating_controller.h"
}

// Test Fixture for Seat Heating Controller
class SeatHeatingControllerTestSuite : public ::testing::Test {
protected:
    void SetUp() override {
        // Initialize to known state before each test
        seatOccupied = false;
        powerMgtState = NOK;
        errorStatus = INVALID;
        heatingCoil = 0;
        leds[0] = false;
        leds[1] = false;
        leds[2] = false;
        getButtonPressed_stub = false;
    }

    void TearDown() override {
        // Cleanup after each test if needed
    }
};

// ============================================================
// Tests for check_conditions function
// ============================================================

// BTC_LINK_TO_REQ(subterra:data-service:objects:/default/seatheater${WorkItem}SEAT-79)
TEST(CheckConditionsTestSuite, Test_CheckConditions_ReturnsTrue) {
    tBool result = check_conditions(true, OK, NO_ERROR);
    EXPECT_TRUE(result);
}

// TEST(CheckConditionsTestSuite, Test_CheckConditions_ReturnsFalse_1) {
//     tBool result = check_conditions(false, OK, NO_ERROR);
//     EXPECT_FALSE(result);
// }


// ============================================================
// Tests for seat_heating_controller main function
// ============================================================

// BTC_LINK_TO_REQ(subterra:data-service:objects:/default/seatheater${WorkItem}SEAT-82)
// BTC_LINK_TO_REQ(subterra:data-service:objects:/default/seatheater${WorkItem}SEAT-84)
TEST_F(SeatHeatingControllerTestSuite, Test_Controller_CyclesThroughAllStages) {
    seatOccupied = true;
    powerMgtState = OK;
    errorStatus = NO_ERROR;
    
    // First press: 0 -> 1
    getButtonPressed_stub = true;
    seat_heating_controller();
    EXPECT_EQ(state.temperatureStage, 1);
    EXPECT_EQ(heatingCoil, 45);
    
    // Release button
    getButtonPressed_stub = false;
    seat_heating_controller();
    
    // Second press: 1 -> 2
    getButtonPressed_stub = true;
    seat_heating_controller();
    EXPECT_EQ(state.temperatureStage, 2);
    EXPECT_EQ(heatingCoil, 35);
    
    // Release button
    getButtonPressed_stub = false;
    seat_heating_controller();
    
    // Third press: 2 -> 3
    getButtonPressed_stub = true;
    seat_heating_controller();
    EXPECT_EQ(state.temperatureStage, 3);
    EXPECT_EQ(heatingCoil, 30);
}

// ============================================================
// Main function
// ============================================================

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
