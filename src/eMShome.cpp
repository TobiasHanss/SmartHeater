#include <Arduino.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "eMShome.h"
#include "task.h"

eMShome::sDataPoint dataPoint[]={

// NOTE: ORDER MATTERS !!!
    {1099528667391, 0, 3,  1, "All Wirkleistung", "W" }, //positiv. The server can't send neagiv values. Therefore it sends two values, one positive, the other must be interpreted negatively. The invalid value is always 0.  
    {1099545444607, 0, 3, -1, "All Wirkleistung", "W" }, //negativ. The server can't send neagiv values. Therefore it sends two values, one positive, the other must be interpreted negatively. The invalid value is always 0.   
    
    {1099864211711, 0, 3,  1, "L1 Wirkleistung", "W" },
    {1099880988927, 0, 3, -1, "L1 Wirkleistung", "W" },
    
    {1100199756031, 0, 3,  1, "L2 Wirkleistung", "W" },
    {1100216533247, 0, 3, -1, "L2 Wirkleistung", "W" },
    
    {1100535300351, 0, 3,  1, "L3 Wirkleistung", "W" },
    {1100552077567, 0, 3, -1, "L3 Wirkleistung", "W" }

};

#if 0
    //------ L1 --------
    {1099914543359, 0, 3,  1, "L1 Blindleistung", "var" },
    {1099998429439, 0, 3,  1, "L1 Scheinleistung", "VA" },
    {1100015206655, 0, 3, -1, "L1 Scheinleistung", "VA" },
    {1100031983871, 0, 3,  1, "L1 Current", "A" },
    {1100048761087, 0, 3,  1, "L1 Voltage", "V" },
    {1100065538303, 0, 1,  1, "L1 Power Factor", "" },
    {1099864473855, 0, 6,  1, "L1 Wirkenergie", "kWh" },
    {1099881251071, 0, 6, -1, "L1 Wirkenergie", "kWh" },
    {1099898028287, 0, 6,  1, "L1 Blindenergie", "kvarh" },
    {1099914805503, 0, 6, -1, "L1 Blindenergie", "kvarh" },
    {1099998691583, 0, 6,  1, "L1 Scheinenergie", "kVAh" },
    {1100015468799, 0, 6, -1, "L1 Scheinenergie", "kVAh" },

    //------ L2 --------
    {1100250087679, 0, 3,  1, "L2 Blindleistung", "var" },
    {1100333973759, 0, 3,  1, "L2 Scheinleistung", "VA" },
    {1100350750975, 0, 3, -1, "L2 Scheinleistung", "VA" },
    {1100367528191, 0, 3,  1, "L2 Current", "A" },
    {1100384305407, 0, 3,  1, "L2 Voltage", "V" },
    {1100401082623, 0, 1,  1, "L2 Power Factor", "" },
    {1100200018175, 0, 6,  1, "L2 Wirkenergie", "kWh" },
    {1100216795391, 0, 6, -1, "L2 Wirkenergie", "kWh" },
    {1100233572607, 0, 6,  1, "L2 Blindenergie", "kvarh" },
    {1100250349823, 0, 6, -1, "L2 Blindenergie", "kvarh" },
    {1100334235903, 0, 6,  1, "L2 Scheinenergie", "kVAh" },
    {1100351013119, 0, 6, -1, "L2 Scheinenergie", "kVAh" },

    //------ L3 ---------
    {1100585631999, 0, 3,  1, "L3 Blindleistung", "var" },
    {1100669518079, 0, 3,  1, "L3 Scheinleistung", "VA" },
    {1100686295295, 0, 3, -1, "L3 Scheinleistung", "VA" },
    {1100703072511, 0, 3,  1, "L3 Current", "A" },
    {1100719849727, 0, 3,  1, "L3 Voltage", "V" },
    {1100736626943, 0, 1,  1, "L3 Power Factor", "" },
    {1100535562495, 0, 6,  1, "L3 Wirkenergie", "kWh" },
    {1100552339711, 0, 6, -1, "L3 Wirkenergie", "kWh" },
    {1100569116927, 0, 6,  1, "L3 Blindenergie", "kvarh" },
    {1100585894143, 0, 6, -1, "L3 Blindenergie", "kvarh" },
    {1100669780223, 0, 6,  1, "L3 Scheinenergie", "kVAh" },
    {1100686557439, 0, 6, -1, "L3 Scheinenergie", "kVAh" },
   
    //------ Total -------
    {1099578999039, 0, 3,  1, "T Blindleistung", "var" },
    {1099662885119, 0, 3,  1, "T Scheinleistung", "VA" },
    {1099679662335, 0, 3, -1, "T Scheinleistung", "VA" },
    {1099528929535, 0, 6,  1, "T Wirkenergie", "kWh" },
    {1099545706751, 0, 6, -1, "T Wirkenergie", "kWh" },
    {1099562483967, 0, 6,  1, "T Blindenergie", "kvarh" },
    {1099579261183, 0, 6, -1, "T Blindenergie", "kvarh" },
    {1099663147263, 0, 6,  1, "T Scheinenergie", "kVAh" },
    {1099679924479, 0, 6,  1, "T Scheinenergie", "kVAh" },
    {1099729993983, 0, 1,  1, "T Power Factor", "" },
    {1099746771199, 0, 1,  1, "T Freq", "Hz" },

