// -------------- TYPEDEFS ------------

#include <stdbool.h>
typedef bool tBool;

typedef enum { NOK, OK } tPowerMgtState;

typedef char tErrorStatus;
#define NO_ERROR 0
#define TIMEOUT 1
#define INVALID 2

typedef struct {
    tBool prevButtonPressed;
    unsigned char temperatureStage;
} tState;

typedef unsigned char tTemperature;
typedef float tPower;

typedef struct {
    tTemperature low;
    tTemperature medium;
    tTemperature high;
} tTemperatureParams;
