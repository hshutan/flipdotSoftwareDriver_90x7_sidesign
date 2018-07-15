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
#include "Arduino.h"
#include "Modbus_CoProcessor.h"

// This serial port should be connected to an RS485 converter. Also define this in the main program.
#define SERIALDEVICE Serial2 

mcp::mcp(int baudRate)
{
  //SERIALDEVICE.begin(baudRate); // Moved to main code

  // Init the human readable bitmap with 0s
  dotAllOff();

  // Init the byte array with 0s
  for (int i = 0; i < byteStreamSize; i++) {
    Bytestream[i] = 0;
  }

}

void mcp::dotOn(byte x, byte y)
{
  // Change a dot in bitmap to on but only if it is within the bounds of the array
  // Gracefully fails, allowing you to paint partially off-screen if needed
  if ((x < xSize) && (x >= 0) && (y < ySize) && (y >= 0)) {
    BitmapMatrix[x][y] = 1;
  }
  // else, do nothing, throw away the out of bounds data.
}

void mcp::dotOff(byte x, byte y)
{
  // Change a dot in bitmap to off but only if it is within the bounds of the array
  // Gracefully fails, allowing you to paint partially off-screen if needed
  if ((x < xSize) && (x >= 0) && (y < ySize) && (y >= 0)) {
    BitmapMatrix[x][y] = 0;
  }
}
// else, do nothing, throw away the out of bounds data.

void mcp::dotInvert(byte x, byte y)
{
  // There is no OOB checking in this function.
  //
  if (BitmapMatrix[x][y]) {
    BitmapMatrix[x][y] = 0;
  } else {
    BitmapMatrix[x][y] = 1;
  };
}

void mcp::dotAllOn()
{
  // Init the 2D array with all 1s
  for (int x = 0; x < xSize; x++) {
    for (int y = 0; y < ySize; y++) {
      BitmapMatrix[x][y] = 1;
    }// close for x
  }// close for y
}

void mcp::dotAllOff()
{
  // Init the 2D array with all 0s
  for (int x = 0; x < xSize; x++) {
    for (int y = 0; y < ySize; y++) {
      BitmapMatrix[x][y] = 0;
    }// close for x
  }// close for y
}

void mcp::invertAll()
{
  // Invert all data in the 2D array
  for (int x = 0; x < xSize; x++) {
    for (int y = 0; y < ySize; y++) {
      if (BitmapMatrix[x][y] == 0) {
        BitmapMatrix[x][y] = 1;
      } else {
        BitmapMatrix[x][y] = 0;
      }
    }
  }
}


void mcp::UpdateSign()
{
  // This will take everything in the human bitmap,
  // Convert it to the correct ordering and stream of bytes
  // Then print those bytes out as ASCII modbus, with checksum, to serial port

  // Convert human bitmap to correct stream of bytes for flipdot
  ConvertBitmapToBytestream();

  // Print static start of image command
  PrintString(":01000303A257");

  // Print computed registers 0 thru 5
  PrintRegister0();
  PrintRegister1();
  PrintRegister2();
  PrintRegister3();
  PrintRegister4();
  PrintRegister5();

  // Print static end of image code and tell sign to display it!
  PrintString(":00000C01F3");
  PrintString(":0100030200FA");
  PrintString(":0100030600F6");
  PrintString(":0100030200FA");
  PrintString(":01000303A950");
}

void mcp::ConvertBitmapToBytestream()
{
  int byteStreamCounter = 0;
  for (int x = 0; x < xSize; x++) { // Loop thru each column

    for (int y1 = 7; y1 > -1; y1--) { // Loop thru the first 8 bits of a column
      if (BitmapMatrix[x][y1] == 1) {
        bitSet(Bytestream[byteStreamCounter], 0); // Set bit to a 1 in the right most place of the byte
      } else {
        bitClear(Bytestream[byteStreamCounter], 0); // Set bit to a 0 in the right most place of the byte
      }
      if (y1 > 0) { // This if prevents the bitshift << from happening more than 8 times
        Bytestream[byteStreamCounter] <<= 1; // Because bitSet and bitClear write to the LSB (right most) bit,
        //bump all the bits over to the left one position to prepare the LSB for next set or clear
      }
    }// close for first 8 bits of a column
    byteStreamCounter++;
  }// close for x
}

