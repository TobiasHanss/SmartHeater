#ifndef __TEMPSENS__
#define __TEMPSENS__

#include <OneWire.h>
#include <DallasTemperature.h>

#define ONE_WIRE_BUS 17 
#define NO_OF_SENSORS 2 
#define SAMPLE_INTERVAL 5000 //ms

class CLogging
{
public:
    CLogging();
    void begin(void);

    int16_t getSensor(uint8_t No) {return m_nValues[No];}

private:
    void Update(void);
    void readTemp(void);
    static void taskHandler (void * ClassPointer);
    
    int16_t m_nValues[NO_OF_SENSORS];
    OneWire *m_oOneWire;
    DallasTemperature *m_oSensors;

};
#endif
