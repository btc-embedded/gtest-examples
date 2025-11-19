#ifndef SEAT_HEATING_CONTROLLER_C_
#define SEAT_HEATING_CONTROLLER_C_

#include "seat_heating_controller.h"


// ----------- Inputs -----------
// seatOccupied         : tBool         // Whether the seat is occupied
// powerMgtState        : tPowerMgtState// Power management state
// errorStatus          : tErrorStatus  // Error status
// getButtonPressed()   : tBool         // Button press input

// ----------- Outputs ----------
// leds[3]              : tBool[3]      // LED indicators
// heatingCoil          : tTemperature  // Heating coil temperature
// requestedPower               : tPower             // Requested power level

// ----------- Parameters for Temperatures -----------
// TemperatureParams.low        : tTemperatureParams // Low Temperature
// TemperatureParams.medium     : tTemperatureParams // Medium Temperature
// TemperatureParams.high       : tTemperatureParams // High Temperature

// ----------- FunctionCall Graph -----------
/*
seat_heating_controller()
    ├─ check_conditions()
    ├─ getButtonPressed() [external]
    └─ request_power()
*/

// Global variables
tBool seatOccupied;
tPowerMgtState powerMgtState;
tErrorStatus errorStatus;
tTemperature heatingCoil;
tTemperature currentTemp;
tPower requestedPower;  
tBool leds[3] = { 0, 0, 0 };

// Parameters
tTemperatureParams TemperatureParams = { 30, 35, 45 };

// Application state
tState state = { 0, 0 };

/**
 * Main step function
 */
void seat_heating_controller(void) {

    if (check_conditions(seatOccupied, powerMgtState, errorStatus)) {

        // detect rising edge ? update temp stage
        if (!state.prevButtonPressed && getButtonPressed()) {
            // cycle through temperature stages (0-1-2-3-0)
            state.temperatureStage = (state.temperatureStage + 1) % 3;
        }

        // determine the heating coil temperature and update the LEDs
        if (state.temperatureStage == 0) {
            heatingCoil = 0;
            leds[0] = 0;
            leds[1] = 0;
            leds[2] = 0;
        } else if (state.temperatureStage == 1) {
            heatingCoil = TemperatureParams.high;
            leds[0] = 1;
            leds[1] = 1;
            leds[2] = 1;
        } else if (state.temperatureStage == 2) {
            heatingCoil = TemperatureParams.medium;
            leds[0] = 1;
            leds[1] = 1;
            leds[2] = 0;
        } else if (state.temperatureStage == 3) {
            heatingCoil = TemperatureParams.low;
            leds[0] = 1;
            leds[1] = 0;
            leds[2] = 0;
        } else {
            heatingCoil = 0;
            leds[0] = 0;
            leds[1] = 0;
            leds[2] = 0;
        }
    }

    // store values to access the current state in future calls
    state.prevButtonPressed = getButtonPressed();

    // determine requested power based on current and desired temperature
    requestedPower = request_power(currentTemp, heatingCoil); 
}

/**
 * Check activation conditions
 */
tBool check_conditions(tBool seatStatus, tPowerMgtState pwMgtstate, tErrorStatus errStatus){
    return (seatStatus && (pwMgtstate == OK) && errStatus == NO_ERROR);
}

/**
 * Returns requested power (0-100) based on the difference between current and requested temperature.
 */
float request_power(tTemperature currentTemp, tTemperature requestedTemp) {

    float power = 0;
    float diff = (requestedTemp - currentTemp);
    if (diff < 0) {
        power = 0;
    }
    else {
        power = (1 - 1/diff)*100;
    }
    return (power > 100) ? 100 : power;
}

#endif