void mcp::InitSign()
{
  // This must be run at least once after the sign is physically powered on.
  // This command puts the sign into "ready" mode, where it waits for new data.
  // Note that this is hardcoded to sign ID 3, also checksums are hardcoded.
  PrintString(":01000502FFF9");
  PrintString(":01000602FFF8");
  PrintString(":01000303A158");
  PrintString(":1000000004200006071E1E1E00080000000000005D");
  PrintString(":00000101FE");
  PrintString(":0100030200FA");
}

void mcp::PrintString(String in)
{
  // This will write data to the serial device that is hooked to RS485
  // In case the sign is talking, wait for it to finish
  while (SERIALDEVICE.available() > 0) {
    SERIALDEVICE.read();
  }

  SERIALDEVICE.println(in); // println default EOL is CRLF, good for modbus
  SERIALDEVICE.flush(); // Wait for the serial port to finish sending
  delay(11); // Delay after each line is sent

  // In case the sign is responding, wait for it to finish
  while (SERIALDEVICE.available() > 0) {
    SERIALDEVICE.read();
  }
}

void mcp::PrintRegister0()
{
  // Loop thru correct number of bytes for this register
  // Concat any before/after text
  // Call calculate checksum
  // Print final String

  int startingPoint = 0; // R0 starts at byte 0
  int numOfBytesToChewOff = 12; // R0 needs 12 bytes of data
  String prependData = ""; // Per-register string to start with
  String middleData = "";
  String checksum = ""; // Per-register checksum to be calculated

  prependData = ":1000000001100000";

  for (int i = startingPoint; i < startingPoint + numOfBytesToChewOff; i++) {
    char sBuffer [4];
    sprintf(sBuffer, "%0.2X", Bytestream[i]);
    middleData.concat(sBuffer[0]);
    middleData.concat(sBuffer[1]);
  }

  checksum = calculateLRC(prependData + middleData);
  PrintString(prependData + middleData + checksum);
}

void mcp::PrintRegister1()
{
  // Loop thru correct number of bytes for this register
  // Concat any before/after text
  // Call calculate checksum
  // Print final String

  int startingPoint = 12; // R1 starts at byte 12
  int numOfBytesToChewOff = 16; // R1 needs 16 bytes of data
  String prependData = ""; // Per-register string to start with
  String middleData = "";
  String checksum = ""; // Per-register checksum to be calculated

  prependData = ":10001000";

  for (int i = startingPoint; i < startingPoint + numOfBytesToChewOff; i++) {
    char sBuffer [4];
    sprintf(sBuffer, "%0.2X", Bytestream[i]);
    middleData.concat(sBuffer[0]);
    middleData.concat(sBuffer[1]);
  }

  checksum = calculateLRC(prependData + middleData);
  PrintString(prependData + middleData + checksum);
}

void mcp::PrintRegister2()
{
  // Loop thru correct number of bytes for this register
  // Concat any before/after text
  // Call calculate checksum
  // Print final String

  int startingPoint = 28; // R2 starts at byte 28
  int numOfBytesToChewOff = 16; // R2 needs 16 bytes of data
  String prependData = ""; // Per-register string to start with
  String middleData = "";
  String checksum = ""; // Per-register checksum to be calculated

  prependData = ":10002000";

  for (int i = startingPoint; i < startingPoint + numOfBytesToChewOff; i++) {
    char sBuffer [4];
    sprintf(sBuffer, "%0.2X", Bytestream[i]);
    middleData.concat(sBuffer[0]);
    middleData.concat(sBuffer[1]);
  }

  checksum = calculateLRC(prependData + middleData);
  PrintString(prependData + middleData + checksum);
}

