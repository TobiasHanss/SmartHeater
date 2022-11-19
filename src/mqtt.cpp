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
  for (int i = 0 ; i < NO_OF_TOPICS ; i++)
  {
    m_TopicList[i].TopicName = "";
  }
  client.setServer(mqttServer, 1883);
  //client.setCallback(std::bind(&mqtt::callback,std::placeholders::_3,this));
  client.setCallback(callback);


  xTaskCreate(taskHandler,"mqtt",512*8,this,2,NULL );
}

void Cmqtt::taskHandler (void * ClassPointer)
{
    while(1){
        static_cast<Cmqtt*> (ClassPointer)->update();
        delay(10000);
    }
}



void Cmqtt::publish(String TopicName, String Value, uint32_t nGap_s, uint32_t nRefresh_s )
{
  uint32_t Now = millis()/1000;
  for (int i = 0 ; i < NO_OF_TOPICS ; i++)
  {
    if (  (m_TopicList[i].TopicName == TopicName) && 
          (Now > (m_TopicList[i].LastSend + nGap_s)) && 
          ( (m_TopicList[i].LastValue != Value) || (Now > (m_TopicList[i].LastSend + nRefresh_s)) ) )
    {
      Serial.printf("Found: %s Value: %s\n", TopicName, Value);
      m_TopicList[i].LastValue = Value;
      m_TopicList[i].LastSend = Now;
      client.publish(TopicName.c_str(), Value.c_str()); 
      return;
    }
    if (m_TopicList[i].TopicName  == "")
    {
      Serial.printf("New: %s Value: %s\n", TopicName, Value);
      m_TopicList[i].TopicName = TopicName;
      m_TopicList[i].LastValue = Value;
      m_TopicList[i].LastSend = Now;
      client.publish(TopicName.c_str(), Value.c_str()); 
      return;
    }
  }
}

void Cmqtt::publish(String TopicName, int Value, uint32_t nGap_s, uint32_t nRefresh_s )
{
  publish(TopicName, String(Value), nGap_s, nRefresh_s );
}

void Cmqtt::publish(String TopicName, float Value, uint32_t nGap_s, uint32_t nRefresh_s )
{
  publish(TopicName, String(Value), nGap_s, nRefresh_s );
}

void Cmqtt::update (void )
{
    reconnect();
    client.loop();
   
    publish("WaterTemp", Logging.getSensor(0));
    publish("HeaterTemp", Logging.getSensor(1));
    publish("LineTotal", SmartMeter.getActivePower_W(0));
    publish("Line1", SmartMeter.getActivePower_W(1));
    publish("Line2", SmartMeter.getActivePower_W(2));
    publish("Line3", SmartMeter.getActivePower_W(3));
    publish("Output1", Outputs.get(0)*100);
    publish("Output2", Outputs.get(1)*100);
    publish("Output3", Outputs.get(2)*100);
    publish("Output4", Outputs.get(3)*100);

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