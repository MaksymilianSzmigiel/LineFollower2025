#include "motor.h"
#include "Arduino.h"
#include "QTRSensors.h"


#define SENSOR_COUNT 8  // Liczba czujników

const int PWM_CHANNEL_A = 0;
const int PWM_CHANNEL_B = 1;
const int PWM_FREQ = 5000;
const int PWM_RESOLUTION = 8;
int calcPosition;
int previousPosition;
int previousPosition2;
QTRSensors qtr;
uint16_t sensorValues[SENSOR_COUNT];
const uint8_t sensorPins[SENSOR_COUNT] = {21,19, 18, 12, 14, 16, 4, 15};





void Motor::pinDefinition()
{
  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);
  pinMode(BIN1, OUTPUT);
  pinMode(BIN2, OUTPUT);

  ledcSetup(PWM_CHANNEL_A, PWM_FREQ, PWM_RESOLUTION);
  ledcSetup(PWM_CHANNEL_B, PWM_FREQ, PWM_RESOLUTION);
  
  ledcAttachPin(PWMA, PWM_CHANNEL_A);
  ledcAttachPin(PWMB, PWM_CHANNEL_B);

}
void Motor::enableDriver()
{
    digitalWrite(AIN1, 1);
    digitalWrite(BIN1, 1);

}


void Motor::calibration() {
    delay(2000);
    Serial.println("Kalibracja...");
    for (int i = 0; i < 200; i++) {
        qtr.calibrate();
        delay(10);
    }
}

void Motor::setSensors() {
    Serial.begin(9600);
    delay(250);

    qtr.setTypeRC();  // Używaj trybu Analog zamiast RC
    qtr.setSensorPins(sensorPins, SENSOR_COUNT);

    calibration();
}



int Motor::readSensors()
{
    int position = 0;
    int tab[8] = {0};
    qtr.read(sensorValues, QTRReadMode::On);
    int sum = 0; 
    int count = 0;
    for (int i = 0; i < SENSOR_COUNT; i++) {
        if(sensorValues[i] > sensorThreshold)
        {
            tab[i] = i * 1000;
            sum += tab[i];
            count++;
        }
    }   
    if(count > 0 and count < 6)
    {
        position = sum / count;
    }
    else if( count >= 4 and count <= 6)
    {
        if(position > 0)
        position = 3500;
        else if(position < 0)
        position = -3500;
    }  
    else if(count >=6 and count <8)
        position = 3500;
    else if(count == 0)
    {
        if(previousError > 3000)
        position += 7000;
    }

    return position - 3500 ;     
}

void Motor::updateSensors() {
    qtr.read(sensorValues, QTRReadMode::On);
  
  
      //Serial.print("Wartości czujników: ");
      //for (int i = 0; i < SENSOR_COUNT; i++) {
         // Serial.print(sensorValues[i]);
       //   Serial.print("\t");
     // }
     // Serial.println();
      
  }

int Motor::updatePosition() {
    
    int position=qtr.readLineBlack(sensorValues,QTRReadMode::On);
   
    return position;
  
}



void Motor::regulation()
{
    int error = readSensors();
    int der = error - previousError;
    int correction = (KP * error) + (KD * der);
     
    int leftMotor = BASE_SPEED+ 5 - correction;
    int rightMotor = BASE_SPEED  + correction;

    ledcWrite(PWM_CHANNEL_A, leftMotor);
    ledcWrite(PWM_CHANNEL_B, rightMotor);

    
    previousError = error;
 
}