void mcp::PrintRegister3()
{
  // Loop thru correct number of bytes for this register
  // Concat any before/after text
  // Call calculate checksum
  // Print final String

  int startingPoint = 44; // R3 starts at byte 28
  int numOfBytesToChewOff = 16; // R3 needs 16 bytes of data
  String prependData = ""; // Per-register string to start with
  String middleData = "";
  String checksum = ""; // Per-register checksum to be calculated

  prependData = ":10003000";

  for (int i = startingPoint; i < startingPoint + numOfBytesToChewOff; i++) {
    char sBuffer [4];
    sprintf(sBuffer, "%0.2X", Bytestream[i]);
    middleData.concat(sBuffer[0]);
    middleData.concat(sBuffer[1]);
  }

  checksum = calculateLRC(prependData + middleData);
  PrintString(prependData + middleData + checksum);
}

void mcp::PrintRegister4()
{
  // Loop thru correct number of bytes for this register
  // Concat any before/after text
  // Call calculate checksum
  // Print final String

  int startingPoint = 60; // R4 starts at byte 28
  int numOfBytesToChewOff = 16; // R4 needs 16 bytes of data
  String prependData = ""; // Per-register string to start with
  String middleData = "";
  String checksum = ""; // Per-register checksum to be calculated

  prependData = ":10004000";

  for (int i = startingPoint; i < startingPoint + numOfBytesToChewOff; i++) {
    char sBuffer [4];
    sprintf(sBuffer, "%0.2X", Bytestream[i]);
    middleData.concat(sBuffer[0]);
    middleData.concat(sBuffer[1]);
  }

  checksum = calculateLRC(prependData + middleData);
  PrintString(prependData + middleData + checksum);
}

void mcp::PrintRegister5()
{
  // Loop thru correct number of bytes for this register
  // Concat any before/after text
  // Call calculate checksum
  // Print final String

  int startingPoint = 76; // R5 starts at byte 28
  int numOfBytesToChewOff = 14; // R5 needs 14 bytes of data
  String prependData = ""; // Per-register string to start with
  String middleData = "";
  String checksum = ""; // Per-register checksum to be calculated

  prependData = ":10005000";

  for (int i = startingPoint; i < startingPoint + numOfBytesToChewOff; i++) {
    char sBuffer [4];
    sprintf(sBuffer, "%0.2X", Bytestream[i]);
    middleData.concat(sBuffer[0]);
    middleData.concat(sBuffer[1]);
  }

  middleData.concat("0000"); // Special padding for last register.

  checksum = calculateLRC(prependData + middleData);
  PrintString(prependData + middleData + checksum);
}

String mcp::calculateLRC(String input)
{
  char * a = (char *)input.c_str();
  int myLength = strlen(a);
  int val[myLength / 2];
  for (int i = 1, j = 0 ; i < strlen(a); i = i + 2, j++ )
  {
    val[j] = conv(a[i], a[i + 1]);
  }
  int sum = find_sum(val, myLength);
  char hex[5];
  utoa((unsigned)sum, hex, 16);
  int hex_val = (int)strtol(hex, NULL, 16);
  hex_val = ((~hex_val) + B01) & 0xff;
  char hex_val_str[4];
  sprintf(hex_val_str, "%0.2X", hex_val);
  String finally = hex_val_str;
  return finally;
}

int mcp::toDec(char val)
{
  if (val <= '9')
  {
    return val - '0';
  }
  else
  {
    return val - '0' - 7;
  }
}

int mcp::conv(char val1, char val2)
{
  int val_a = toDec(val1);
  int val_b = toDec(val2);
  return (val_a * 16) + val_b;
}

int mcp::find_sum(const int * val, int myLength)
{
  {
    int sum = 0;
    for (int i = 0; i <= (myLength / 2) - 1; i++)
    {
      sum = sum + val[i];
    }
    return sum;
  }
}


