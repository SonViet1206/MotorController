#pragma once
#include <Arduino.h>
#include "IMotorController.h"

class SerialCommandHandler{
    private:

        String buffer;
        IMotorController& controller;
        void processCommand(const String& command);

    public:
        explicit SerialCommandHandler(IMotorController& controller);
        void update();
};