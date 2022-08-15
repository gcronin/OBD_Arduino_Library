#include "OBD.h"

// constructor
OBD::OBD() {
  _ODBConnection = NULL;
}

void OBD::init(SoftwareSerial *_serial, HardwareSerial *_debug_connection) {
  _ODBConnection = _serial;
  _ODBConnection->begin(9600);
  _debug = _debug_connection;
  _debug->begin(9600);
  for(int i=0; i<rxBufferLength; i++) {  rxData[i]=0;  }
  for(int i=0; i<3; i++) {  codes[i]=0;  }
  rxIndex = 0;
}

// The following five functions extend Software Serial methods to this object
int OBD::available() {
  _ODBConnection->available();
}

void OBD::flush() {
  _ODBConnection->flush();
}

int OBD::read() {
  _ODBConnection->read();
}

int OBD::peek() {
  _ODBConnection->peek();
}

int OBD::println(const String &s) {
  _ODBConnection->println(s);
}

/*!
  @brief   Display data over the Hardware Serial object
*/
void OBD::setDebugOn() { debugLevel = true; }

/*!
  @brief   Do NOT display data over the Hardware Serial object
*/
void OBD::setDebugOff() { debugLevel = false; }

/*!
  @brief   Reset the ELM327 chip
*/
bool OBD::reset() {
  if(debugLevel) _debug->print("Rebooting...");
  delay(2000);
  flush();
  emptyRXBuffer();
  println("atz");
  char endCharacter = '>';
  char inChar=0;
  while(inChar != endCharacter){
    if(available() > 0){  inChar=read(); }
  }
  if(debugLevel) _debug->println("done.");
  delay(1000);
}

/*!
  @brief   Clear the Software Serial receive buffer
*/
void OBD::emptyRXBuffer() {
  while(available()>0) {
    read();   
  }
}

/*!
  @brief   Look for the expected response in the OBD::rxData[] array.  Return the
            array index just after the expected response or -1 if not found.
  @note    Probably unnecessary...remains from a time before the rxBuffer was cleared before each read
*/
int OBD::getDataIndexInRxArray(int responseLength, char *expectedResponse) {
  int arrayIndex = 0;
  int matchCount=0;
  for(int i=0; i<(rxBufferLength-responseLength); i++) {
    for(int j=0; j<responseLength; j++) {
      if( rxData[i+j] == expectedResponse[j] )  matchCount++; 
    }
    if(matchCount == responseLength) return (arrayIndex+responseLength);
    arrayIndex++;
  }
  return -1;
}

/*!
  @brief   Remove all Check Engine codes
*/
boolean OBD::resetCodes() {
  flush();
  emptyRXBuffer();
  println("04");
  getResponse('\r'); getResponse('\r');
  char expResponse[2] = {'4', '4'};
  if(getDataIndexInRxArray(2, &expResponse[0]) != -1)  {
    if(debugLevel) _debug->println("Codes Reset");
    return true; 
  }
  else {
    if(debugLevel) _debug->println("Error");
    return false;
  }
}

/*!
  @brief   Read Software Serial buffer till end of message character received.  Discard spaces.
  @note    The getResponse function collects incoming data from the UART into the rxData buffer
            and only exits when a designated end character is seen. Once the end character
            is detected, the rxData buffer is null terminated (so we can treat it as a string)
            and the rxData index is reset to 0 so that the next string can be copied.  Break out
            if allocated buffer would overflow or 'timeout' milliseconds have passed.
   THIS FUNCTION DEVELOPED BY SPARKFUN!!!  SMALL CHANGES MADE FOR MY APPLICATION.
   CREDIT:  https://github.com/sparkfun/OBD-II_UART
*/
void OBD::getResponse(char endCharacter, int timeout){
  char inChar=0;
  int startTime = millis();

  //Keep reading characters until we get the end character
  while(inChar != endCharacter){
    // break out if stuck or to prevent buffer overrun
    int currentTime = millis();    
    if(currentTime - startTime > timeout  || rxIndex > (rxBufferLength-1)) {
    //if(rxIndex > (rxBufferLength-1)) {
      rxIndex = 0;
      break;
    }

    //If a character comes in on the serial port, we need to act on it.
    else if(this->available() > 0){
      //Start by checking if we've received the end character
      if(this->peek() == endCharacter){
        //Clear the Serial buffer
        inChar=this->read();
        //Put the end of string character on our data string
        rxData[rxIndex]='\0';
        rxIndex = 0;
      }
      //Now check if we've received a space character (' ').
      else if(this->peek() == ' '){
        //Clear the space without saving
        inChar=this->read();
      }
      //If we didn't get the end character or a space, just add the new character to the string.
      else{
        //Get the new character from the Serial port.
        inChar = this->read();
        //Add the new character to the string, and increment the index variable.
        rxData[rxIndex++]=inChar;
      }
    }
  }
}

