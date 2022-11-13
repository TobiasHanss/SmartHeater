#include <Arduino.h>
#include "settings.h"

CSettings::CSettings(const char *sFileName, size_t maxSize )
{
    if (!SPIFFS.begin(true)) {
        Serial.println("SPIFFS init failed");
    }
    m_pJSON = new DynamicJsonDocument(maxSize);
    m_sFileName = sFileName;
    load();
}

void CSettings::load(void)
{
    //Serial.printf("%s():\n",__func__);
    File file = SPIFFS.open(m_sFileName);
    DeserializationError error = deserializeJson(*m_pJSON, file);
    if (error) 
    {
        Serial.printf("%s(): deserializeJson() failed: %s\n",__func__,error.f_str());
        //createDefault();
        //save();
    }

    //serializeJsonPretty(*m_pJSON, Serial);
    file.close();
}

void CSettings::save(void)
{
    Serial.printf("%s():\n",__func__);
    File file = SPIFFS.open(m_sFileName,"w",true);
    if (file)
    {
        serializeJson(*m_pJSON,file);
        file.close();
    }
}

