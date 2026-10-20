#include "MatrixKeypad.h"
#include <new>

MatrixKeypad::MatrixKeypad(
    uint8_t rows,
    uint8_t cols,
    const uint8_t* rowPins,
    const uint8_t* colPins,
    const char* keyMap
)
    : rows(rows),
      cols(cols),
      rowPins(rowPins),
      colPins(colPins),
      keyMap(keyMap)
{
    if (rows == 0 || cols == 0 ||
        rowPins == nullptr ||
        colPins == nullptr ||
        keyMap == nullptr)
    {
        return;
    }

    uint16_t total = (uint16_t)rows * cols;

    keyStates = new (std::nothrow) KeyState[total];
    scanBuffer = new (std::nothrow) bool[total];

    if (!keyStates || !scanBuffer)
    {
        delete[] keyStates;
        delete[] scanBuffer;

        keyStates = nullptr;
        scanBuffer = nullptr;
    }
}

MatrixKeypad::~MatrixKeypad()
{
    delete[] keyStates;
    delete[] scanBuffer;
}

bool MatrixKeypad::begin()
{
    initialized = false;

    if (!keyStates || !scanBuffer)
        return false;

    for (uint8_t r = 0; r < rows; r++)
    {
        pinMode(rowPins[r], INPUT);
    }

    for (uint8_t c = 0; c < cols; c++)
    {
        pinMode(colPins[c], INPUT_PULLUP);
    }

    uint16_t total = (uint16_t)rows * cols;

    for (uint16_t i = 0; i < total; i++)
    {
        keyStates[i] = KeyState{};
        scanBuffer[i] = false;
    }

    queueHead = 0;
    queueTail = 0;
    queueCount = 0;
    queueOverflow = false;
    ghostingDetected = false;

    firstScan = true;
    initialized = true;

    return true;
}

void MatrixKeypad::setScanInterval(uint32_t ms)
{
    if (ms > 0)
        scanInterval = ms;
}

void MatrixKeypad::setDebounceTime(uint32_t ms)
{
    debounceTime = ms;
}

void MatrixKeypad::setLongPressTime(uint32_t ms)
{
    longPressTime = ms;
}

void MatrixKeypad::setRepeatTime(
    uint32_t delayMs,
    uint32_t intervalMs)
{
    repeatDelay = delayMs;
    repeatInterval = intervalMs;
}

void MatrixKeypad::scanMatrix()
{
    for (uint8_t r = 0; r < rows; r++)
    {
        digitalWrite(rowPins[r], LOW);
        pinMode(rowPins[r], OUTPUT);

        // Cho tín hiệu GPIO ổn định
        delayMicroseconds(5);

        for (uint8_t c = 0; c < cols; c++)
        {
            uint16_t index = (uint16_t)r * cols + c;

            scanBuffer[index] =
                (digitalRead(colPins[c]) == LOW);
        }

        pinMode(rowPins[r], INPUT);
    }
}

bool MatrixKeypad::detectGhosting() const
{
    // Nếu hai hàng cùng có ít nhất hai cột
    // được nhận là đang nhấn, tổ hợp có nguy cơ ghosting.
    for (uint8_t r1 = 0; r1 < rows; r1++)
    {
        for (uint16_t r2 = (uint16_t)r1 + 1;
             r2 < rows; r2++)
        {
            uint8_t commonCols = 0;

            for (uint8_t c = 0; c < cols; c++)
            {
                uint16_t i1 = (uint16_t)r1 * cols + c;
                uint16_t i2 = r2 * cols + c;

                if (scanBuffer[i1] && scanBuffer[i2])
                {
                    commonCols++;

                    if (commonCols >= 2)
                        return true;
                }
            }
        }
    }

    return false;
}

void MatrixKeypad::processDebounce(uint32_t now)
{
    uint16_t total = (uint16_t)rows * cols;

    for (uint16_t i = 0; i < total; i++)
    {
        KeyState& state = keyStates[i];

        if (state.rawPressed != state.lastRawPressed)
        {
            state.lastRawPressed = state.rawPressed;
            state.lastChangeTime = now;
        }

        if (now - state.lastChangeTime < debounceTime)
            continue;

        if (state.stablePressed == state.rawPressed)
            continue;

        state.stablePressed = state.rawPressed;

        if (state.stablePressed)
        {
            state.pressedTime = now;
            state.lastRepeatTime = now;
            state.longPressTriggered = false;
            state.repeatStarted = false;

            pushEvent(
                keyMap[i],
                KeyEventType::Pressed,
                now
            );
        }
        else
        {
            pushEvent(
                keyMap[i],
                KeyEventType::Released,
                now
            );
        }
    }
}

