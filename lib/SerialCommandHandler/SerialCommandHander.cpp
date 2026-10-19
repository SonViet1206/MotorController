#include "SerialCommandHandler.h"

SerialCommandHandler::SerialCommandHandler(MotorController& controller):controller(controller){

}
void SerialCommandHandler::update(){

    while (Serial.available() > 0){
        char c = Serial.read();
        if (c == '\n')
        {
            buffer.trim();
            if (buffer.length() > 0)
            {
                processCommand(buffer);
            }
            buffer = "";
        }
        else if (c != '\r')
        {
            if (buffer.length() < 64)
            {
                buffer += c;
            }
            else
            {
                buffer = "";
                Serial.println("Command too long");
            }
        }
    }
}

void SerialCommandHandler::processCommand(const String& command)
{
    if(command.startsWith("START "))
    {
        float rpm = command.substring(6).toFloat();

        controller.start(rpm);

        Serial.println("START requested");
    }
    else if (command.startsWith("STOP"))
    {
        controller.stop();
    }
    else if (command == "RESET")
    {
        if (controller.resetFault())
            Serial.println("Reset successful");
        else
            Serial.println("Reset rejected");
    }
    else if (command == "STATUS")
    {
        Serial.println(controller.getStatus());
    }
    else
    {
        Serial.println("Unknown command");
    }
    
}