#include <Arduino.h>
#include "mqtt.h"
#include "settings.h"
#include "logging.h"
#include "eMShome.h"
#include "output.h"


extern CLogging Logging;
extern CSettings Config;
extern COutput Outputs;
extern eMShome SmartMeter;

WiFiClient m_wifiClient;
PubSubClient client(m_wifiClient);

const char* mqttServer = "mqtt.tingg.io";//"mqtt.tingg.io"; //MQTT Broker
const char* mqttUsername = "thing"; //MQTT username
const char* mqttPassword = "2coem6nx3lh3myhqvf7wfjkn4l2ge3g3"; //Thing Key
const char* mqttDeviceId = "9821e7ac-ff82-4414-8cfc-7de47cea3e32"; //Thing ID


Cmqtt::Cmqtt(void)
{

    client.setServer(mqttServer, 1883);
    //client.setCallback(std::bind(&mqtt::callback,std::placeholders::_3,this));
    client.setCallback(callback);


    xTaskCreate(taskHandler,"mqtt",512*4,this,2,NULL );
}

void Cmqtt::taskHandler (void * ClassPointer)
{
    while(1){
        static_cast<Cmqtt*> (ClassPointer)->update();
        delay(10000);
    }
}


void Cmqtt::update (void )
{
    reconnect();
    client.loop();
    client.publish("WaterTemp", String(Logging.getSensor(0)).c_str());
    client.publish("HeaterTemp", String(Logging.getSensor(1)).c_str());
    client.publish("LineTotal", String(SmartMeter.getActivePower_W(0)).c_str());
    client.publish("Line1", String(SmartMeter.getActivePower_W(1)).c_str());
    client.publish("Line2", String(SmartMeter.getActivePower_W(2)).c_str());
    client.publish("Line3", String(SmartMeter.getActivePower_W(3)).c_str());
    client.publish("Output1", String(Outputs.get(0)*100).c_str());
    client.publish("Output2", String(Outputs.get(1)*100).c_str());
    client.publish("Output3", String(Outputs.get(2)*100).c_str());
    client.publish("Output4", String(Outputs.get(3)*100).c_str());
       
}


void Cmqtt::reconnect(void)
{
  while (!client.connected()) 
  {
    Serial.printf("connecting to %s...",mqttServer);
 
    if (client.connect(mqttDeviceId, mqttUsername, mqttPassword)) 
    {
      Serial.println("connected.");
      //client.subscribe(subTopic);
    } else 
    {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}



void Cmqtt::callback(char* topic, byte* payload, unsigned int length)
{
    Serial.print("topic: ");
    Serial.print(topic);
    Serial.print(" message: ");
    for (int i = 0; i < length; i++) 
    {
        Serial.print((char)payload[i]);
    }
    Serial.println();
}