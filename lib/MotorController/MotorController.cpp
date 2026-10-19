#include <MotorController.h>
MotorController::MotorController(
    Motor &motor,
    Encoder &encoder,
    PIDController &pid,
    RampGenerator &ramp) : motor(motor), encoder(encoder), pid(pid), ramp(ramp), state(MotorState::Idle), targetRPM(0.0f)
{
}

void MotorController::start(float rpm)
{
  if (state == MotorState::EmergencyStop)
  {
    return;
  }
  targetRPM = rpm;
  state = MotorState::Running;
}

void MotorController::stop()
{
  if (state == MotorState::Running)
  {
    targetRPM = 0;
    state = MotorState::Stopping;
  }
}
void MotorController::emergencyStop()
{
  state = MotorState::EmergencyStop;
  targetRPM = 0;
  motor.stop();
  pid.reset();
  ramp.reset();
}
bool MotorController::checkFault(float pwm,float actualRPM,unsigned long now)
{
  bool faultCondition =fabs(pwm) > 200 && fabs(actualRPM) < 5;//nếu sai điều kiện này chứng tỏ không có lỗi

  if(!faultCondition)//thì nhảy vào đây để cập nhật thời gian cho faultiming lúc chưa lỗi
  {
    faultTiming = false;
    return false;
  }
  if(!faultTiming)
  {
    faultStartTime = now;//cập nhật
    faultTiming = true;
    return false;
  }
  if(now - faultStartTime >= 1000)//nếu quá 1s không được cập nhật là không lỗi thì sẽ là lỗi
  {
      return true;
  }
}
void MotorController::enterFault(MotorFault fault)
{
  currentFault = fault;
  state = MotorState::Fault;
  motor.setPWM(0);
  pid.reset();
  ramp.reset(0);
}
MotorFault MotorController::getFault() const
{
    return currentFault;
}
bool MotorController::isSafeToReset() const
{
    if (currentFault == MotorFault::OverTemperature)
    {
        // code chờ tương lai
    }

    return true;
}
bool MotorController::resetFault()
{
  if (state != MotorState::Fault)
    return false;
  if (!isSafeToReset())
    return false;
  currentFault = MotorFault::None;
  targetRPM = 0;

    pid.reset();
    ramp.reset(0);

    motor.setPWM(0);

    state = MotorState::Idle;

    return true;
  
  
}
void MotorController::update(float dt)
{


  
  switch (state)
  {
  case MotorState::Idle:
  {
    motor.setPWM(0);
    break;
  }

  case MotorState::Running:
  {
    float smoothTarget = ramp.update(targetRPM, dt);

    float actualRPM = encoder.getRPM(dt);

    float pwm = pid.calculate(smoothTarget, actualRPM, dt);
    if(checkFault(pwm,actualRPM,millis()))
    {
      MotorFault faultName=MotorFault::EncoderFailure;
      enterFault(faultName);
      break;
    }

    motor.setPWM(pwm);
    break;
  }

  case MotorState::Stopping:
  {
    float smoothTarget = ramp.update(0, dt);

    float actualRPM = encoder.getRPM(dt);

    float pwm = pid.calculate(
        smoothTarget,
        actualRPM,
        dt);

    motor.setPWM(pwm);
    if (smoothTarget == 0 && fabs(actualRPM) < 5.0f)
    {
      motor.setPWM(0);
      pid.reset();
      state = MotorState::Idle;
    }

    break;
  }

  case MotorState::EmergencyStop:
  {
    motor.setPWM(0);
    break;
  }
  case MotorState::Fault:
  {

    break;
  }
  }
}

String MotorController:: getStatus()const{
  String value;
  switch (state)
  {
  case MotorState::Idle:{
    value="Idle";
    return value;
    break;
  }
  case MotorState::Running:{
    value="Running";
    return value;
    break;
  }
  case MotorState::Stopping:{
    value="Stopping";
    return value;
    break;
  }
  case MotorState::EmergencyStop:{
    value="EmergencyStop";
    return value;
    break;
  }
  case MotorState::Fault:{

    switch (getFault())
    {
      
      case MotorFault::MotorStall:
      {
        value="MotorStall";
     
        break;
      }
      case MotorFault::DriverFailure:
      {
        value="DriverFailure";
       
        break;
      }
      case MotorFault::EncoderFailure:
      {
        value="EncoderFailure";
        
        break;
      }
      case MotorFault::OverTemperature:
      {
        value="EncoderOverTemperatureFailure";
        
        break;
      }
      default:
        value="No detect";
        
        break;
    }
    return value;
    break;
  }
    
  }
}