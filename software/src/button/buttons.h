#pragma once

#include <Arduino.h>
#include "button.h"

namespace Buttons
{
    const int BUTTON_POWER_PIN = 27;
    const int BUTTON_UP_PIN = 12;
    const int BUTTON_DOWN_PIN = 14;

    extern Button *buttons[3];

    void setup();
    void update();
} // namespace Buttons
