#include "ServoController.h"

ServoController* ServoController::_instance = nullptr;

ServoController::ServoController(){
    priPwm.begin();
    priPwm.setPWMFreq(PWM_FREQ);
    secPwm.begin();
    secPwm.setPWMFreq(PWM_FREQ);
    delay(10);
}

ServoController* ServoController::getInstance(){
    if(_instance == nullptr)
        _instance = new ServoController();
    return _instance;
}

void ServoController::writeLeg(int leg, vec3 anglesDeg){

    setAngle(leg * 3 + 0, anglesDeg.x);
    setAngle(leg * 3 + 1, anglesDeg.y);
    setAngle(leg * 3 + 2, anglesDeg.z);

}

void ServoController::setAngle(uint8_t channel, float deg){
    deg = constrain(deg, 0, 180);
    uint16_t pulse = (uint16_t)(SERVO_MIN + (deg / 180.0f) * (SERVO_MAX - SERVO_MIN));

    if(channel < MAX_CHANNEL_PER_BOARD){
        priPwm.setPWM(channel, 0, pulse);
    } else {
        secPwm.setPWM(channel - MAX_CHANNEL_PER_BOARD, 0, pulse);
    }

}
