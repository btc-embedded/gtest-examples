#ifndef SEAT_HEATING_CONTROLLER_H_
#define SEAT_HEATING_CONTROLLER_H_

#include "types.h"

// -------------- VARIABLES ------------
extern tBool seatOccupied;
extern tPowerMgtState powerMgtState;
extern tErrorStatus errorStatus;
extern tTemperature heatingCoil;
extern tBool leds[3];

extern tState state;    
extern tBool getButtonPressed_stub;

// -------------- FUNCTIONS ------------
// internal
void seat_heating_controller(void);
tBool check_conditions(tBool seatStatus, tPowerMgtState pwMgtstate, tErrorStatus errStatus);
tTemperature handle_temperature_stage(int temperatureStage);
// Returns requested power (0-100) based on temperature difference
float request_power(tTemperature currentTemp, tTemperature requestedTemp);

// external
extern tBool getButtonPressed();

#endif