
#ifndef VEC_H
#include "../math/Vec3.h"
using vec3 = Vec3;
#endif

#include <Adafruit_PWMServoDriver.h>
#ifndef ARDUINO
#include <Arduino.h>
#endif



#define MAX_CHANNEL_PER_BOARD 16
#define PWM_FREQ 50

// servo dependent constants
#define SERVO_MIN 150
#define SERVO_MAX 500


class ServoController{

    public:

        static ServoController* getInstance();
        void writeLeg(int leg, vec3 anglesDeg);

    private:


        ServoController();

        void setAngle(uint8_t channel, float deg);

        static ServoController* _instance;
        Adafruit_PWMServoDriver priPwm = Adafruit_PWMServoDriver(0x41);
        Adafruit_PWMServoDriver secPwm = Adafruit_PWMServoDriver(0x40);

};
