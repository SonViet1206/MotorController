# MenuManager – Thư viện Menu đa cấp cho Arduino / ESP32

Thư viện C++ OOP tạo hệ thống menu dạng cây (Tree Menu), hỗ trợ nhiều cấp menu con, điều hướng bằng bàn phím và hiển thị trên nhiều loại màn hình thông qua interface.

## 1. Tính năng

- Tạo menu nhiều cấp, không giới hạn cố định độ sâu.
- Một menu có thể chứa nhiều menu con.
- Hỗ trợ di chuyển lên, xuống, vào menu con và quay lại menu cha.
- Thực hiện callback khi chọn mục menu.
- Tự động cuộn khi số mục vượt quá số dòng hiển thị.
- Độc lập với phần cứng hiển thị thông qua `IMenuRenderer`.
- Tích hợp với thư viện `MatrixKeypad`.
- Không sử dụng `delay()` trong logic điều hướng.
- Có thể mở rộng cho TFT hoặc các màn hình khác.

### Các màn hình hỗ trợ

| Màn hình | Renderer | Thư viện nền |
|---|---|---|
| LCD 16×2 Parallel | `LCDRenderer` | LiquidCrystal |
| LCD 16×4 Parallel | `LCDRenderer` | LiquidCrystal |
| LCD 16×2 I2C | `LCDRenderer` | LiquidCrystal_I2C |
| LCD 16×4 I2C | `LCDRenderer` | LiquidCrystal_I2C |
| LCD 20×4 | `LCDRenderer` | LiquidCrystal / LiquidCrystal_I2C |
| OLED SSD1306 I2C | `OLEDRenderer` | U8g2 |
| OLED SSD1306 SPI | `OLEDRenderer` | U8g2 |
| OLED SH1106 I2C/SPI | `OLEDRenderer` | U8g2 |

**Lưu ý:** OLED phải sử dụng constructor U8g2 phù hợp với controller, kích thước và giao tiếp thực tế.

---

## 2. Cấu trúc thư viện

Đặt thư viện trong thư mục `lib/` của PlatformIO:

```text
MyProject/
├── platformio.ini
├── src/
│   └── main.cpp
│
└── lib/
    ├── MenuManager/
    │   ├── MenuItem.h
    │   ├── MenuManager.h
    │   ├── IMenuRenderer.h
    │   │
    │   └── Renderers/
    │       ├── LCDRenderer.h
    │       └── OLEDRenderer.h
    │
    └── MatrixKeypad/
        ├── MatrixKeypad.h
        └── MatrixKeypad.cpp
```

Thư viện `MenuManager` sử dụng các class dạng header-only trong phiên bản này, nên không bắt buộc có file `.cpp`.

---

## 3. Cài đặt thư viện phụ thuộc

### PlatformIO

Đối với OLED, thêm U8g2 vào `platformio.ini`:

```ini
[env:esp32dev]
platform = espressif32
board = esp32dev
framework = arduino

monitor_speed = 115200

lib_deps =
    olikraus/U8g2@^2.36.18
```

Đối với LCD Parallel, sử dụng thư viện `LiquidCrystal` tương thích với framework Arduino.

Đối với LCD I2C, cài thêm thư viện `LiquidCrystal_I2C` tương thích với module LCD và API được sử dụng.

Nếu sử dụng riêng LCD, không cần cài U8g2.

---

## 4. Nguyên lý hoạt động

Thư viện được chia thành ba thành phần chính.

### MenuItem

Đại diện cho một mục menu.

Một `MenuItem` có thể chứa các mục con hoặc một callback.

### MenuManager

Quản lý:

- Menu hiện tại.
- Vị trí mục đang được chọn.
- Điều hướng giữa các mục.
- Chuyển vào menu con.
- Quay về menu cha.
- Yêu cầu renderer cập nhật màn hình.

### IMenuRenderer

Interface chịu trách nhiệm hiển thị.

```cpp
class IMenuRenderer
{
public:
    virtual ~IMenuRenderer() = default;

    virtual void begin() = 0;

    virtual uint8_t visibleRows() const = 0;

    virtual void startFrame(const char* title) = 0;

    virtual void drawItem(
        uint8_t row,
        const char* text,
        bool selected,
        bool hasChildren
    ) = 0;

    virtual void endFrame() = 0;
};
```

