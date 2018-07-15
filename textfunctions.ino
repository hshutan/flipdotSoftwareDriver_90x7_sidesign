void writeCharToPosition(char inChar, int inPos) {
  // Overload method; if you do not specify V offset
  writeCharToPositionWithOffset(inChar, inPos, 0);
}

void writeCharToPositionWithOffset(char inChar, int inPos, int inVOffset) {
  // Writes individual ASCI characters at pre-programmed static locations.
  // Displays exactly 15 characters (positions 0 thru 14) on the display, no more.
  // Allows for a V offset
  if ((inChar > 0) && (inChar < 127)) {

    for (int r = 0; r < 5; r++) {
      for (int c = 0; c < 7; c++) {

        if (bitRead(font[inChar][r], c)) {
          mcp.dotOn(r + (inPos * 6), c + inVOffset);
        }
      }
    }
  }
}

void writeString(String inString) {
    // Overload method; if you do not specify an X starting point.
  writeStringAtX(inString, 0);
}

void writeStringAtX(String inString, int inX) {
  // Displays a given String, starting from a given X position (usually 0)
  // Can potentially fit slightly more than 15 characters thanks to limited kerning functionality.
  int kerning = 0;
  kerning = kerning + inX;
  inString.remove(32); // Trim String from being far too large

  // For each character in the string
  for (int i = 0; i < inString.length(); i++) {
    char charToWrite = inString.charAt(i);

    // Limited Kerning Function #1 of 2
    if (charToWrite == 'l') {
      kerning = kerning - 1;
    } else if (charToWrite == 'I') {
      kerning = kerning - 1;
    } else if (charToWrite == 'i') {
      kerning = kerning - 1;
    } else if (charToWrite == '\'') {
      kerning = kerning - 2;
    } else if (charToWrite == '.') {
      kerning = kerning - 1;
    }

    // Basic out of bounds checking, then writes character to human bitmap
    if ((charToWrite > 0) && (charToWrite < 127)) {
      for (int x = 0; x < 5; x++) {
        for (int y = 0; y < 7; y++) {

          if (bitRead(font[charToWrite][x], y)) {
            mcp.dotOn(x + kerning, y);
          }
        }
      }
    }

    // Limited Kerning Function #2 of 2
    // Sorry about all the else ifs.
    if (charToWrite == ':') {
      kerning = kerning + 5;
    } else if (charToWrite == ' ') {
      kerning = kerning + 3;
    } else if (charToWrite == 'l') {
      kerning = kerning + 5;
    } else if (charToWrite == 'r') {
      kerning = kerning + 5;
    } else if (charToWrite == ',') {
      kerning = kerning + 5;
    } else if (charToWrite == '1') {
      kerning = kerning + 5;
    } else if (charToWrite == 'j') {
      kerning = kerning + 5;
    } else if (charToWrite == 'I') {
      kerning = kerning + 5;
    } else if (charToWrite == 'i') {
      kerning = kerning + 5;
    } else if (charToWrite == 't') {
      kerning = kerning + 5;
    } else if (charToWrite == '\'') {
      kerning = kerning + 4;
    } else if (charToWrite == '.') {
      kerning = kerning + 5;
    } else {
      // Normally add 6 spaces from the beginning of the last character
      kerning = kerning + 6;
    }

  }

}
