#include <Arduino.h>
#include "output.h"
#include "eMShome.h"
#include "settings.h"
#include "logging.h"
#include "task.h"

extern eMShome SmartMeter;
extern CSettings Settings;
extern CLogging Logging;

COutput::COutput(void)
{
    pinMode(RELAY1, OUTPUT);
    digitalWrite(RELAY1,LOW);

    pinMode(RELAY2, OUTPUT);
    digitalWrite(RELAY2,LOW);

    pinMode(RELAY3, OUTPUT);
    digitalWrite(RELAY3,LOW);

    pinMode(RELAY4, OUTPUT);
    digitalWrite(RELAY4,LOW);

    for (uint8_t i = 0; i < NO_OF_OUTPUT; i++)
    { 
        m_bOutput[i] = false;
        m_bOutput_Old[i] = false;
        m_iDelayCounter[i] = 0;
    }  
    
    m_iNextUpdateTime = millis() +1000;
}

void COutput::begin (void)
{
    xTaskCreate(taskHandler,"COutput",512*4,this,1,NULL );
}

void COutput::taskHandler (void * ClassPointer)
{
    while(1){
        static_cast<COutput*> (ClassPointer)->update();
        delay(450);
    }
}

void COutput::update(void)
{
    unsigned long Time = millis();
    if (m_iNextUpdateTime < Time)
    {  
        m_iNextUpdateTime = Time + 1000;
        checkRules();
    }
}


void COutput::checkRules(void)
{
    int32_t iOnAt;
    int32_t iOffAt;
    int32_t iPower;

    bool bEnabled;
    char nRx_EN[6] = {"R1_EN"};
    char nRx_ON[6] = {"R1_ON"};
    char nRx_OFF[7] = {"R1_OFF"};   


    int32_t iTotalPower = SmartMeter.getActivePower_W(0); //Total Power is on index 0
    int32_t iTotalPowerOn = Settings.getInt("nR4_ON");
    int32_t iTotalPowerOff = Settings.getInt("nR4_OFF"); 

    int16_t currentTemp = Logging.getSensor(0);
    int16_t tempOnAt = Settings.getInt("ON_TEMP");
    int16_t tempOffAt = Settings.getInt("OFF_TEMP");


    for (uint8_t i = 0; i < NO_OF_OUTPUT-1; i++)
    {
        bEnabled = Settings.getStr(nRx_EN) == "on" ? true : false;
        nRx_EN[1] += 1; //Increment Rx_EN
        iOnAt =  Settings.getInt(nRx_ON);
        nRx_ON[1] += 1; //Increment Rx_ON
        iOffAt = Settings.getInt(nRx_OFF);
        nRx_OFF[1] += 1; //Increment Rx_OFF
        iPower = SmartMeter.getActivePower_W(i+1);

        if (bEnabled)
        {
            //Serial.printf("OutNo:%d State:%d OnAt:%d OffAt:%d Power:%d \n",i,m_bOutput[i],iOnAt,iOffAt,iPower);
            if (m_bOutput[i])
            {
                if ((iPower >= iOffAt) || (iTotalPower >= iTotalPowerOff) || ( currentTemp >= tempOffAt ))
                {
                    m_bOutput[i] = false;
                }
            }
            else
            {
                if ((iPower <= iOnAt) && (iTotalPower <= iTotalPowerOn) && ( currentTemp <= tempOnAt ))
                {
                    m_bOutput[i] = true;
                    iTotalPower -= iOnAt;
                }
            }
        

            if (m_bOutput[i] != m_bOutput_Old[i])
            {
                m_iDelayCounter[i] ++;
                if (m_bOutput[i])
                {
                    int32_t OffDelay  = Settings.getInt("ON_DELAY");
                    if (m_iDelayCounter[i] >= OffDelay) 
                    {
                        m_bOutput_Old[i]= true;
                        setHW(3,true);
                        delay(100);
                        setHW(i,true);
                    }
                }
                else
                {
                    int32_t OnDelay  = Settings.getInt("OFF_DELAY");
                    if (m_iDelayCounter[i] >= OnDelay) 
                    {
                        m_bOutput_Old[i]= false;
                        setHW(i,false);
                        setHW(3,(m_bOutput[0] || m_bOutput[1] || m_bOutput[2]));  
                    }
                }
            }
            else
            {
                m_iDelayCounter[i] = 0;
            }
        }
    }
}

void COutput::setHW(uint8_t No, bool Value)
{
    switch (No)
    {
        case(0): digitalWrite(RELAY1,Value); break;
        case(1): digitalWrite(RELAY2,Value); break;
        case(2): digitalWrite(RELAY3,Value); break;
        case(3): digitalWrite(RELAY4,Value); break;
        default: Serial.printf("%s(): Bug: Unknown output! (%d)\n",__func__, No);
    }
    //Serial.printf("%s():No:%d to %d\n",__func__,No,Value);

}

bool COutput::getHW(uint8_t No)
{   
    bool Value;
    switch (No)
    {
        case(0): Value = digitalRead(RELAY1); break;
        case(1): Value = digitalRead(RELAY2); break;
        case(2): Value = digitalRead(RELAY3); break;
        case(3): Value = digitalRead(RELAY4); break;
        default: Serial.printf("%s(): Bug: Unknown output! (%d)\n",__func__, No);
    }
    return Value;
}