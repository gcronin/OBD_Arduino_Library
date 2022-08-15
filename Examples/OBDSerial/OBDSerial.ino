/////////////////////////////////////////////////////////////////////////////////////////////
//
//   OBDSerial.ino
//   Connections:  OBD II UART, Sparkfun #WIG-09555, 
//        RX pin to Arduino pin 9 
//        TX pin to Arduino pin 8
//
//   Purpose:  Displays many of the available PIDs over the Serial monitor
//
/////////////////////////////////////////////////////////////////////////////////////////////


#include <SoftwareSerial.h>
#include <OBD.h>

SoftwareSerial OBD_Uart(8, 9); // RX, TX
OBD OBD;

// Methods use to turn the raw data into real values.  See, for example:
// http://www.totalcardiagnostics.com/support/Knowledgebase/Article/View/104/0/obd2-pids-for-programmers-technical

int parsePercent(char *data, int index) {
  int percent = strtol(&data[index],0,16)*100/255;
  return (abs(percent) > 100) ? -1 : percent;
}

int parseTemp(char *data, int index) {
  int temp = strtol(&data[index],0,16)-40;
  temp = (abs(temp) > 500) ? -1 : temp;
  if(temp == -1) return -1;
  else {
    float tempC = float(temp);
    float tempF = (tempC*1.8)+32;
    return int(tempF);
  }
}

int parseRPM(char *data, int index) {
  return strtol(&data[index],0,16)/4;
}

int parseMPH(char *data, int index) {
  int spd = strtol(&data[index],0,16);
  spd = (abs(spd) > 300) ? -1 : spd;
  if(spd == -1) return -1;
  else {
    float kph = float(spd);
    float mph = (kph*0.62);
    return int(mph);
  }
}

int parseTiming(char *data, int index) {
  int timing = strtol(&data[index],0,16)/2-64;
  return (abs(timing) > 64) ? -1 : timing;
}

int parseMAF(char *data, int index) {
  int flow = strtol(&data[index],0,16)/100;
  return (flow > 655 || flow < 0) ? -1 : flow;
}

int parseFuelTrim(char *data, int index) {
  int percent = (strtol(&data[index],0,16)-128)*100/128;
  return (abs(percent) > 100) ? -1 : percent;
}

int parseO2Sense(char *data, int index) {
  char Hex[3]; Hex[0] = data[index]; Hex[1] = data[index+1]; Hex[2] = '\0';
  int HighByte = strtol(Hex,0,16);
  Hex[0] = data[index+2]; Hex[1] = data[index+3];
  int LowByte = strtol(Hex,0,16);
  int temp = LowByte - 128;
  temp = temp*HighByte;
  return temp/256;
}
///////////////////////////////////////////////////////////////////


// Declarations of pointers to functions///////////////////////////
parseFunction fparsePercent = parsePercent;                      //
parseFunction fparseTemp = parseTemp;                            //
parseFunction fparseRPM = parseRPM;                              //
parseFunction fparseMPH = parseMPH;                              //
parseFunction fparseTiming = parseTiming;                        //
parseFunction fparseMAF = parseMAF;                              //
parseFunction fparseFuelTrim = parseFuelTrim;                    //
parseFunction fparseO2Sense = parseO2Sense;                      //
///////////////////////////////////////////////////////////////////


// Declaration of PIDS/////////////////////////////////////////////
PID Load = {"0104", "Load%", fparsePercent};                     //
PID EngineTemp = {"0105", "Eng F", fparseTemp};                  //
PID RPMs = {"010C", "RPMs ", fparseRPM};                         //
PID MPH = {"010D", "mph  ", fparseMPH};                          //
PID AirTemp = {"010F", "Air F", fparseTemp};                     //
PID Throttle = {"0111", "Thrt%", fparsePercent};                 //
PID Timing = {"010E", "Timing", fparseTiming};                   //
PID MAF = {"0110", "Air Flow", fparseMAF};                       //
PID SFuelTrim = {"0106", "SFuelTrm", fparseFuelTrim};            //
PID LFuelTrim = {"0107", "LFuelTrm", fparseFuelTrim};            //  
PID O2Sense1 = {"0114", "O2 Sens1", fparseO2Sense};              // 
PID O2Sense2 = {"0115", "O2 Sens2", fparseO2Sense};              // 
///////////////////////////////////////////////////////////////////

void setup() {
  OBD.init(&OBD_Uart, &Serial);
  OBD.setDebugOn();
  OBD.reset();
}

void loop() {
  OBD.getData(Load);
  delay(1000);
  OBD.getData(EngineTemp);
  delay(1000);
  OBD.getData(RPMs);
  delay(1000);
  OBD.getData(MPH);
  delay(1000);
  OBD.getData(AirTemp);
  delay(1000);
  OBD.getData(Throttle);
  delay(1000);
  OBD.getData(Timing);
  delay(1000);
  OBD.getData(MAF);
  delay(1000);
  OBD.getData(SFuelTrim);
  delay(1000);
  OBD.getData(LFuelTrim);
  delay(1000);
  OBD.getData(O2Sense1);
  delay(1000);
  OBD.getData(O2Sense2);
  delay(1000);
  OBD.getVoltage();
  delay(1000);
  OBD.getCodes();
  delay(1000);
}
