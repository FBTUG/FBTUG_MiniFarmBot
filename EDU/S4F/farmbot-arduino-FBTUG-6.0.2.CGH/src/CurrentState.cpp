/*
 * CurrentState.cpp
 *
 *  Created on: 15 maj 2014
 *      Author: MattLech
 */

#include "CurrentState.h"

#include "Config.h"   //-2018.08.15- 沒加, #ifdef Serial2_Use 失效

static CurrentState *instance;

long x = 0;
long y = 0;
long z = 0;

unsigned int speed = 0;
bool endStopState[3][2];
long Q = 0;
int lastError = 0;

unsigned int pMotionStatus[3] = {0, 0, 0};    //-2018.05.05-CGH

//------------------------------------------------------------------------------------------
CurrentState *CurrentState::getInstance()
{
  if (!instance){
    instance = new CurrentState();
  };
  
  return instance;
};

//------------------------------------------------------------------------------------------
CurrentState::CurrentState()
{
    x = 0;
    y = 0;
    z = 0;
    speed = 0;
    Q = 0;
    lastError = 0;
}

//------------------------------------------------------------------------------------------
long CurrentState::getX()
{
    return x;
}

//------------------------------------------------------------------------------------------
long CurrentState::getY()
{
    return y;
}

//------------------------------------------------------------------------------------------
long CurrentState::getZ()
{
    return z;
}

//------------------------------------------------------------------------------------------
long *CurrentState::getPoint()
{
    static long currentPoint[3] = {x, y, z};
    return currentPoint;
}

//------------------------------------------------------------------------------------------
void CurrentState::setX(long newX)
{
    x = newX;
}

//------------------------------------------------------------------------------------------
void CurrentState::setY(long newY)
{
    y = newY;
}

//------------------------------------------------------------------------------------------
void CurrentState::setZ(long newZ)
{
    z = newZ;
}

//------------------------------------------------------------------------------------------
int CurrentState::getLastError()
{
    return lastError;
}

//------------------------------------------------------------------------------------------
void CurrentState::setLastError(int error)
{
    lastError = error;
}

//------------------------------------------------------------------------------------------
void CurrentState::setEndStopState(unsigned int axis, unsigned int position, bool state)
{
    endStopState[axis][position] = state;
}

//------------------------------------------------------------------------------------------
void CurrentState::setMotionStatus(unsigned int Status)  //-2018.05.05
{
    pMotionStatus[0] = Status;
}

//------------------------------------------------------------------------------------------  
void CurrentState::setStepsPerMm(long stepsX, long stepsY, long stepsZ)
{
    stepsPerMmX = stepsX;
    stepsPerMmY = stepsY;
    stepsPerMmZ = stepsZ;
}

//------------------------------------------------------------------------------------------
void CurrentState::storeEndStops()
{
    CurrentState::getInstance()->setEndStopState(0, 0, digitalRead(X_MIN_PIN));
    CurrentState::getInstance()->setEndStopState(0, 1, digitalRead(X_MAX_PIN));
    CurrentState::getInstance()->setEndStopState(1, 0, digitalRead(Y_MIN_PIN));
    CurrentState::getInstance()->setEndStopState(1, 1, digitalRead(Y_MAX_PIN));
    CurrentState::getInstance()->setEndStopState(2, 0, digitalRead(Z_MIN_PIN));
    CurrentState::getInstance()->setEndStopState(2, 1, digitalRead(Z_MAX_PIN));
}

//------------------------------------------------------------------------------------------
void CurrentState::printPosition()
{
    if (stepsPerMmX <= 0) { stepsPerMmX = 1; }
    if (stepsPerMmY <= 0) { stepsPerMmY = 1; }
    if (stepsPerMmZ <= 0) { stepsPerMmZ = 1; }
  
    Serial.print("R82");
    Serial.print(" X"); Serial.print((float)x / (float)stepsPerMmX);
    Serial.print(" Y"); Serial.print((float)y / (float)stepsPerMmY);
    Serial.print(" Z"); Serial.print((float)z / (float)stepsPerMmZ * 1.0);
    
    //-2018.02.25-CGH
    #ifdef Serial2_Use
      Serial2.print("R82");
      Serial2.print(" X"); Serial2.print((float)x / (float)stepsPerMmX);
      Serial2.print(" Y"); Serial2.print((float)y / (float)stepsPerMmY);
      Serial2.print(" Z"); Serial2.print((float)z / (float)stepsPerMmZ * 1.0);
    #endif
    
    printQAndNewLine();
}

