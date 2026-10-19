#include "Encoder.h"

Encoder::Encoder(uint32_t countsPerRevolution): countsPerRevolution(countsPerRevolution),count(0),previousCount(0),rpm(0),first(true){

}
void Encoder::update(bool A, bool B){
  uint8_t currentState = (A << 1) | B;
  uint8_t transition;
  if(first)
  {
    previousState=currentState;
    first=false;
    return;
  }
  transition =(previousState << 2) | currentState;
  switch (transition)
  {
    // Chiều thuận
    case 0b0001:
    case 0b0111:
    case 0b1110:
    case 0b1000:
        count.fetch_add(1,memory_order_release);
        break;

    // Chiều ngược
    case 0b0010:
    case 0b1011:
    case 0b1101:
    case 0b0100:
        count.fetch_sub(1,memory_order_release);
        break;

    default:
        break;
  }
  previousState=currentState;

}
int32_t Encoder::getCount() const
{
    return count.load(std::memory_order_relaxed);
}
void Encoder::calculateRPM(float dt)
{
  if (dt <= 0.0f || countsPerRevolution <= 0) return;

  // 1. Đọc giá trị atomic ĐÚNG 1 LẦN duy nhất
  int32_t currentCount = count.load();

  // 2. Tính chênh lệch xung
  int32_t deltaCount = currentCount - previousCount;

  // 3. Cập nhật previousCount bằng đúng giá trị đã đọc ở bước 1
  previousCount = currentCount;

  // 4. Tính RPM (Giả sử dt tính bằng GIÂY)
  rpm = ((float)deltaCount * 60.0f) / ((float)countsPerRevolution * dt);
}
void Encoder::reset()
{
    count.store(0, std::memory_order_relaxed);
    previousCount=0;
    previousState=0;
    rpm=0;
    first = true;
}
float Encoder:: getRPM() const{
  return rpm;
}
