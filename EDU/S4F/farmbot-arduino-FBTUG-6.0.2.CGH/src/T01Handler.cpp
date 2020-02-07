/*
 * T01Handler.cpp
 *
 *  Created on: 19 MAY 2017
 *      Author: Joe Hou
 */

#include "T01Handler.h"

static T01Handler *instance;
//const long interval = 100 * 1000; //  100000 microseconds = 100 ms = 0.1 second

//------------------------------------------------------------------------------------------
T01Handler *T01Handler::getInstance()
{
  if (!instance){
    instance = new T01Handler();
  }
  
  return instance;
}

//------------------------------------------------------------------------------------------
T01Handler::T01Handler()
{
    //
}

//------------------------------------------------------------------------------------------
ISR(TIMER3_COMPB_vect)
{
int val;
int SoilRate;
int SoilSensorHigh;
int SoilSensorLow;
volatile double PercentageIndex = 0;;

    //val = analogRead(5); //connect sensor to Analog 0
    val = analogRead(SOIL_SENSOR_PIN);  //Pin D59 (A5)-2018.06.03
  
    SoilSensorHigh = ParameterList::getInstance()->getValue(SOIL_SENSOR_HIGH);
    SoilSensorLow = ParameterList::getInstance()->getValue(SOIL_SENSOR_LOW);
    
    // Serial.print("Water Soil = "); //print the value to serial
    // Serial.print(val); //print the value to serial
    // Serial.print("\n"); //print the value to serial
    //
    //  Val:518 = 0% , 250 = 100%
    //  1% = 518-250 = 268 * 0.01 = 2.68
    
    PercentageIndex = (SoilSensorHigh - SoilSensorLow) * 0.01;
    SoilRate = 100 - ((val - SoilSensorLow) / PercentageIndex);
    
    //print the value to serial
    Serial.print("Soil Rate = "); Serial.print(SoilRate); Serial.print("\%\n ");
    #ifdef Serial2_Use
      Serial2.print("Soil Rate = "); Serial2.print(SoilRate); Serial2.print("\%\n ");   //-2018.02.25-CGH    
    #endif  
}

//------------------------------------------------------------------------------------------
void EnableTimer3() {
    // enable timer compare interrupt:
    TIMSK3 |= (1 << OCIE3B);
}

//------------------------------------------------------------------------------------------
void DisableTimer3() {
    // disable timer compare interrupt:
    TIMSK3 &= ~(1 << OCIE3B);
}

//------------------------------------------------------------------------------------------
void Timer3Setting() {
    // Initialize Timer
    cli();          // disable global interrupts
    TCCR3A = 0;     // set entire TCCR3A register to 0
    TCCR3B = 0;     // same for TCCR3B
    
    // set compare match register to desired timer count: 800 Hz
    OCR3B = 5208; // 800Hz 5; // 3 Hz
    
    // Set CS10 and CS12 bits for 1024 prescaler:
    TCCR3B |= (1 << CS30) | (1 << CS32);
    
    // enable global interrupts:
    sei();
    
    return 0;
}

//------------------------------------------------------------------------------------------
int T01Handler::execute(Command *command)
{
int Cmd = 0;

    Cmd = command->getV();
    
    //print the value to serial
    Serial.print("T01 = "); Serial.println(Cmd);
    #ifdef Serial2_Use
      Serial2.print("T01 = "); Serial2.println(Cmd);    //-2018.02.25-CGH
    #endif  
    
    Timer3Setting();
    if (Cmd == ENABLE) {
        EnableTimer3();
    }
    if (Cmd == DISABLE) {
        DisableTimer3();
    }
    return 0;
}

//------------------------------------------------------------------------------------------
