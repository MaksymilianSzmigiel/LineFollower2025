#ifndef MOTOR_H
#define MOTOR_H

#include <Arduino.h>
#include <QTRSensors.h>

// ====== DEFINICJE PINÓW SILNIKÓW ======
#define AIN1 25
#define AIN2 26
#define BIN1 27
#define BIN2 33

#define PWMA 32
#define PWMB 23

// ====== PARAMETRY REGULATORA ======
#define KP 0.25
#define KD 2.5
#define BASE_SPEED 120

// ====== PARAMETRY CZUJNIKÓW ======
#define sensorThreshold 700

class Motor {
public:
    // inicjalizacja pinów i PWM
    void pinDefinition();
    void enableDriver();

    // czujniki linii
    void setSensors();
    void calibration();
    int readSensors();
    void updateSensors();
    int updatePosition();

    // regulator PID
    void regulation();

private:
    int previousError = 0;
};

#endif