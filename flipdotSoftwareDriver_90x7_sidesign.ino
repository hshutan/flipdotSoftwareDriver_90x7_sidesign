/*
   This program will allow basic software control of a flipdot sign.

   Built for a:
   GTI Luminator MegaMax 3000 Side Sign 90x7 (Sign ID: 3)
   All copyright for sign and communication protocol to their
   original owners/creators.
   
   Harrison Shutan - Jul 2018 - https://github.com/hshutan
*/

// This serial port should be connected to an RS485 converter. Also define this in Modbus_CoProcessor.cpp
#define SERIALDEVICE Serial2

#include "font.h"
#include "Modbus_CoProcessor.h"

mcp mcp(19200); // Prepare object, set serial baud to 19200

void setup() {

  delay(1000);
  // Start serial
  SERIALDEVICE.begin(19200);
  delay(250);
  // After sign is powered on, send init command
  mcp.InitSign();
  delay(250);

}

void loop() {

  testFunction1("Hello!");
  delay(7000);
  testFunction1("The sign works.");
  delay(7000);

}

void testFunction1(String inString)
{
  // Prints a string to sign
  mcp.dotAllOff(); // Clear human bitmap if it is not already clear
  writeString(inString);
  mcp.dotInvert(89, 6); // Fix for one flipdot that is always inverted.
  mcp.UpdateSign();
}

void testFunction2()
{
  // Turns all flipdots on, then fades away to reveal a String
  String myString = "This is a test...";
  mcp.dotAllOn();
  mcp.dotInvert(89, 6); // Fix for one flipdot that is always inverted.
  mcp.UpdateSign();
  delay(3000);

  mcp.dotAllOn();

  for (int i = 0; i < 5; i++) {

    for (int i = 0; i < 375; i++) {
      mcp.dotOff(random(-1, 91), random(-1, 8));
    }
    writeStringAtX(myString, 0);
    mcp.UpdateSign();
    delay(1000);
  }

  mcp.dotAllOff();
  mcp.dotInvert(89, 6); // Fix for one flipdot that is always inverted.
  writeStringAtX(myString, 0);
  mcp.UpdateSign();
  delay(1000);

}
