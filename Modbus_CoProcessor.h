/*
   This library will allow basic software control of a flipdot sign.

   Built for a:
   GTI Luminator MegaMax 3000 Side Sign 90x7 (Sign ID: 3)
   All copyright for sign and communication protocol to their
   original owners/creators.

   The sign uses a form of ASCII modbus to communicate
   over RS485. This library outputs serial data, which
   should be physically connected to a TTL->RS485 device.

   Harrison Shutan - Jul 2018 - https://github.com/hshutan
*/
#ifndef Modbus_CoProcessor_h
#define Modbus_CoProcessor_h

#include "Arduino.h"

// Note that this library is tailored to 90x7. It would require work to adapt to other sizes.
const int xSize = 90; // Enter the real number of x dots (1 indexed)
const int ySize = 8; // Enter the real number of y dots (1 indexed) (This is 8 due to how the sign's bitmap is rendered)
const int byteStreamSize = ((xSize*ySize) / 8);

class mcp
{
  public:
    mcp(int baudRate);
    void dotOn(byte x, byte y);
    void dotOff(byte x, byte y);
    void dotInvert(byte x, byte y);
    void dotAllOn();
    void dotAllOff();
    void invertAll();
    void UpdateSign();
    void ConvertBitmapToBytestream();
    void InitSign();
    void PrintString(String in);
    void PrintRegister0();
    void PrintRegister1();
    void PrintRegister2();
    void PrintRegister3();
    void PrintRegister4();
    void PrintRegister5();
    // Credit to author Kunchala Anil for C++ Arduino modbus LRC calculation code below:
    String calculateLRC(String input);
    int toDec(char val);
    int conv(char val1,char val2);
    int find_sum(const int * val, int myLength);
    
  private:
    bool BitmapMatrix[xSize][ySize]; // Create a 2D array to hold the "human" readable bitmap sign image.
    byte Bytestream[byteStreamSize]; // Create a stream of bytes that will be sent to the sign via modbus
};

#endif
