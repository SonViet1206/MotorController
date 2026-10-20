#pragma once

#include <Arduino.h>

enum class KeyEventType : uint8_t
{
    Pressed,
    Released,
    LongPress,
    Repeat
};

struct KeyEvent
{
    char key;
    KeyEventType type;
    uint32_t timestamp;
};

class MatrixKeypad
{
public:
    MatrixKeypad(
        uint8_t rows,
        uint8_t cols,
        const uint8_t* rowPins,
        const uint8_t* colPins,
        const char* keyMap
    );

    ~MatrixKeypad();

    MatrixKeypad(const MatrixKeypad&) = delete;
    MatrixKeypad& operator=(const MatrixKeypad&) = delete;

    bool begin();
    void update(uint32_t now);

    void setScanInterval(uint32_t ms);
    void setDebounceTime(uint32_t ms);
    void setLongPressTime(uint32_t ms);
    void setRepeatTime(uint32_t delayMs, uint32_t intervalMs);

    bool getEvent(KeyEvent& event);

    bool isPressed(char key) const;
    bool isLongPressed(char key) const;

    uint16_t getPressedCount() const;

    bool isInitialized() const;
    bool hasGhosting() const;
    bool hasQueueOverflowed() const;
    void clearQueueOverflow();

private:
    struct KeyState
    {
        bool rawPressed = false;
        bool lastRawPressed = false;
        bool stablePressed = false;

        uint32_t lastChangeTime = 0;
        uint32_t pressedTime = 0;
        uint32_t lastRepeatTime = 0;

        bool longPressTriggered = false;
        bool repeatStarted = false;
    };

    static constexpr uint8_t EVENT_QUEUE_SIZE = 32;

    uint8_t rows;
    uint8_t cols;

    const uint8_t* rowPins;
    const uint8_t* colPins;
    const char* keyMap;

    KeyState* keyStates = nullptr;
    bool* scanBuffer = nullptr;

    KeyEvent eventQueue[EVENT_QUEUE_SIZE];

    uint8_t queueHead = 0;
    uint8_t queueTail = 0;
    uint8_t queueCount = 0;

    uint32_t scanInterval = 5;
    uint32_t debounceTime = 20;
    uint32_t longPressTime = 800;
    uint32_t repeatDelay = 1000;
    uint32_t repeatInterval = 150;

    uint32_t previousScanTime = 0;

    bool initialized = false;
    bool firstScan = true;
    bool ghostingDetected = false;
    bool queueOverflow = false;

    void scanMatrix();
    bool detectGhosting() const;

    void processDebounce(uint32_t now);
    void processHold(uint32_t now);

    void pushEvent(
        char key,
        KeyEventType type,
        uint32_t now
    );

    int16_t findKeyIndex(char key) const;
};