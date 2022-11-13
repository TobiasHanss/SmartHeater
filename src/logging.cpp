#include <Arduino.h>
#include "logging.h"
#include "task.h"

CLogging::CLogging()
{
    m_oOneWire = new OneWire(ONE_WIRE_BUS);
    m_oSensors = new DallasTemperature(m_oOneWire);
}

void CLogging::begin (void)
{
    xTaskCreate(taskHandler,"CLogging",512*4,this,1,NULL );
}

void CLogging::taskHandler (void * ClassPointer)
{
    static_cast<CLogging*> (ClassPointer)->Update(); 
}

void CLogging::Update(void)
{
    while (1){

        readTemp();
        delay(SAMPLE_INTERVAL);
    }
}

void CLogging::readTemp(void)
{
    
    // call sensors.requestTemperatures() to issue a global temperature request to all devices on the bus
    m_oSensors->requestTemperatures(); // Send the command to get temperatures
    
    for (int i = 0 ; i < NO_OF_SENSORS ; i++)
    {
        m_nValues[i] = m_oSensors->getTempCByIndex(i);
    }

}