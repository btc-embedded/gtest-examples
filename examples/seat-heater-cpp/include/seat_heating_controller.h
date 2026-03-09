#pragma once
#include <array>
#include <cstdint>
#include "types.h"

namespace seat_heating {

using tBool = bool;
enum tPowerMgtState { NOK, OK };
using tErrorStatus = char;
using tPower = float;

struct tState {
    tBool prevButtonPressed;
    uint8_t temperatureStage;
};

struct tTemperatureParams {
    tTemperature low;
    tTemperature medium;
    tTemperature high;
};

class SeatHeatingController {
public:
    tBool seatOccupied = false;
    tPowerMgtState powerMgtState = NOK;
    tErrorStatus errorStatus = INVALID;
    tTemperature heatingCoil = 0;
    std::array<tBool, 3> leds = { false, false, false };
    tState state = { false, 0 };
    tTemperatureParams TemperatureParams = { 30, 35, 45 };
    tTemperature currentTemp = 0;
    tPower requestedPower = 0;

    // Default stub: always returns false, override in test
    virtual tBool getButtonPressed() { return false; }
    void step();
    static tBool check_conditions(tBool seatStatus, tPowerMgtState pwMgtstate, tErrorStatus errStatus);
    static tPower request_power(tTemperature currentTemp, tTemperature requestedTemp);
};

} // namespace seat_heating
