#include "buttons.h"

// Constants
const int BUTTON_POWER_PIN = 27;
const int BUTTON_UP_PIN = 12;
const int BUTTON_DOWN_PIN = 14;

using namespace Buttons;

Button *buttons[] = {};

void Buttons::setup()
{
    // Power button
    Button *powerButton = new Button(BUTTON_POWER_PIN);
    powerButton->onClick([]()
                         { Serial.println("Power button clicked"); });
    powerButton->onDoubleClick([]()
                               { Serial.println("Power button double clicked"); });
    powerButton->onLongPressStart([]()
                                  { Serial.println("Power button long press started"); });
    powerButton->onLongPress([]()
                             { Serial.println("Power button long press"); });
    powerButton->onLongPressStop([]()
                                 { Serial.println("Power button long press stopped"); });
    buttons[0] = powerButton;

    // Up button
    Button *upButton = new Button(BUTTON_UP_PIN);
    upButton->onClick([]()
                      { Serial.println("Up button clicked"); });
    upButton->onDoubleClick([]()
                            { Serial.println("Up button double clicked"); });
    upButton->onLongPressStart([]()
                               { Serial.println("Up button long press started"); });
    upButton->onLongPress([]()
                          { Serial.println("Up button long press"); });
    upButton->onLongPressStop([]()
                              { Serial.println("Up button long press stopped"); });
    buttons[1] = upButton;

    // Down button
    Button *downButton = new Button(BUTTON_DOWN_PIN);
    downButton->onClick([]()
                        { Serial.println("Down button clicked"); });
    downButton->onDoubleClick([]()
                              { Serial.println("Down button double clicked"); });
    downButton->onLongPressStart([]()
                                 { Serial.println("Down button long press started"); });
    downButton->onLongPress([]()
                            { Serial.println("Down button long press"); });
    downButton->onLongPressStop([]()
                                { Serial.println("Down button long press stopped"); });
    buttons[2] = downButton;
}

void Buttons::update()
{
    for (int i = 0; i < 3; i++)
    {
        buttons[i]->update();
    }
}