//------------------------------------------------------------------------------------------
String CurrentState::getPosition()
{
    if (stepsPerMmX <= 0) { stepsPerMmX = 1; }
    if (stepsPerMmY <= 0) { stepsPerMmY = 1; }
    if (stepsPerMmZ <= 0) { stepsPerMmZ = 1; }
  
    String output = "";
  
    output += "R82";
    output += " X";
    output += (float)x / (float)stepsPerMmX * 1.0;
    output += " Y";
    output += (float)y / (float)stepsPerMmY * 1.0;
    output += " Z";
    output += (float)z / (float)stepsPerMmZ * 1.0;
    //output += getQAndNewLine();
  
    return output;
}

//------------------------------------------------------------------------------------------
void CurrentState::printBool(bool value)
 {
    if(value){
        Serial.print("1");
        #ifdef Serial2_Use
          Serial2.print("1");   //-2018.02.25-CGH
        #endif  
    }
    else{
        Serial.print("0");
        #ifdef Serial2_Use
          Serial2.print("0");   //-2018.02.25-CGH
        #endif   
    }
}

//------------------------------------------------------------------------------------------
void CurrentState::printEndStops()
{
    Serial.print("R81"); 
    #ifdef Serial2_Use 
      Serial2.print("R81");    //-2018.02.25-CGH
    #endif
    
    Serial.print(" XA");   
    #ifdef Serial2_Use
      Serial2.print(" XA");    //-2018.02.25-CGH
    #endif
    printBool(endStopState[0][0]);
    
    Serial.print(" XB"); 
    #ifdef Serial2_Use
      Serial2.print(" XB");    //-2018.02.25-CGH
    #endif  
    printBool(endStopState[0][1]);
    
    Serial.print(" YA"); 
    #ifdef Serial2_Use
      Serial2.print(" YA");    //-2018.02.25-CGH
    #endif   
    printBool(endStopState[1][0]);
    
    Serial.print(" YB"); 
    #ifdef Serial2_Use
      Serial2.print(" YB");    //-2018.02.25-CGH
    #endif  
    printBool(endStopState[1][1]);
    
    Serial.print(" ZA");  
    #ifdef Serial2_Use
      Serial2.print(" ZA");   //-2018.02.25-CGH
    #endif  
    printBool(endStopState[2][0]);
    
    Serial.print(" ZB"); 
    #ifdef Serial2_Use
      Serial2.print(" ZB");    //-2018.02.25-CGH
    #endif  
    printBool(endStopState[2][1]);

    Serial.print(" "); Serial.print(pMotionStatus[0]);
    //Serial.print(" "); Serial.print(pMotionStatus[1]);
    //Serial.print(" "); Serial.print(pMotionStatus[2]);
    #ifdef Serial2_Use
      Serial2.print(" "); Serial2.print(pMotionStatus[0]);
    //  Serial2.print(" "); Serial2.print(pMotionStatus[1]);
    //  Serial2.print(" "); Serial2.print(pMotionStatus[2]);
    #endif  
    
    //Serial.print("\r\n");
    printQAndNewLine();
}

//------------------------------------------------------------------------------------------
void CurrentState::print()
{
    printPosition();
    printEndStops();
}

//------------------------------------------------------------------------------------------
void CurrentState::setQ(long q)
{
    Q = q;
}

//------------------------------------------------------------------------------------------
void CurrentState::resetQ()
{
    Q = 0;
}

//------------------------------------------------------------------------------------------
void CurrentState::printQAndNewLine()
{
    Serial.print(" Q"); Serial.print(Q); Serial.print("\r\n");  
    #ifdef Serial2_Use
      Serial2.print(" Q"); Serial2.print(Q); Serial2.print("\r\n");   //-2018.02.25-CGH
    #endif  
}

//------------------------------------------------------------------------------------------
String CurrentState::getQAndNewLine()
{
String output = "";

    output += " Q";
    output += Q;
    output += "\r\n";
  
    return output;
}

//------------------------------------------------------------------------------------------
void CurrentState::setEmergencyStop()
{
    emergencyStop = true;
}

//------------------------------------------------------------------------------------------
void CurrentState::resetEmergencyStop()
{
    emergencyStop = false;
}

//------------------------------------------------------------------------------------------
bool CurrentState::isEmergencyStop()
{
    return emergencyStop;
}

//------------------------------------------------------------------------------------------
