#ifndef OBD_H
#define OBD_H

#include <Arduino.h>
#include <SoftwareSerial.h>

// Definitions  //////////////////////////////////////////
typedef int (*parseFunction) (char *data, int index);   //
                                                        //
typedef struct{                                         //
  String commandString;                                 //
  String displayText;                                   //
  parseFunction parseFunction;                          //
} PID;                                                  //
//////////////////////////////////////////////////////////

const uint8_t rxBufferLength = 20;

class OBD {

  private:
    SoftwareSerial *_ODBConnection;
    HardwareSerial *_debug;
    char rxData[rxBufferLength];
    char rxIndex=0;
    boolean debugLevel = false;
    int println(const String &s);
    bool listen();
    virtual int available();
    virtual void flush();
    virtual int read();
    virtual int peek();
    void emptyRXBuffer();
    int getDataIndexInRxArray(int responseLength, char *expectedResponse);
    void getResponse(char endCharacter, int timeout);
    void getResponse(char endCharacter);
    
  public:
    OBD();
    void init(SoftwareSerial *_serial, HardwareSerial *_debug);
    bool reset();
    boolean resetCodes();
    int getData(PID _PID);
    float getVoltage();
    void setDebugOn();
    void setDebugOff();
    void getCodes();
    int codes[3];
};

#endif