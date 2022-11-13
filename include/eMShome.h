#ifndef __EMSHOME__
#define __EMSHOME__

//NOTE!: Changes on the generateHandshake() function in the websockets_client.cpp (@ArduinoWebsockets library)
// Remove User-Agent and Origion parameters
#include <ArduinoWebsockets.h>

using namespace websockets;

//The Server sends data all 250ms. In order to reduce the CPU load skip ever x msg
#define SKIP_EVER_x_MSG  4 

class eMShome
{
public:
  eMShome(String IP,String PW);
  
  typedef struct{
       uint64_t Key;        // Key defined by eMShome is the Measurment ID, the value came in mixed order. So, you need to use the Key (ID)
       uint64_t Value;      // Value, the raw value came mostly in milli... mW for example
       int8_t   PowerOfTen; // Value * 10^x to get the correct unit (kW, W, MW etc.) 
       int8_t   Sign;       //-1 = negaitv value, 1= positiv value
       char const *Name;      // Measurment Name
       char const *Unit;      // Measurment Unit 
  }sDataPoint;

  void begin (void);

  bool connect(void);
  bool isConnected(void) {return m_Conneced;} 

  int32_t getActivePower_W(uint8_t Line);  //Line: 0 => Total; 1 => L1; 3 => L3; 3 => L3
  uint64_t getEpoch (void) {return m_Seconds;}

private:

  static void taskHandler (void * ClassPointer);
  void update(void);
  void decodeMessage(String message);

  uint32_t getUInt32(void); //extraced form eMShome WebIF -> JavaScript 
  uint64_t getUint64(void);  //extraced form eMShome WebIF -> JavaScript
  String getString(void);  //extraced form eMShome WebIF -> JavaScript
  void GDRDecode(void);  //extraced form eMShome WebIF -> JavaScript
  void isTimeSamp(void);  //extraced form eMShome WebIF -> JavaScript
  

  bool getTocken(void); //Requests a accesstocken from the eMShome SmartMeter
  void updateDataPoint(uint64_t Key, uint64_t Value);
  void printMeasurmen (void); 

  String     m_IP;      // SmartMeter IP
  String     m_PW;      // SmartMeter PW

  String     m_Tocken;  // Access Tocken needet to establisch WebSocket connection
  String     m_buf;     // String buffer used to decode the Values
  uint32_t   m_pos;     // Possiont of the buffer used to decode the Values
 
  uint64_t   m_Seconds; // Timestamp send by eMShome
  uint32_t   m_NanoSec; // Timestamp send by eMShome
  String     m_Name;    // Name send by eMShome: always "smart_meter"
  String     m_ID;      // Name send by eMShome: always "smart_meter"
  int32_t    m_Status;  // Name send by eMShome: always 1
  
  uint8_t    m_SkipCounter;

  bool       m_Conneced; // true if connected
  
  WebsocketsClient m_oWSClient; //WebSocked Client object
};

#endif