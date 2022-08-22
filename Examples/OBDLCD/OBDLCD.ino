/////////////////////////////////////////////////////////////////////////////////////////////
//
//   OBDLCD.ino
//   Connections:  OBD II UART, Sparkfun #WIG-09555, 
//        RX pin to Arduino pin 9 
//        TX pin to Arduino pin 8
//   LCD, 16x2:
//        rs = 7, en = 6, d4 = 5, d5 = 4, d6 = A1, d7 = A0;
//
//   Purpose:  Display RPMs and MPH on an LCD
//
/////////////////////////////////////////////////////////////////////////////////////////////


#include <SoftwareSerial.h>
#include <LiquidCrystal.h>
#include <OBD.h>

const int rs = 7, en = 6, d4 = 5, d5 = 4, d6 = A1, d7 = A0;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
SoftwareSerial OBD_Uart(8, 9); // RX, TX
OBD OBD;

// Methods use to turn the raw data into real values.  See, for example:
// http://www.totalcardiagnostics.com/support/Knowledgebase/Article/View/104/0/obd2-pids-for-programmers-technical

int parseRPM(char *data, int index) {
  return strtol(&data[index],0,16)/4;
}

int parseMPH(char *data, int index) {
  int spd = strtol(&data[index],0,16);
  spd = (abs(spd) > 300) ? -999 : spd;
  if(spd == -999) return -999;
  else {
    float kph = float(spd);
    float mph = (kph*0.62);
    return int(mph);
  }
}
///////////////////////////////////////////////////////////////////


// Declarations of pointers to functions///////////////////////////
parseFunction fparseRPM = parseRPM;                              //
parseFunction fparseMPH = parseMPH;                              //
///////////////////////////////////////////////////////////////////


// Declaration of PIDS/////////////////////////////////////////////
PID RPMs = {"010C", "RPMs ", fparseRPM};                         //
PID MPH = {"010D", "mph  ", fparseMPH};                          //
///////////////////////////////////////////////////////////////////

void setup() {
  OBD.init(&OBD_Uart, &Serial);
  OBD.setDebugOn();
  OBD.reset();
  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.print("MPH:");
  lcd.setCursor(0, 1);
  lcd.print("RPMs:");
}

void loop() {
  lcd.setCursor(5, 0);
  lcd.print("       ");
  lcd.setCursor(5, 0);
  lcd.print(OBD.getData(MPH));
  delay(600);
  lcd.setCursor(5, 1);
  lcd.print("      ");
  lcd.setCursor(5, 1);
  lcd.print(OBD.getData(RPMs));
  delay(600);
}
