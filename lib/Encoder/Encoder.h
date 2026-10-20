#pragma once
#include <stdint.h>
#include <Arduino.h>
#include <atomic> // Thêm thư viện này
using namespace std;
class Encoder{
  private:
    /// @brief Tổng số bước encoder có dấu
    atomic<int32_t> count{0};
    
    /// @brief trạng thái A,B trước đó
    uint8_t previousState=0;
    /// @brief Số đếm tại lần tính RPM trước
    int32_t previousCount=0;
    /// @brief Tốc độ hiện tại
    float rpm=0.0f;
    /// @brief Số count trên một vòng quay 
    uint32_t countsPerRevolution;
    bool first;

  public:
    explicit Encoder(uint32_t countsPerRevolution);
    void syncRPM();
    void update(bool A, bool B);
    int32_t getCount() const;
    float getRPM() const;
    void calculateRPM(float dt);
    void reset();
    
};