#endif

  


//************************************************************
//************************************************************
eMShome::eMShome(String IP, String PW)
{
  m_IP = IP;
  m_PW = PW;
  
  m_Conneced = false;

  // install Callback
  m_oWSClient.onMessage([&](WebsocketsMessage message)
  { 
    if (m_SkipCounter ++ >= SKIP_EVER_x_MSG)
    {
      decodeMessage(message.data());
      //printMeasurmen();
      m_SkipCounter = 0;
    }
  }
  );

  // install callback when events are occuring
  m_oWSClient.onEvent([&] (WebsocketsEvent event, String data)
    {
      if(event == WebsocketsEvent::ConnectionOpened) 
      {
        Serial.println("WebSocket Connnection Opened");
      } 
      else if(event == WebsocketsEvent::ConnectionClosed) 
      {
        Serial.println("WebSocket Connnection Closed");
        m_Conneced = false;
      } 
      else if(event == WebsocketsEvent::GotPing) 
      {
        Serial.println("WebSocket Got a Ping!");
      } 
      else if(event == WebsocketsEvent::GotPong) 
      {
        Serial.println("WebSocket Got a Pong!");
      }
    }
    );
}

//************************************************************
//************************************************************
void eMShome::begin (void)
{
    xTaskCreate(taskHandler,"eMShome",512*6,this,1,NULL );
}

//************************************************************
//************************************************************
void eMShome::taskHandler (void * ClassPointer)
{
    while(1){
        //printStack();
        static_cast<eMShome*> (ClassPointer)->update();
        delay(250);
    }
}

//************************************************************
//************************************************************
void eMShome::update(void)
{

  if (m_Conneced)
  {
    // let the websockets client check for incoming messages
    if(m_oWSClient.available()) 
    {
        m_oWSClient.poll();
        //decodeMessage(m_oWSClient.readBlocking());
    }
  }
  else
  {
    connect();
  }
}

//************************************************************
//************************************************************
bool eMShome::connect(void)
{

  if (WiFi.status() != WL_CONNECTED) 
  {
    m_Conneced = false; 
    return false;
  }

  // Get access Tocken
  if (!getTocken())
  {
    m_Conneced = false; 
    return false;
  }
 
  // try to connect to Websockets server
  m_Conneced = m_oWSClient.connect(m_IP, 80, "/api/data-transfer/ws/protobuf/gdr/local/values/smart-meter");
  if(m_Conneced) 
  {
      m_oWSClient.send(m_Tocken);
      m_oWSClient.send(m_Tocken);
  } 
  else 
  {
      Serial.printf("%s(): Can't connected to WebSocked!\n",__func__);
  }
  return m_Conneced;
}

//************************************************************
//************************************************************
bool eMShome::getTocken(void)
{
  HTTPClient http;
  
  http.begin("http://"+m_IP+"/api/web-login/token");
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  String httpRequestData = "grant_type=password&client_id=emos&client_secret=56951025&username=admin&password="+m_PW;
  
  int httpResponseCode = http.POST(httpRequestData);
  String response = http.getString();  //Get the response to the request
  http.end();
  
  if (httpResponseCode != 200)
  {
    Serial.printf("Can't get access tocken, Post failed! (%d)",httpResponseCode);
    return false;
  }  
  
   StaticJsonDocument<650> jsonDoc;
   DeserializationError error = deserializeJson(jsonDoc, response);
   if (error) 
   {
     Serial.print(F("deserializeJson() failed: "));Serial.println(error.f_str());
     return false;
   }
   
   m_Tocken = String ((const char*)jsonDoc["token_type"]);
   m_Tocken += " ";
   m_Tocken += String ((const char*)jsonDoc["access_token"]);  
  
   //Serial.print("Tocken: ");
   //Serial.println( m_Tocken);

  return true;
}

