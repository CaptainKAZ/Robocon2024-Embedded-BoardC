#include "controller.h"
#include "math.h"

const ControlInputLim radianInputLim1D = {.low = -M_PI, .high = M_PI};
const ControlInputLim degreeInputLim1D = {.low = -180.0f, .high = 180.0f};