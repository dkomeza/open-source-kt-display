#include <Arduino.h>

long lastTime = 0;

void setup()
{
    lastTime = millis();
}

void loop()
{

    // Consistent loop time (10ms)
    long currentTime = millis();
    int timeDiff = currentTime - lastTime;
    if (timeDiff < 10)
    {
        delay(10 - timeDiff);
    }
    lastTime = millis();
}