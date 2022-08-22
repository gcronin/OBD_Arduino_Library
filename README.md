## OBD II UART Arduino Library
 This is a library for use with the Sparkfun OBD II UART, item #WIG-09555, connected to an Arduino or similar device via a Software Serial connection.  The hardware serial is available for debugging.
 
###### Classes:  
* OBD
 
###### Public Methods:
* void init(SoftwareSerial *_serial, HardwareSerial *_debug);
* bool reset();
* boolean resetCodes();
* int getData(PID _PID);
* float getVoltage();
* void setDebugOn();
* void setDebugOff();
* void getCodes();

###### Public Variables:
* int codes[3];

###### Examples:
 **OBDSerial.ino:**  Displays data for many PIDs via a hardware Serial connection.
 
![TeraOutput](https://user-images.githubusercontent.com/2449263/185819989-9eca9f44-4e98-4654-90e2-85a8b1f94b36.png)

**OBDLCD.ino:**  Displays RPMS and MPH on a 16x2 LCD screen.
![P1040779](https://user-images.githubusercontent.com/2449263/184568562-807107c8-3e03-4241-9224-d3f49d5f2dc0.JPG)