//************************************************************
//************************************************************
uint32_t eMShome::getUInt32(void)
{
    uint64_t Value = 0xFFFFFFFF;

    Value = 0x7F & (unsigned char)m_buf[m_pos];
    if ((unsigned char)m_buf[m_pos++] < 0x80)
    {
        return Value;
    }

    Value = Value | ((0x7F & (unsigned char)m_buf[m_pos]) << 7);
    if ((unsigned char)m_buf[m_pos++] < 0x80)
    {
        return Value;
    }

    Value = Value | ((0x7F & (unsigned char)m_buf[m_pos]) << 14);
    if ((unsigned char)m_buf[m_pos++] < 0x80)
    {
        return Value;
    }

    Value = Value | ((0x7F & (unsigned char)m_buf[m_pos]) << 21);
    if ((unsigned char)m_buf[m_pos++] < 0x80)
    {
        return Value;
    }

    Value = Value | ((0x0F & (unsigned char)m_buf[m_pos]) << 28);
    if ((unsigned char)m_buf[m_pos++] < 0x80)
    {
        return Value;
    }

    m_pos +=5;
    if ( m_pos > m_buf.length() )
    {
        Serial.printf("%s(): Error=?\n",__func__);
    }

    return Value;

}

//************************************************************
//************************************************************
uint64_t eMShome::getUint64(void)
{
    uint32_t nLo = 0;
    uint64_t nHi = 0;
    int t = 0;
    if (! (m_buf.length() - m_pos > 4))
    {
        for (; t < 3 ;++t )
        {
            if (m_pos >= m_buf.length()) Serial.printf("%s(): ERROR1!",__func__);

            nLo = (nLo | (0x7f & (unsigned char)m_buf[m_pos]) << 7 * t);
            if ((unsigned char)m_buf[m_pos++] < 0x80)
            {
                return (nHi<<32) | nLo;
            }
        }
        nLo = (nLo | (0x7f & (unsigned char)m_buf[m_pos++]) << 7 * t);
        return nHi | nLo;
    }

    for (; t < 4 ;++t )
    {
        nLo = (nLo | (0x7f & (unsigned char)m_buf[m_pos]) << 7 * t);
        if ((unsigned char)m_buf[m_pos++] < 0x80)
        {
            return (nHi<<32) | nLo;
        }
    }

    nLo = (nLo | (0x7f & (unsigned char)m_buf[m_pos]) << 28);
    nHi = (nHi | (0x7f & (unsigned char)m_buf[m_pos]) >> 4);
    if ((unsigned char)m_buf[m_pos++] < 0x80)
    {
        return (nHi<<32) | nLo;
    }

    t = 0;
    if (m_buf.length() - m_pos > 4)
    {
        for (; t < 5 ;++t )
        {
            nHi = (nHi | (0x7f & (unsigned char)m_buf[m_pos]) << ((7 * t) + 3));
            if ((unsigned char)m_buf[m_pos++] < 0x80)
            {
                return (nHi<<32) | nLo;
            }
        }
    }
    else
    {
        for ( ; t < 5 ;++t )
        {
            if (m_pos >= m_buf.length()) Serial.printf("%s(): ERROR2!\n",__func__);
            nHi = (nHi | (0x7f & (unsigned char)m_buf[m_pos]) << ((7 * t) + 3));
            if ((unsigned char)m_buf[m_pos++] < 0x80)
            {
                return (nHi<<32) | nLo;
            }
        }
    }

    Serial.printf("%s(): ERROR3!\n",__func__);
    return 0;
}

//************************************************************
//************************************************************
String eMShome::getString(void)
{
    uint32_t len = getUInt32();
    uint32_t start = m_pos;
    uint32_t end   = m_pos+len;
    if (end >= m_buf.length())
    {
        Serial.printf("%s(): ERROR!\n",__func__);
        return "ERR!";
    }
    m_pos += len;
    return m_buf.substring(start,end);
}