void OBD::getResponse(char endCharacter){
  getResponse(endCharacter, 2000);
}

/*!
  @brief   Read data for a struct PID (defined in OBD.h)
  @note    Won't work for 'at' type commands
*/
int OBD::getData(PID _PID) {
  char expResponse[5];
  flush();
  emptyRXBuffer();
  if(debugLevel) { _debug->print(_PID.displayText);  _debug->print(" "); }
  //request data from OBD
  println(_PID.commandString);

  //first response repeats sent data for OBD commands
  //second response is what we want.
  getResponse('\r');
  getResponse('\r');

  if(debugLevel) { 
    for(int i=0; i<rxBufferLength; i++) {  _debug->print(rxData[i]);  }
    _debug->print(" ");
  }
  
  //setup expected response code... same as PID with '4' at beginning
  _PID.commandString.toCharArray(expResponse, 5);
  expResponse[0] = '4';

  // find expected response code in received response...compare 4 digits 
  int syncLocation = getDataIndexInRxArray(4, &expResponse[0]);
  if(debugLevel) { _debug->print(syncLocation);  _debug->print(" "); }
  
  if(syncLocation != -1) {
    int parsedData = _PID.parseFunction(&rxData[0], syncLocation);
    if(debugLevel) { _debug->println(parsedData); }
    return parsedData;
  }
  else {
    if(debugLevel) { _debug->println(""); }
    return -1;
  }
}

/*!
  @brief   Read battery voltage, return as a float
*/
float OBD::getVoltage() {
  char expResponse[5] = {'a','t','r','v','\0'};
  flush();
  emptyRXBuffer();
  if(debugLevel) { _debug->print("Volts");  _debug->print(" "); }
  //request data from OBD
  println("atrv");
  getResponse('>');

  if(debugLevel) { 
    for(int i=0; i<rxBufferLength; i++) {  _debug->print(rxData[i]);  }
    _debug->print(" ");
  }
  
  // find expected response code in received response...compare 4 digits 
  int syncLocation = getDataIndexInRxArray(4, &expResponse[0]);
  if(debugLevel) { _debug->print(syncLocation);  _debug->print(" "); }
  
  if(syncLocation != -1) {
    String battResponse = rxData;
    battResponse = battResponse.substring(syncLocation);
    float voltage = battResponse.toFloat();
    voltage = (voltage > 8 && voltage < 20) ? voltage : -1;
    if(debugLevel) { _debug->println(voltage); }
    return voltage;
  }
  else {
    if(debugLevel) { _debug->println(""); }
    return -1;
  }
}

/*!
  @brief   Read up to three Error Codes, store in array OBD::codes[]
*/
void OBD::getCodes() {
  char expResponse[3] = {'4','3','\0'};
  flush();
  emptyRXBuffer();
  if(debugLevel) { _debug->print("Codes");  _debug->print(" "); }
  //request data from OBD
  println("03");
  getResponse('\r');
  getResponse('\r');

  if(debugLevel) { 
    for(int i=0; i<rxBufferLength; i++) {  _debug->print(rxData[i]);  }
    _debug->print(" ");
  }
  
  // find expected response code in received response...compare 2 digits 
  int syncLocation = getDataIndexInRxArray(2, &expResponse[0]);
  if(debugLevel) { _debug->print(syncLocation);  _debug->print(" "); }
  
  if(syncLocation != -1) {
    for(int j=0; j<3; j++) {
      String codeString = "";  int codeInt;
      boolean bufferOverflow = false;
      //offset is 0 first loop, 4 second loop, 8 third loop
      uint8_t indexOffset = 0;
      if(j>0) { indexOffset = (j==1) ? 4: 8; }
      //code is four characters
      for(int i=0; i<4; i++) {
        uint8_t index = syncLocation + i + indexOffset;
        if(index < rxBufferLength) {
          codeString += rxData[index];
        }
        else {
          bufferOverflow = true;
          break;
        }
      }
      codeInt = codeString.toInt();
      // error if index of rxData overflowed or toInt returned -1 (no codes are negative)
      if (codeInt<0 || bufferOverflow) {
        if(debugLevel) { _debug->print("Err"); }
        break;
      }
      else if (codeInt>0) {
        if(debugLevel) { _debug->print(codeInt);  _debug->print(" "); }
        codes[j] = codeInt;
      }
      else {
        // if first code is zero than there are no codes, otherwise just stop outer loop
        if( j == 0 ) if(debugLevel) { _debug->print("No Codes"); }
        break;
      }
    }
  }
  
  if(debugLevel) { _debug->println(""); }
}