`MenuManager` chỉ phụ thuộc vào interface, không phụ thuộc trực tiếp vào thư viện LCD hoặc OLED.

---

## 5. Tạo cây menu

Ví dụ hệ thống menu điều khiển robot:

```text
MAIN MENU
├── Motor Control
│   ├── Motor 1
│   │   ├── Start
│   │   ├── Stop
│   │   └── PID Settings
│   │       ├── Kp
│   │       ├── Ki
│   │       └── Kd
│   └── Motor 2
├── Sensors
│   ├── Temperature
│   └── Encoder
└── System
    ├── Network
    └── Display
```

### Khai báo các mục menu

```cpp
#include "MenuItem.h"

MenuItem root("MAIN MENU");

MenuItem motor("Motor Control");
MenuItem sensors("Sensors");
MenuItem system("System");

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
```

### Liên kết các menu

```cpp
void buildMenu()
{
    root.addChild(motor);
    root.addChild(sensors);
    root.addChild(system);

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

    system.addChild(network);
    system.addChild(displaySettings);
}
```

`addChild()` trả về `true` nếu thêm thành công.

Mỗi `MenuItem` chỉ được gắn vào một vị trí trong cây. Các đối tượng phải tồn tại trong suốt thời gian menu sử dụng chúng.

---

## 6. Thực hiện hành động khi chọn menu

Đối với các mục không chứa menu con, có thể truyền callback vào constructor.

```cpp
void startMotor()
{
    Serial.println("Motor START");
}

void stopMotor()
{
    Serial.println("Motor STOP");
}

MenuItem start("Start", startMotor);
MenuItem stop("Stop", stopMotor);
```

Khi người dùng chọn `Start` và nhấn Enter, hàm `startMotor()` được thực thi.

Callback trong phiên bản này có dạng:

```cpp
void functionName();
```

Chưa hỗ trợ trực tiếp lambda có capture hoặc callback nhận tham số.

---

## 7. Sử dụng OLED SSD1306 I2C

Ví dụ màn hình OLED 128×64 sử dụng ESP32.

### Khai báo màn hình

```cpp
#include <Arduino.h>
#include <U8g2lib.h>

#include "MenuItem.h"
#include "MenuManager.h"
#include "Renderers/OLEDRenderer.h"

U8G2_SSD1306_128X64_NONAME_F_HW_I2C display(
    U8G2_R0,
    U8X8_PIN_NONE
);

OLEDRenderer renderer(display);

MenuItem root("MAIN MENU");
MenuItem motor("Motor");
MenuItem sensors("Sensors");

MenuManager menu(root, renderer);
```

### Hàm setup

```cpp
void setup()
{
    Serial.begin(115200);

    root.addChild(motor);
    root.addChild(sensors);

    menu.begin();
}
```

`menu.begin()` sẽ gọi `renderer.begin()`, từ đó khởi tạo màn hình OLED.

Nếu sử dụng chân I2C tùy chỉnh, cần cấu hình `Wire` phù hợp trước khi khởi tạo màn hình.

### Điều hướng thử bằng Serial

```cpp
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
```

Mở Serial Monitor ở tốc độ 115200 baud.

Các phím điều khiển:

| Phím | Chức năng |
|---|---|
| `w` | Lên |
| `s` | Xuống |
| `e` | Enter |
| `b` | Back |

---

## 8. Sử dụng OLED SPI

Nếu OLED SSD1306 sử dụng SPI 4 dây, thay khai báo màn hình bằng:

```cpp
U8G2_SSD1306_128X64_NONAME_F_4W_HW_SPI display(
    U8G2_R0,
    /* CS */ 5,
    /* DC */ 16,
    /* RESET */ 17
);

OLEDRenderer renderer(display);
```

Các chân SCK và MOSI sử dụng cấu hình SPI phần cứng phù hợp với ESP32.

Không cần sửa `MenuItem` hoặc `MenuManager`.

---

