# MatrixKeypad — Thư viện bàn phím ma trận Arduino / ESP32

Thư viện C++ OOP dùng để đọc bàn phím ma trận trên Arduino và ESP32, hỗ trợ nhiều kích thước bàn phím, chống dội phím và xử lý sự kiện mà không sử dụng `delay()` trong vòng lặp chính.

## 1. Tính năng

- Hỗ trợ bàn phím 3×4, 4×4, 5×5 và các kích thước tùy chỉnh.
- Debounce độc lập cho từng phím.
- Phát hiện 4 loại sự kiện: `Pressed`, `Released`, `LongPress`, `Repeat`.
- Hỗ trợ nhiều phím được nhấn cùng lúc.
- Sử dụng hàng đợi FIFO tối đa 32 sự kiện.
- Phát hiện một số tổ hợp có nguy cơ ghosting.
- Sử dụng `millis()` để cập nhật không chặn.
- Phù hợp tích hợp với PID, MotorController và các hệ thống robot.

## 2. Cấu trúc thư viện

```text
Project/
├── platformio.ini
├── lib/
│   └── MatrixKeypad/
│       ├── MatrixKeypad.h
│       ├── MatrixKeypad.cpp
│       └── README.md
└── src/
    └── main.cpp
```

## 3. Kết nối phần cứng

Ví dụ bàn phím 4×4 kết nối với ESP32 DevKit:

| Bàn phím | GPIO ESP32 |
|---|---|
| R1 | GPIO13 |
| R2 | GPIO14 |
| R3 | GPIO27 |
| R4 | GPIO26 |
| C1 | GPIO25 |
| C2 | GPIO33 |
| C3 | GPIO32 |
| C4 | GPIO23 |

Các chân GPIO trên chỉ là ví dụ. Cần kiểm tra chân khả dụng trên board ESP32 đang sử dụng.

Bàn phím ma trận cơ khí thông thường không cần cấp nguồn VCC/GND riêng. Thư viện sử dụng `INPUT_PULLUP` cho các cột.

## 4. Khởi tạo bàn phím 4×4

```cpp
#include <Arduino.h>
#include <MatrixKeypad.h>

const uint8_t rowPins[] = {13, 14, 27, 26};
const uint8_t colPins[] = {25, 33, 32, 23};

const char keyMap[4][4] =
{
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}
};

MatrixKeypad keypad(
    4,
    4,
    rowPins,
    colPins,
    &keyMap[0][0]
);
```

Các mảng cấu hình phải tồn tại trong suốt thời gian sử dụng đối tượng `keypad`.

## 5. Cấu hình thư viện

```cpp
void setup()
{
    Serial.begin(115200);

    if (!keypad.begin())
    {
        Serial.println("Keypad initialization failed!");
        return;
    }

    keypad.setScanInterval(5);
    keypad.setDebounceTime(20);
    keypad.setLongPressTime(800);
    keypad.setRepeatTime(1000, 150);
}
```

| Hàm | Ý nghĩa |
|---|---|
| `begin()` | Khởi tạo GPIO và trạng thái bàn phím |
| `setScanInterval(5)` | Quét toàn bộ ma trận mỗi 5 ms |
| `setDebounceTime(20)` | Chống dội trong 20 ms |
| `setLongPressTime(800)` | Nhấn giữ đủ 800 ms |
| `setRepeatTime(1000, 150)` | Repeat đầu tiên sau 1000 ms, sau đó mỗi 150 ms |

Đơn vị thời gian là mili giây (ms).

Đặt `repeatInterval = 0` để tắt Key Repeat.

## 6. Đọc sự kiện bàn phím

Trong `loop()`, luôn gọi:

```cpp
keypad.update(millis());
```

Đọc sự kiện bằng:

```cpp
KeyEvent event;

while (keypad.getEvent(event))
{
    switch (event.type)
    {
        case KeyEventType::Pressed:
            Serial.printf("Pressed: %c\n", event.key);
            break;

        case KeyEventType::Released:
            Serial.printf("Released: %c\n", event.key);
            break;

        case KeyEventType::LongPress:
            Serial.printf("LongPress: %c\n", event.key);
            break;

        case KeyEventType::Repeat:
            Serial.printf("Repeat: %c\n", event.key);
            break;
    }
}
```

Các sự kiện:

| Sự kiện | Mô tả |
|---|---|
| `Pressed` | Phím vừa được nhấn và đã qua debounce |
| `Released` | Phím vừa được nhả và đã qua debounce |
| `LongPress` | Phím được giữ đủ thời gian quy định |
| `Repeat` | Phím tiếp tục được giữ và đến thời điểm lặp |

## 7. Ví dụ main.cpp hoàn chỉnh

```cpp
#include <Arduino.h>
#include <MatrixKeypad.h>

const uint8_t rowPins[] = {13, 14, 27, 26};
const uint8_t colPins[] = {25, 33, 32, 23};

const char keyMap[4][4] =
{
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}
};

MatrixKeypad keypad(
    4, 4,
    rowPins,
    colPins,
    &keyMap[0][0]
);

void setup()
{
    Serial.begin(115200);

    if (!keypad.begin())
    {
        Serial.println("Keypad init failed!");
        return;
    }

    keypad.setScanInterval(5);
    keypad.setDebounceTime(20);
    keypad.setLongPressTime(800);
    keypad.setRepeatTime(1000, 150);

    Serial.println("MatrixKeypad Ready!");
}

void loop()
{
    keypad.update(millis());

    KeyEvent event;

    while (keypad.getEvent(event))
    {
        switch (event.type)
        {
            case KeyEventType::Pressed:
                Serial.printf("Pressed: %c\n", event.key);
                break;

            case KeyEventType::Released:
                Serial.printf("Released: %c\n", event.key);
                break;

            case KeyEventType::LongPress:
                Serial.printf("LongPress: %c\n", event.key);
                break;

            case KeyEventType::Repeat:
                Serial.printf("Repeat: %c\n", event.key);
                break;
        }
    }

    if (keypad.hasQueueOverflowed())
    {
        Serial.println("Keypad queue overflow!");
        keypad.clearQueueOverflow();
    }
}
```

