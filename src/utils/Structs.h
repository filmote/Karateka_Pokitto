#pragma once

#include "Pokitto.h"
#include "Enums.h"

using PC = Pokitto::Core;
using PD = Pokitto::Display;


struct SplashScreenVariables {

    uint8_t counter = 0;
    uint8_t buttonCounter = 0;

};

struct TitleScreenVars {

    int8_t arrowCounter = 0;
    uint8_t orbCounter = 0;
    uint8_t fadeCounter = 0;
    int32_t yPos = 0;

};

