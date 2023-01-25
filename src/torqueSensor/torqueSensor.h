#ifndef TORQUESENSOR_H
#define TORQUESENSOR_H

#define TORQUE_ARRAY_SIZE 40
#define TORQUE_OUTPUT_TABLE_SIZE 5

#define TORQUE_INPUT_PIN 32
#define TORQUE_OUTPUT_PIN 25

#include <Arduino.h>

class TorqueSensor {
    public:
        TorqueSensor();
        int currentTorque = 0;
        int torqueVoltage = 0;
        const int torqueOutputTable[TORQUE_OUTPUT_TABLE_SIZE] = {55, 70, 90, 125, 163};
        int torqueArray[TORQUE_ARRAY_SIZE];
        
        void toggleTorqueSensor();
        void handleTorqueSensor();
    private:
        void populateTorqueArray();
        void shiftTorqueArray(int value);
        int torqueArrayMax();
        int calculateTorqueOutput(int torque);
}

#endif