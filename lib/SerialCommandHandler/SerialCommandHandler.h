#pragma once
#include <Arduino.h>
#include "MotorController.h"

class SerialCommandHandler{
    private:
        MotorController& controller;
        String buffer;
        void processCommand(const String& command);

    public:
        explicit SerialCommandHandler(MotorController& controller);
        void update();
};