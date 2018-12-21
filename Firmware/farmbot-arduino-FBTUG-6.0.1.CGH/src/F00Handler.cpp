/*
 * F00Handler.cpp
 *
 *  Created on: 2018/02/26
 *      Author: Chen GeHom
 */

#include "F00Handler.h"

static F00Handler *instance;

//------------------------------------------------------------------------------------------
F00Handler *F00Handler::getInstance()
{
  if (!instance){
      instance = new F00Handler();
  }
  
  return instance;
}

//------------------------------------------------------------------------------------------
F00Handler::F00Handler()
{
    //
}

//------------------------------------------------------------------------------------------
int F00Handler::execute(Command *command)
{    
int vFuncCode = 0;

    //ParameterList::getInstance()->readValue(command->getP());
    //ParameterList::getInstance()->writeValue(command->getP(), command->getV());
    
    vFuncCode = command->getP();
    //Serial.print("FuncCode = "); Serial.println(vFuncCode);
    
    //----------------------------------------------------------------
    if(vFuncCode == 1){   //-F00 P1 Reset EEPROM to Default-2018.02.26
        Serial.print("R00 Reset AllValuesToDefault, writeAllValuesToEeprom\r\n");    
        #ifdef BT_Serial2_Use
          Serial2.print("R00 Reset AllValuesToDefault, writeAllValuesToEeprom\r\n");    //-2018.02.25-CGH
        #endif  
              
        //writeValueEeprom(0, 0);   //-2018.02.26-Set to Default   
        ParameterList::getInstance()->setAllValuesToDefault();
        ParameterList::getInstance()->writeAllValuesToEeprom();
    }
    
    //----------------------------------------------------------------
    if(vFuncCode == 2){   //-F00 P2 X1 Y1 Z1 ServoOn/Off 激磁-2018.03.02
        //StepperControl::ServoON(int vAxis, int vOnOff)   //-2018.03.02-ServoOn/Off
        //StepperControl::getInstance()->moveToCoords(command->getX(), command->getY(), command->getZ(), command->getA(), command->getB(), command->getC(), false, false, false);
        
        if(command->getX() == 1) StepperControl::getInstance()->ServoON(0, 1);    //-X Axis Servo ON
        if(command->getX() == -1) StepperControl::getInstance()->ServoON(0, 0);   //-X Axis Servo OFF
        
        if(command->getY() == 1) StepperControl::getInstance()->ServoON(1, 1);    //-Y Axis Servo ON
        if(command->getY() == -1) StepperControl::getInstance()->ServoON(1, 0);   //-Y Axis Servo OFF        

        if(command->getZ() == 1) StepperControl::getInstance()->ServoON(2, 1);    //-Z Axis Servo ON
        if(command->getZ() == -1) StepperControl::getInstance()->ServoON(2, 0);   //-Z Axis Servo OFF        
    }
    
    //----------------------------------------------------------------
    if(vFuncCode == 3){   //-F00 P3 X1 Y1 Z1 Stop Axis motion-2018.03.03
        if(command->getX() > 0) StepperControl::getInstance()->Stop_Axis(0);   //-Stop X Axis
        if(command->getY() > 0) StepperControl::getInstance()->Stop_Axis(1);   //-Stop Y Axis
        if(command->getZ() > 0) StepperControl::getInstance()->Stop_Axis(2);   //-Stop Z Axis
    }

    //----------------------------------------------------------------
    if(vFuncCode == 4){   //-F00 P4 Read SOIL_SENSOR-2018.06.03
        int val;
        int SoilRate;
        int SoilSensorHigh;
        int SoilSensorLow;
        volatile double PercentageIndex = 0;;

        val = analogRead(SOIL_SENSOR_PIN);  //Pin D59 (A5)-2018.06.03      
        
        //  Val:518 = 0% , 250 = 100%
        //  1% = 518-250 = 268 * 0.01 = 2.68
        
        SoilSensorHigh = ParameterList::getInstance()->getValue(SOIL_SENSOR_HIGH);
        SoilSensorLow = ParameterList::getInstance()->getValue(SOIL_SENSOR_LOW);
    
        PercentageIndex = (SoilSensorHigh - SoilSensorLow) * 0.01;
        SoilRate = 100 - ((val - SoilSensorLow) / PercentageIndex);
    
        Serial.print("Soil Rate = "); Serial.print(SoilRate); Serial.print("\%\n ");
        #ifdef BT_Serial2_Use
          Serial2.print("Soil Rate = "); Serial2.print(SoilRate); Serial2.print("\%\n ");   //-2018.02.25-CGH  
        #endif            
    }
                  
    return 0;
}

//------------------------------------------------------------------------------------------