## 9. Sử dụng LCD 16×2 I2C

Ví dụ LCD dùng module PCF8574, địa chỉ I2C `0x27`.

```cpp
#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#include "MenuItem.h"
#include "MenuManager.h"
#include "Renderers/LCDRenderer.h"

LiquidCrystal_I2C lcd(0x27, 16, 2);

LCDRenderer<LiquidCrystal_I2C> renderer(
    lcd, 16, 2
);

MenuItem root("MAIN MENU");
MenuItem motor("Motor");
MenuItem sensors("Sensors");

MenuManager menu(root, renderer);

void setup()
{
    Wire.begin();

    lcd.init();
    lcd.backlight();

    root.addChild(motor);
    root.addChild(sensors);

    menu.begin();
}

void loop()
{
    // Xử lý nút nhấn hoặc bàn phím ở đây.
}
```

`LCDRenderer` không tự gọi `lcd.init()`, vì mỗi thư viện LCD có API khởi tạo khác nhau.

### LCD 16×4 I2C

```cpp
LiquidCrystal_I2C lcd(0x27, 16, 4);

LCDRenderer<LiquidCrystal_I2C> renderer(
    lcd, 16, 4
);
```

### LCD 20×4 I2C

```cpp
LiquidCrystal_I2C lcd(0x27, 20, 4);

LCDRenderer<LiquidCrystal_I2C> renderer(
    lcd, 20, 4
);
```

Địa chỉ `0x27` chỉ là ví dụ; module của bạn có thể dùng địa chỉ khác như `0x3F`.

---

## 10. Sử dụng LCD Parallel

Ví dụ LCD 16×2 giao tiếp 4-bit.

```cpp
#include <LiquidCrystal.h>

#include "MenuItem.h"
#include "MenuManager.h"
#include "Renderers/LCDRenderer.h"

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

LCDRenderer<LiquidCrystal> renderer(
    lcd, 16, 2
);

MenuItem root("MAIN MENU");
MenuItem motor("Motor");

MenuManager menu(root, renderer);

void setup()
{
    lcd.begin(16, 2);

    root.addChild(motor);

    menu.begin();
}

void loop()
{
}
```

Thay đổi chân kết nối và kích thước LCD theo phần cứng thực tế.

---

## 11. Kết hợp với MatrixKeypad

Thư viện `MenuManager` không trực tiếp đọc bàn phím.

Thay vào đó, ứng dụng nhận sự kiện từ `MatrixKeypad` rồi gọi các phương thức điều hướng.

Ví dụ:

```cpp
void loop()
{
    keypad.update(millis());

    KeyEvent event;

    while (keypad.getEvent(event))
    {
        if (event.type != KeyEventType::Pressed)
            continue;

        switch (event.key)
        {
            case '2':
                menu.previous();
                break;

            case '8':
                menu.next();
                break;

            case '#':
                menu.enter();
                break;

            case '*':
                menu.back();
                break;
        }
    }
}
```

Quy ước:

| Phím keypad | Chức năng |
|---|---|
| `2` | Lên |
| `8` | Xuống |
| `#` | Enter |
| `*` | Back |

Có thể bổ sung sự kiện `Repeat` để giữ phím và cuộn menu liên tục.

---

## 12. API của MenuManager

| Phương thức | Chức năng |
|---|---|
| `begin()` | Khởi tạo renderer và hiển thị menu |
| `next()` | Chọn mục tiếp theo |
| `previous()` | Chọn mục phía trước |
| `enter()` | Vào menu con hoặc thực thi callback |
| `back()` | Quay về menu cha |
| `render()` | Vẽ lại menu hiện tại |

### API của MenuItem

| Phương thức | Chức năng |
|---|---|
| `addChild(item)` | Thêm menu con |
| `title()` | Lấy tên menu |
| `parent()` | Lấy menu cha |
| `firstChild()` | Lấy menu con đầu tiên |
| `nextSibling()` | Lấy mục kế tiếp cùng cấp |
| `childAt(index)` | Lấy menu con theo vị trí |
| `childCount()` | Đếm số menu con |
| `execute()` | Thực hiện callback |

---

