#pragma once

#include <Arduino.h>
#include "button.h"

namespace Buttons
{
    extern Button *buttons[3];

    void setup();
    void update();
} // namespace Buttons
