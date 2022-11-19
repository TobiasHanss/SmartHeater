#ifndef __MQTT__
#define __MQTT__

#include <WiFi.h>
#include <PubSubClient.h>

#define NO_OF_TOPICS 10

class Cmqtt
{
    struct TxTopics
    {
        String TopicName;
        String LastValue;
        uint32_t LastSend;
    };
    
public:
    Cmqtt();
    

private:
    static void taskHandler (void * ClassPointer);
    static void callback (char* topic, byte* payload, unsigned int length);
    
    void publish(String TopicName, float Value, uint32_t nGap_s = 15, uint32_t nRefresh_s  = (60*2) );
    void publish(String TopicName, int Value, uint32_t nGap_s = 15, uint32_t nRefresh_s  = (60*2) );
    void publish(String TopicName, String Value, uint32_t nGap_s = 15, uint32_t nRefresh_s = (60*2) );
    void update (void);
    void reconnect(void);
   
    TxTopics  m_TopicList[NO_OF_TOPICS];
};

#endif