## 13. Lưu ý khi sử dụng

### Bộ nhớ

`MenuItem` sử dụng con trỏ liên kết giữa các node, không tự cấp phát bộ nhớ động.

Nên khai báo các `MenuItem` dưới dạng biến toàn cục hoặc biến có vòng đời đủ dài.

### Cuộn menu

Nếu số mục lớn hơn số dòng hiển thị, `MenuManager` sẽ tự điều chỉnh vị trí hiển thị để mục được chọn luôn nằm trong vùng nhìn thấy.

### LCD ký tự

`LCDRenderer` hiện dành toàn bộ số dòng cho các mục menu, không hiển thị tiêu đề menu cha.

Tên mục dài sẽ bị cắt theo chiều rộng màn hình.

### OLED

`OLEDRenderer` hiện dùng font `u8g2_font_6x10_tf`.

Số dòng hiển thị phụ thuộc vào chiều cao màn hình và `lineHeight`.

Tên menu quá dài có thể tràn sang vùng ký hiệu menu con; phiên bản này chưa tự động cắt chuỗi theo pixel.

### Cập nhật màn hình

Màn hình chỉ được vẽ lại khi gọi `render()` hoặc các phương thức điều hướng.

Với OLED dùng U8g2, việc gửi framebuffer qua I2C/SPI có thể mất thời gian. Không nên gọi `render()` liên tục không cần thiết trong vòng lặp điều khiển PID tốc độ cao.

### Callback

Callback nên thực hiện nhanh, tránh `delay()` dài hoặc vòng lặp chặn.

### Các tính năng chưa triển khai

- Chỉnh sửa trực tiếp giá trị `int`, `float`, `bool`.
- Giao diện nhập số.
- Lưu cấu hình bằng EEPROM/NVS.
- Callback có tham số hoặc lambda capture.
- Tự động hiển thị giá trị cảm biến theo thời gian.
- Renderer TFT.
- Hỗ trợ tiếng Việt có dấu đầy đủ.
- Tối ưu chỉ cập nhật các dòng LCD thay đổi.

Những tính năng này có thể bổ sung mà không cần thay đổi kiến trúc interface.

---

## 14. Mở rộng thêm renderer

Để hỗ trợ một loại màn hình mới, tạo class kế thừa `IMenuRenderer`:

```cpp
class CustomRenderer : public IMenuRenderer
{
public:
    void begin() override
    {
        // Khởi tạo màn hình
    }

    uint8_t visibleRows() const override
    {
        return 4;
    }

    void startFrame(const char* title) override
    {
        // Bắt đầu vẽ
    }

    void drawItem(
        uint8_t row,
        const char* text,
        bool selected,
        bool hasChildren
    ) override
    {
        // Hiển thị một mục menu
    }

    void endFrame() override
    {
        // Hoàn tất cập nhật màn hình
    }
};
```

Sau đó:

```cpp
CustomRenderer renderer;
MenuManager menu(root, renderer);
```

Không cần sửa `MenuManager` hoặc `MenuItem`.

---

## 15. Hướng phát triển

Các phiên bản tiếp theo có thể bổ sung:

- `MenuValueItem<T>` để chỉnh sửa thông số.
- `MenuToggleItem` cho ON/OFF.
- `MenuEnumItem` để chọn một trong nhiều giá trị.
- `MenuActionItem` để thực hiện lệnh.
- `MenuInputHandler` để thống nhất keypad, encoder xoay và nút nhấn.
- `TFTMenuRenderer` cho màn hình màu.
- Cơ chế đánh dấu màn hình cần cập nhật (dirty flag).
- Tự động lưu và khôi phục cấu hình bằng ESP32 Preferences/NVS.

## 16. Yêu cầu

- Arduino Framework.
- ESP32 hoặc board Arduino tương thích.
- C++11 trở lên.
- Thư viện màn hình phù hợp.
- U8g2 nếu sử dụng OLED.

**MenuManager** được thiết kế theo nguyên tắc Dependency Inversion: logic menu chỉ phụ thuộc vào interface hiển thị, cho phép thay đổi phần cứng màn hình mà không cần viết lại hệ thống menu.