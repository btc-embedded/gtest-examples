#include <cstdint>

using tBool = bool;
enum class tPowerMgtState : uint8_t { NOK, OK };
using tErrorStatus = int8_t;
constexpr tErrorStatus NO_ERROR = 0;
constexpr tErrorStatus TIMEOUT = 1;
constexpr tErrorStatus INVALID = 2;

struct tState {
    tBool prevButtonPressed;
    unsigned char temperatureStage;
};

using tTemperature = uint8_t;
using tPower = float;

struct tTemperatureParams {
    tTemperature low;
    tTemperature medium;
    tTemperature high;
};