## 8. Kiểm tra trạng thái phím trực tiếp

Không bắt buộc phải dùng sự kiện. Có thể kiểm tra trực tiếp:

```cpp
if (keypad.isPressed('5'))
{
    // Phím 5 đang được nhấn
}

if (keypad.isLongPressed('A'))
{
    // Phím A đã đạt thời gian nhấn giữ
}

uint16_t count = keypad.getPressedCount();
```

Lưu ý: `isLongPressed()` trả về `true` trong suốt thời gian phím tiếp tục được giữ sau khi đạt ngưỡng. Nếu chỉ muốn xử lý một lần, hãy dùng sự kiện `LongPress`.

## 9. Sử dụng bàn phím 4×3

Chỉ cần thay cấu hình:

```cpp
const uint8_t rowPins[] = {13, 14, 27, 26};
const uint8_t colPins[] = {25, 33, 32};

const char keyMap[4][3] =
{
    {'1', '2', '3'},
    {'4', '5', '6'},
    {'7', '8', '9'},
    {'*', '0', '#'}
};

MatrixKeypad keypad(
    4, 3,
    rowPins,
    colPins,
    &keyMap[0][0]
);
```

Không cần sửa mã nguồn thư viện.

## 10. Danh sách API

| API | Chức năng |
|---|---|
| `bool begin()` | Khởi tạo thư viện |
| `void update(uint32_t now)` | Cập nhật trạng thái bàn phím |
| `void setScanInterval(uint32_t ms)` | Cài đặt chu kỳ quét |
| `void setDebounceTime(uint32_t ms)` | Cài đặt thời gian chống dội |
| `void setLongPressTime(uint32_t ms)` | Cài đặt thời gian nhấn giữ |
| `void setRepeatTime(uint32_t delayMs, uint32_t intervalMs)` | Cài đặt lặp phím |
| `bool getEvent(KeyEvent& event)` | Lấy sự kiện từ FIFO |
| `bool isPressed(char key) const` | Kiểm tra phím đang nhấn |
| `bool isLongPressed(char key) const` | Kiểm tra trạng thái nhấn giữ |
| `uint16_t getPressedCount() const` | Đếm số phím đang nhấn |
| `bool isInitialized() const` | Kiểm tra khởi tạo |
| `bool hasGhosting() const` | Kiểm tra tổ hợp phím mơ hồ |
| `bool hasQueueOverflowed() const` | Kiểm tra tràn hàng đợi |
| `void clearQueueOverflow()` | Xóa cờ tràn FIFO |

## 11. Ghosting và Multi-key

Thư viện hỗ trợ nhiều phím đồng thời nhưng có giới hạn của phần cứng bàn phím ma trận.

Nếu bàn phím không có diode chống ghosting, một số tổ hợp ba phím trở lên có thể tạo ra tín hiệu phím ảo.

Thư viện phát hiện một số tổ hợp mơ hồ và bỏ qua lần quét đó.

Trong khi ghosting được phát hiện, trạng thái phím đã xác nhận có thể tạm giữ nguyên. Do đó, ứng dụng cần lưu ý rằng `isPressed()` không nhất thiết phản ánh trạng thái vật lý mới nhất khi có ghosting.

Để hỗ trợ nhiều phím đồng thời đáng tin cậy, nên sử dụng diode chống ghosting cho từng phím.

## 12. FIFO và giới hạn bộ nhớ

Thư viện sử dụng hàng đợi tối đa 32 sự kiện.

Nếu ứng dụng không đọc sự kiện đủ nhanh, hàng đợi có thể đầy và sự kiện mới bị bỏ qua.

Kiểm tra bằng:

```cpp
if (keypad.hasQueueOverflowed())
{
    Serial.println("FIFO Overflow");
    keypad.clearQueueOverflow();
}
```

Thư viện cấp phát bộ nhớ cho trạng thái từng phím khi khởi tạo đối tượng và không cấp phát lại trong `update()`.

Mã nguồn hiện tại giả định các ký tự trong `keyMap` là duy nhất để các hàm `isPressed(char)` và `isLongPressed(char)` xác định đúng một phím.

## 13. Lưu ý khi tích hợp với robot

- Gọi `keypad.update(millis())` thường xuyên.
- Không đặt `delay()` dài trong `loop()`.
- Xử lý sự kiện bằng `getEvent()` để tránh mất dữ liệu do FIFO đầy.
- Tránh dùng GPIO đang phục vụ encoder, PWM hoặc giao tiếp khác.
- Không dùng riêng bàn phím ma trận làm thiết bị dừng khẩn cấp.
- Kiểm thử kỹ tổ hợp nhiều phím và trạng thái mất kết nối trước khi điều khiển động cơ thực tế.

## 14. Yêu cầu hệ thống

- Arduino framework.
- ESP32 hoặc board Arduino tương thích.
- Trình biên dịch hỗ trợ C++11 trở lên.
- PlatformIO hoặc Arduino IDE.

Không cần cài thêm thư viện bên thứ ba.

---

**MatrixKeypad** — Thư viện C++ OOP phục vụ các dự án Arduino, ESP32, điều khiển động cơ và robot.