void MatrixKeypad::processHold(uint32_t now)
{
    uint16_t total = (uint16_t)rows * cols;

    for (uint16_t i = 0; i < total; i++)
    {
        KeyState& state = keyStates[i];

        if (!state.stablePressed || !state.rawPressed)
            continue;

        uint32_t heldTime = now - state.pressedTime;

        if (!state.longPressTriggered &&
            heldTime >= longPressTime)
        {
            state.longPressTriggered = true;

            pushEvent(
                keyMap[i],
                KeyEventType::LongPress,
                now
            );
        }

        if (repeatInterval == 0)
            continue;

        if (!state.repeatStarted)
        {
            if (heldTime >= repeatDelay)
            {
                state.repeatStarted = true;
                state.lastRepeatTime = now;

                pushEvent(
                    keyMap[i],
                    KeyEventType::Repeat,
                    now
                );
            }
        }
        else if (now - state.lastRepeatTime >= repeatInterval)
        {
            state.lastRepeatTime = now;

            pushEvent(
                keyMap[i],
                KeyEventType::Repeat,
                now
            );
        }
    }
}

void MatrixKeypad::update(uint32_t now)
{
    if (!initialized)
        return;

    if (!firstScan &&
        now - previousScanTime < scanInterval)
    {
        return;
    }

    firstScan = false;
    previousScanTime = now;

    scanMatrix();

    ghostingDetected = detectGhosting();

    if (ghostingDetected)
    {
        // Không cập nhật trạng thái từ một lần quét mơ hồ.
        // Các phím đã được xác nhận vẫn giữ trạng thái cũ.
        // Không phát sinh LongPress/Repeat khi mơ hồ.
        return;
    }

    uint16_t total = (uint16_t)rows * cols;

    for (uint16_t i = 0; i < total; i++)
    {
        keyStates[i].rawPressed = scanBuffer[i];
    }

    processDebounce(now);
    processHold(now);
}

void MatrixKeypad::pushEvent(
    char key,
    KeyEventType type,
    uint32_t now)
{
    if (queueCount >= EVENT_QUEUE_SIZE)
    {
        queueOverflow = true;
        return;
    }

    eventQueue[queueTail] = {key, type, now};

    queueTail =
        (queueTail + 1) % EVENT_QUEUE_SIZE;

    queueCount++;
}

bool MatrixKeypad::getEvent(KeyEvent& event)
{
    if (queueCount == 0)
        return false;

    event = eventQueue[queueHead];

    queueHead =
        (queueHead + 1) % EVENT_QUEUE_SIZE;

    queueCount--;

    return true;
}

int16_t MatrixKeypad::findKeyIndex(char key) const
{
    uint16_t total = (uint16_t)rows * cols;

    for (uint16_t i = 0; i < total; i++)
    {
        if (keyMap[i] == key)
            return (int16_t)i;
    }

    return -1;
}

bool MatrixKeypad::isPressed(char key) const
{
    if (!initialized)
        return false;

    int16_t index = findKeyIndex(key);

    if (index < 0)
        return false;

    return keyStates[index].stablePressed;
}

bool MatrixKeypad::isLongPressed(char key) const
{
    if (!initialized)
        return false;

    int16_t index = findKeyIndex(key);

    if (index < 0)
        return false;

    const KeyState& state = keyStates[index];

    return state.stablePressed &&
           state.longPressTriggered;
}

uint16_t MatrixKeypad::getPressedCount() const
{
    if (!initialized)
        return 0;

    uint16_t count = 0;
    uint16_t total = (uint16_t)rows * cols;

    for (uint16_t i = 0; i < total; i++)
    {
        if (keyStates[i].stablePressed)
            count++;
    }

    return count;
}

bool MatrixKeypad::isInitialized() const
{
    return initialized;
}

bool MatrixKeypad::hasGhosting() const
{
    return ghostingDetected;
}

bool MatrixKeypad::hasQueueOverflowed() const
{
    return queueOverflow;
}

void MatrixKeypad::clearQueueOverflow()
{
    queueOverflow = false;
}