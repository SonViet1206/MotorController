#include "MenuItem.h"
#include "MenuManager.h"
#include "Renderers/OLEDRenderer.h"
U8G2_SSD1306_128X64_NONAME_F_HW_I2C display(
    U8G2_R0,
    U8X8_PIN_NONE);
OLEDRenderer renderer(display);

MenuItem root("MAIN MENU");

MenuItem motor("Motor Control");
MenuItem sensors("Sensors");
MenuItem systemMenu("System");

MenuItem motor1("Motor 1");
MenuItem motor2("Motor 2");

MenuItem start("Start");
MenuItem stop("Stop");
MenuItem pid("PID Settings");

MenuItem kp("Kp");
MenuItem ki("Ki");
MenuItem kd("Kd");

MenuItem temperature("Temperature");
MenuItem encoder("Encoder");

MenuItem network("Network");
MenuItem displaySettings("Display");

MenuManager menu(root, renderer);

void buildMenu()
{   
    root.addChild(motor);   
    root.addChild(sensors);
    root.addChild(systemMenu);

    motor.addChild(motor1);
    motor.addChild(motor2);

    motor1.addChild(start);
    motor1.addChild(stop);
    motor1.addChild(pid);

    pid.addChild(kp);
    pid.addChild(ki);
    pid.addChild(kd);

    sensors.addChild(temperature);
    sensors.addChild(encoder);

    systemMenu.addChild(network);
    systemMenu.addChild(displaySettings);
}
void setup()
{
    delay(1000); // Wait for the display to initialize
    Serial.begin(115200);

    buildMenu();
    

    menu.begin();
}

void loop()
{
    if (Serial.available())
    {
        char command = Serial.read();

        switch (command)
        {
        case 'w':
            menu.previous();
            break;

        case 's':
            menu.next();
            break;

        case 'e':
            menu.enter();
            break;

        case 'b':
            menu.back();
            break;
        }
    }
}
