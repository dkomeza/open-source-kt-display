#pragma once

#include <Arduino.h>
#include <TFT_eSPI.h>

class TFT : public TFT_eSPI
{
public:
    TFT(int width, int height) : TFT_eSPI(width, height) {
        init();
    };
};