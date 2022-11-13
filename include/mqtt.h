#ifndef __MQTT__
#define __MQTT__

#include <WiFi.h>
#include <PubSubClient.h>

class Cmqtt
{
public:
    Cmqtt();
    

private:
    static void taskHandler (void * ClassPointer);
    static void callback (char* topic, byte* payload, unsigned int length);
    void update (void);
    void reconnect(void);
   
};

#endif