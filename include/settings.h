#ifndef __SETTINGS__
#define __SETTINGS__

#include <ArduinoJson.h>
#include <SPIFFS.h>

class CSettings
{
public:
    CSettings(const char * sFileName,size_t maxSize );

    void load(void);
    void save(void);

    const char* get(const char* key){return (*m_pJSON)[key];}
    String getStr(const char* key){return (*m_pJSON)[key];}
    int32_t getInt(const char* key){return getStr(key).toInt();}   

    void set(const char* key, String Value){(*m_pJSON)[key] = Value;}
    void set(String key, String Value){(*m_pJSON)[key] = Value;}
    void print(void){serializeJsonPretty(*m_pJSON, Serial);}

private:

    void createDefault(void);
    DynamicJsonDocument  *m_pJSON;
    String m_sFileName;


};

#endif