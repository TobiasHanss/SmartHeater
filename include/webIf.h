#ifndef __WEBIF__
#define __WEBIF__

#include <SPIFFS.h> 
#include <WebServer.h> 

class WebIf
{
public:
    WebIf(int nPort);

    void begin(bool setupMode = false);

private:
    static void taskHandler (void * ClassPointer);

    void onRequestData(void);
    void onSettingsPost(void);
    void onOutputPost(void);
    void onConfigPost(void);
    void onSecure(void);
    void onSetup(void);
    void onSetupPost(void);
    void listFiles(void);    
    WebServer *m_oServer;

    bool m_Out1;
    bool m_Out2;
    bool m_Out3;
};
#endif