//************************************************************
//************************************************************
void eMShome::isTimeSamp(void)
{
    uint32_t r ;
    uint32_t Offset = getUInt32();
    r = m_pos + Offset;

    if (Offset == 0)
    {
        r = m_buf.length();
    }

    for (;m_pos < r;)
    {
       uint32_t type = getUInt32();
       switch (type >> 3)
       {
       case(1): m_Seconds = getUint64();  break;
       case(2): m_NanoSec = getUInt32();  break;
       default: Serial.printf("skip type(8)!");  break;
       }
    }
}

//************************************************************
//************************************************************
void eMShome::GDRDecode(void)
{
    uint32_t Offset = getUInt32();
    uint32_t len = m_pos + Offset;
    uint32_t Status;
    
    if  (Offset <= 0) {len = m_buf.length();}

    for (;m_pos <  len;)
    {
        uint32_t type3 = getUInt32();
        switch (type3 >> 3){
        case(1): m_ID = getString();  break;
        case(2): m_Status = (int32_t)getUInt32();  break;
        case(3): isTimeSamp();  break;
        case(4):{
                uint64_t Key = 0;
                uint64_t Value  = 0;
                uint32_t len = getUInt32() + m_pos;
                for (;m_pos < len;)
                {

                   uint32_t type4 = getUInt32();
                   switch (type4 >> 3)
                   {
                   case(1): Key = getUint64();break;
                   case(2): Value  = getUint64(); break;
                   default: Serial.printf("skip type(4)!\n");break;
                   }
                }
                updateDataPoint(Key,Value);
            }break;
        case(5):{
                uint32_t len = getUInt32() + m_pos;
                for (;m_pos < len;)
                {
                   uint32_t type = getUInt32();
                   switch (type >> 3)
                   {
                   case(1): Serial.print("??:"); Serial.println( getString());  break;
                   case(2): Serial.printf("FlexValue.decode, not implemented\n");  break;
                   default: Serial.printf("skip type(5)!\n");break;
                   }
                }
            }break;
        default: Serial.printf("skip type(6)!\n");break;
        }
    }
}

//************************************************************
//************************************************************
void eMShome::decodeMessage(String message)
{
    static uint64_t lastSec = 0;
    m_buf = message;
    m_pos = 0;
    
    while (m_pos < m_buf.length())
    {
        uint32_t type1 = getUInt32();
        switch (type1 >> 3)
        {
          case(1):{ 
                uint32_t len = getUInt32() + m_pos;
                for (;m_pos < len;)
                {
                   uint32_t type2 = getUInt32();
                   switch (type2 >> 3)
                   {
                   case(1): m_Name = getString();  break;
                   case(2): GDRDecode(); break;
                   default: Serial.printf("unknown type(2)!\n");break;
                   }
                }
              }break;
          case(2): Serial.print("uuis:"); Serial.println( getString()); break;
          default: Serial.printf("unknown type(1)!\n");break;
        }
    }
}

//************************************************************
//************************************************************
void eMShome::updateDataPoint(uint64_t Key, uint64_t Value)
{
  for (size_t i = 0 ; i < (sizeof(dataPoint) / sizeof (sDataPoint)); i++)
  {
     if (Key == dataPoint[i].Key)
     {
        dataPoint[i].Value = Value;
     }
  }
}

//************************************************************
//************************************************************
void eMShome::printMeasurmen (void)
{
  int Y = 1;
  //DisplayClear();
  for (size_t i = 0 ; i < (sizeof(dataPoint) / sizeof (sDataPoint)); i++)
  {
    if (dataPoint[i].Value > 0)
    {
       float Value = dataPoint[i].Value;
       Value = (Value / pow(10,dataPoint[i].PowerOfTen))*dataPoint[i].Sign;
       Serial.printf("%s: %.1f %s\n",dataPoint[i].Name,Value,dataPoint[i].Unit);
       //DisplayPrint(0, Y+=10,"%s: %.1f %s\n",dataPoint[i].Name,Value,dataPoint[i].Unit);
    }
  }
}

//************************************************************
//************************************************************
// Line: 0 => Total; 1 => L1; 3 => L3; 3 => L3
int32_t eMShome::getActivePower_W(uint8_t Line)
{
  //The values sent by the server are always unsigned. Therefore it sends two values, one positive, the other must be interpreted negatively. The invalid value is always 0.
  uint8_t Index = Line * 2 ;
  if (dataPoint[Index].Value == 0)
  {
    Index ++;
  }

  return  (dataPoint[Index].Value / pow(10,dataPoint[Index].PowerOfTen))*dataPoint[Index].Sign;
}