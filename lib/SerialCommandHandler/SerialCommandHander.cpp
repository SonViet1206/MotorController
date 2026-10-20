#include "SerialCommandHandler.h"

SerialCommandHandler::SerialCommandHandler(IMotorController & controller):controller(controller){
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
                Serial.println("Command too long, it is smaller than 64 characters");
            }
        }
    }
}

void SerialCommandHandler::processCommand(const String& command)
{
    // Chuẩn hóa chuỗi: Loại bỏ khoảng trắng thừa đầu/cuối và chuyển thành chữ hoa
    String cmd = command;
    cmd.trim();
    cmd.toUpperCase();

    // =================================================================
    // I. CÁC LỆNH CHUNG TIÊU CHUẨN (IEEE 488.2 Common Commands)
    // =================================================================
    if (cmd == "*IDN?") 
    {
        // Trả về thông tin thiết bị: Hãng, Model, Số Serial, Phiên bản Firmware
        Serial.println("RobotController,MotorController,001,1.0");
    }
    else if (cmd == "*RST") 
    {
        controller.stop();
        controller.resetFault();
        controller.setTargetRPM(0);
        Serial.println("Reset system to default state");
    }
    else if (cmd == "*CLS") 
    {
        if(controller.resetFault())
        {
            Serial.println("Fault status cleared");
        }
        else
        {
            Serial.println("No fault to clear");
        }
    }

    // =================================================================
    // II. CÁC LỆNH HỆ THỐNG CON (SCPI Subsystem Commands)
    // =================================================================
    
    // 1. Lệnh điều khiển trạng thái chạy/dừng
    else if (cmd == ":MOTOR:START" || cmd == ":MOT:STAR") 
    {
        controller.start();
        if(controller.isRunning())
        {
            Serial.println("Motor started");
        }
        else
        {
            Serial.println("Motor failed to start (check for faults)");
        }
        // SCPI không bắt buộc phải phản hồi cho lệnh cài đặt (Set), 
        // nhưng nếu muốn bạn có thể gửi phản hồi để debug.
    }
    else if (cmd == ":MOTOR:STOP" || cmd == ":MOT:STOP") 
    {
        controller.stop();
    }
    
    // 2. Lệnh cài đặt tốc độ (Cú pháp: :MOTOR:SPEED <giá trị>)
    else if (cmd.startsWith(":MOTOR:SPEED ") || cmd.startsWith(":MOT:SPEED ") ||
             cmd.startsWith(":MOTOR:SPEE ")  || cmd.startsWith(":MOT:SPEE ")) 
    {
        // Tìm vị trí khoảng trắng cuối cùng để tách giá trị số tham số
        int spaceIdx = cmd.lastIndexOf(' ');
        float rpm = cmd.substring(spaceIdx + 1).toFloat();
        controller.setTargetRPM(rpm);
    }
    
    // 3. Lệnh truy vấn tốc độ hiện tại (Cú pháp có dấu '?')
    else if (cmd == ":MOTOR:SPEED?" || cmd == ":MOT:SPEED?" ||
             cmd == ":MOTOR:SPEE?"  || cmd == ":MOT:SPEE?") 
    {
        float currentRPM = controller.getCurrentRPM();
        Serial.println(currentRPM); // Chuẩn SCPI chỉ trả về đúng giá trị thô (ví dụ: 1500.25)
    }
    
    // 4. Lệnh truy vấn lỗi và Reset lỗi
    else if (cmd == ":MOTOR:FAULT?" || cmd == ":MOT:FAUL?") 
    {
        String faultString = controller.getFault();
        Serial.println(faultString); 
    }
    else if (cmd == ":MOTOR:FAULT:RESET" || cmd == ":MOT:FAUL:RES") 
    {
        if (controller.resetFault())
            Serial.println("1"); // 1 tương đương thành công trong SCPI
        else
            Serial.println("0"); // 0 tương đương bị từ chối
    }
    
    // 5. Lệnh truy vấn trạng thái hoạt động
    else if (cmd == ":MOTOR:STATUS?" || cmd == ":MOT:STAT?") 
    {
        // Trả về 1 nếu đang chạy, 0 nếu đang dừng theo chuẩn định dạng số của SCPI
        Serial.println(controller.isRunning() ? "1" : "0");
    }
    
    // =================================================================
    // III. XỬ LÝ LỖI LỆNH SAI CÚ PHÁP
    // =================================================================
    else 
    {
        // Theo chuẩn SCPI, khi có lỗi cú pháp thiết bị sẽ ghi nhận mã lỗi vào hàng đợi.
        // Ở đây ta in ra mã lỗi tiêu chuẩn -113 (Undefined header)
        Serial.println("-113,\"Undefined header\"");
    }
}

