#include <Arduino.h>
#include <SPIFFS.h> 
#include <WiFi.h>
#include <ArduinoJson.h>
#include "eMShome.h"
#include "settings.h"
#include "webIf.h"
#include "output.h"
#include "logging.h"
#include "task.h"

extern eMShome SmartMeter;
extern CSettings Settings;
extern CSettings Config;
extern CSettings Secure;
extern COutput Outputs;
extern CLogging Logging;

WebIf::WebIf(int nPort)
{
    m_oServer = new WebServer(nPort);
}

void WebIf::taskHandler (void * ClassPointer)
{
    while(1){
        static_cast<WebIf*> (ClassPointer)->m_oServer->handleClient();
        delay(50);
    }
}

void WebIf::listFiles()
{
    File root = SPIFFS.open("/");
    File file = root.openNextFile();
    Serial.println("FILE   \t\tSize");
    while(file){
        Serial.printf("%s \t%d Byte\n",file.name(),file.size());
        file = root.openNextFile();
    }
}

void WebIf::begin(bool setupMode)
{

    if (!SPIFFS.begin(true)) {
        Serial.println("SPIFFS init failed");
    }

    //listFiles();

    if (setupMode)
    {
        m_oServer->on("/", std::bind(&WebIf::onSetup,this));
        m_oServer->on("/setup.post", HTTP_POST ,std::bind(&WebIf::onSetupPost,this));
    }   
    else
    {    
        m_oServer->on("/settings.post", HTTP_POST ,std::bind(&WebIf::onSettingsPost,this));
        m_oServer->on("/output.post", HTTP_POST ,std::bind(&WebIf::onOutputPost,this));
        m_oServer->on("/config.post", HTTP_POST ,std::bind(&WebIf::onConfigPost,this));
    }
    m_oServer->on("/data.json", std::bind(&WebIf::onRequestData,this));
    m_oServer->on("/secure.json", std::bind(&WebIf::onSecure,this));
    m_oServer->serveStatic("/", SPIFFS, "/");

    m_oServer->begin();
    xTaskCreate(taskHandler,"CWebIf",512*6,this,3,NULL );
}

void WebIf::onSetup(void)
{
    const char * redirect =  R"(<!DOCTYPE html><html><head><meta http-equiv="refresh" content="0; url=setup.htm"></head><body><p>Please follow <a href="setup.htm">this link</a>.</p></body></html>)";
    m_oServer->send(200, "text/html", redirect); 
}

void WebIf::onSetupPost(void)
{
    Serial.println("SetupPost");

    for (int i = 0 ; i < m_oServer->args();i++ )
    {
        String Value = m_oServer->arg(i);
        String Key = m_oServer->argName(i);       
        if (Key == "PSK")
        {
            Secure.set(Key,Value);
        }
        else
        {
            Config.set(Key,Value);
        }

        Serial.printf("Key: %s Value: %s\n",Key,Value);
    }
    Config.save();
    Secure.save();
    m_oServer->send(200, "text/plain", ""); //Send web page
    ESP.restart();
}

void WebIf::onSecure(void)
{
    m_oServer->send(403, "text/plain", "Access denied"); //Send web page
}

void WebIf::onSettingsPost(void)
{

    Settings.set("R1_EN","off");
    Settings.set("R2_EN","off");
    Settings.set("R3_EN","off");
    Settings.set("R4_EN","off");

    for (int i = 0 ; i < m_oServer->args();i++ )
    {
        String Value = m_oServer->arg(i);
        if (Value == "off")
            Settings.set(m_oServer->argName(i),"on"); 
        else
            Settings.set(m_oServer->argName(i),Value);       

        Serial.printf("%s -> %s\n",m_oServer->argName(i),m_oServer->arg(i));
    }
    Settings.save();
    m_oServer->send(200, "text/plain", ""); //Send web page

}

void WebIf::onOutputPost(void)
{
    bool Out[NO_OF_OUTPUT] = {false,false,false,false};

    for (int i = 0 ; i < m_oServer->args();i++ )
    {
        if (m_oServer->argName(i) == "OUT1") {Out[0] = true;}
        if (m_oServer->argName(i) == "OUT2") {Out[1] = true;}
        if (m_oServer->argName(i) == "OUT3") {Out[2] = true;}
        if (m_oServer->argName(i) == "OUT4") {Out[3] = true;}
    }
    
    for (int i = 0 ; i < NO_OF_OUTPUT ; i++)
    {
        Outputs.set(i,Out[i]);
    }
    
    m_oServer->send(200, "text/plain", ""); //Send web page
}

void WebIf::onConfigPost(void)
{
    for (int i = 0 ; i < m_oServer->args();i++ )
    {
        String Value = m_oServer->arg(i);
        if (Value == "off")
            Config.set(m_oServer->argName(i),"on"); 
        else
            Config.set(m_oServer->argName(i),Value);       

        Serial.printf("%s -> %s\n",m_oServer->argName(i),m_oServer->arg(i));
    }
    Config.save();
    m_oServer->send(200, "text/plain", ""); //Send web page
    ESP.restart();
}

void WebIf::onRequestData(void)
{
    StaticJsonDocument<300> oJSON;    
    static int i = 0 ;

    oJSON["Epoch"] = String(SmartMeter.getEpoch());
    oJSON["L1_W"]  = String(SmartMeter.getActivePower_W(1));    
    oJSON["L2_W"]  = String(SmartMeter.getActivePower_W(2)); 
    oJSON["L3_W"]  = String(SmartMeter.getActivePower_W(3)); 
    oJSON["ALL_W"] = String(SmartMeter.getActivePower_W(0)); 
    oJSON["LOCAL_IP"] = WiFi.localIP();
    oJSON["MAC"]  = WiFi.macAddress();  
    oJSON["OUT1"] = Outputs.getOnOff(0);
    oJSON["OUT2"] = Outputs.getOnOff(1);
    oJSON["OUT3"] = Outputs.getOnOff(2);
    oJSON["OUT4"] = Outputs.getOnOff(3);  
    oJSON["TEMP1"] = Logging.getSensor(0);     
    oJSON["TEMP2"] = Logging.getSensor(1);       
    String buf;
    serializeJson(oJSON,buf);
    m_oServer->send(200, "application/json", buf);
}


