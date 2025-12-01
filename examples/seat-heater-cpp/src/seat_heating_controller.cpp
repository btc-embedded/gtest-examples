#include "seat_heating_controller.h"

// Global variables
tBool seatOccupied = false;
tPowerMgtState powerMgtState = tPowerMgtState::NOK;
tErrorStatus errorStatus = INVALID;
tTemperature heatingCoil = 0;
tTemperature currentTemp = 0;
tPower requestedPower = 0;
std::array<tBool, 3> leds = { false, false, false };
tState state = { false, 0 };
tTemperatureParams TemperatureParams = { 30, 35, 45 };

// External stub (to be mocked in tests)

namespace seat_heating {

void SeatHeatingController::step() {
    if (check_conditions(seatOccupied, powerMgtState, errorStatus)) {
        if (!state.prevButtonPressed && getButtonPressed()) {
            state.temperatureStage = (state.temperatureStage + 1) % 3;
        }
        if (state.temperatureStage == 0) {
            heatingCoil = 0;
            leds = { 0, 0, 0 };
        } else if (state.temperatureStage == 1) {
            heatingCoil = TemperatureParams.high;
            leds = { 1, 1, 1 };
        } else if (state.temperatureStage == 2) {
            heatingCoil = TemperatureParams.medium;
            leds = { 1, 1, 0 };
        } else if (state.temperatureStage == 3) {
            heatingCoil = TemperatureParams.low;
            leds = { 1, 0, 0 };
        } else {
            heatingCoil = 0;
            leds = { 0, 0, 0 };
        }
    }
    state.prevButtonPressed = getButtonPressed();
    requestedPower = request_power(currentTemp, heatingCoil);
}

tBool SeatHeatingController::check_conditions(tBool seatStatus, tPowerMgtState pwMgtstate, tErrorStatus errStatus) {
    return (seatStatus && (pwMgtstate == OK) && errStatus == NO_ERROR);
}

tPower SeatHeatingController::request_power(tTemperature currentTemp, tTemperature requestedTemp) {
    float power = 0;
    float diff = static_cast<float>(requestedTemp) - static_cast<float>(currentTemp);
    if (diff < 0) {
        power = 0;
    } else {
        power = (1 - 1 / diff) * 100;
    }
    return (power > 100) ? 100 : power;
}

} // namespace seat_heating
