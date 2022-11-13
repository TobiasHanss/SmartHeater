#ifndef __OUTPUT__
#define __OUTPUT__

#define NO_OF_OUTPUT 4
#define RELAY1   16
#define RELAY2   4
#define RELAY3   2
#define RELAY4   15

class COutput
{
public:
    COutput();
    
    void begin (void);

    void setOnOff(uint8_t No, char * State){ setHW(No,((State[0] == 'o') & (State[1] == 'n')) ? true : false); }
    void set(uint8_t No,bool State) { setHW(No, State); }

    String getOnOff(uint8_t No) { if(getHW(No))return "on"; return "off";}
    String getTrueFalse(uint8_t No) { if(getHW(No))return "true"; return "false";}
    bool get(uint8_t No) {return getHW(No);}

    void toggle(uint8_t No){setHW(No, !getHW(No));}

    void readButton(void);

private:

    static void taskHandler (void * ClassPointer);

    void update(void);
    void checkRules(void);
    void setHW(uint8_t No, bool Value);
    bool getHW(uint8_t No);

    bool m_bOutput[NO_OF_OUTPUT];
    bool m_bOutput_Old[NO_OF_OUTPUT];
    uint32_t m_iDelayCounter[NO_OF_OUTPUT];

    unsigned long m_iNextUpdateTime;
};

#endif