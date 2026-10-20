#include "Encoder.h"

Encoder::Encoder(uint32_t countsPerRevolution) : countsPerRevolution(countsPerRevolution), count(0), previousCount(0), rpm(0), first(true)
{
}
void Encoder::syncRPM()
{
  previousCount = count.load(std::memory_order_relaxed);
  rpm = 0;
}
void Encoder::update(bool A, bool B)
{
  uint8_t currentState = (A << 1) | B;
  uint8_t transition;
  if (first)
  {
    previousState = currentState;
    first = false;
    return;
  }
  transition = (previousState << 2) | currentState;
  switch (transition)
  {
  // Chiều thuận
  case 0b0001:
  case 0b0111:
  case 0b1110:
  case 0b1000:
    count.fetch_add(1, memory_order_release);
    break;

  // Chiều ngược
  case 0b0010:
  case 0b1011:
  case 0b1101:
  case 0b0100:
    count.fetch_sub(1, memory_order_release);
    break;

  default:
    break;
  }
  previousState = currentState;
}
int32_t Encoder::getCount() const
{
  return count.load(std::memory_order_relaxed);
}
void Encoder::calculateRPM(float dt)
{
  if (dt <= 0.0f || countsPerRevolution <= 0)
    return;

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
  previousCount = 0;
  previousState = 0;
  rpm = 0;
  first = true;
  positionOffset = 0;
  
}
float Encoder::getRPM() const
{
  return rpm;
}
void Encoder::setWheelDiameter(float diameterMM)
{
  if (diameterMM > 0.0f)
    wheelDiameterMM = diameterMM;
}

float Encoder::getRevolutions() const
{
  if (countsPerRevolution == 0)
    return 0.0f;

  int32_t relativeCount =
      getCount() - positionOffset;

  return (float)relativeCount /
         (float)countsPerRevolution;
}

float Encoder::getAngle() const
{
  return getRevolutions() * 360.0f;
}

float Encoder::getAngleRad() const
{
  return getRevolutions() * 2.0f * PI;
}

float Encoder::getDistance() const
{
  return getRevolutions() *
         PI * wheelDiameterMM;
}

float Encoder::getSpeedMMps() const
{
  return getRPM() *
         PI * wheelDiameterMM / 60.0f;
}

void Encoder::resetPosition()
{
  positionOffset = getCount();
}
