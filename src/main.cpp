#include <Arduino.h>
#include <U8x8lib.h>

// Program Settings

const bool DEBUG = false;

U8X8_SSD1322_NHD_256X64_4W_SW_SPI u8x8(/* clock=*/ 13, /* data=*/ 11, /* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);

const int FLIP = 1;

const int SERIAL_BAUD = 115200;
const char SERIAL_DELIM = '$';

// Declarations
enum State {COMMND, PARAM1, PARAM2, PARAM3, CHKSUM};
struct Command {
  String command = "";
  String param[3] = {"","",""};
  int curChecksum = 0;
  int tgtChecksum = 0;
};

int executeCommand(Command*);
void debugCommand(Command*);

// Variables
State state = (State)0;
Command incomingCommand = Command();

void setup() {
  // Startup the display
  u8x8.begin();
  u8x8.setPowerSave(0);
  u8x8.setFlipMode(FLIP);
  u8x8.setFont(u8x8_font_chroma48medium8_r);

  // Startup Serial
  Serial.begin(SERIAL_BAUD);
}

void loop() {
  if(Serial.available()) {
    char newChar = Serial.read();

    if(newChar == SERIAL_DELIM) {
      // Next state
      state = (State)(state + 1);
    }
    else if(newChar == '\n') {
      // End of command
      
      if (DEBUG) debugCommand(&incomingCommand);

      if(incomingCommand.curChecksum == incomingCommand.tgtChecksum) {
        // Good checksum
        if (executeCommand(&incomingCommand)) {
          Serial.println("ACK");
        } else {
          Serial.println("NAK [BAD_COMMAND]");
        }
      } else {
        // Bad checksum
        Serial.println("NAK [BAD_CHECKSUM]");
      }

      state = (State)0;
      incomingCommand = Command();
    }
    else {
      // Data
      switch(state) {
        case COMMND:
          incomingCommand.command += newChar;
          incomingCommand.curChecksum = (incomingCommand.curChecksum + newChar) % 256;
          break;
        case PARAM1:
          incomingCommand.param[0] += newChar;
          incomingCommand.curChecksum = (incomingCommand.curChecksum + newChar) % 256;
          break;
        case PARAM2:
          incomingCommand.param[1] += newChar;
          incomingCommand.curChecksum = (incomingCommand.curChecksum + newChar) % 256;
          break;
        case PARAM3:
          incomingCommand.param[2] += newChar;
          incomingCommand.curChecksum = (incomingCommand.curChecksum + newChar) % 256;
          break;
        case CHKSUM:
          incomingCommand.tgtChecksum = newChar;
          break;
      }
    }
  }
}

/*
  Executes the corresponding u8x8 command
  Returns 0 if command is not known
*/
int executeCommand(Command* command) {
  if      (command->command == "CLEAR") {
    u8x8.clear();
  } 
  else if (command->command == "DRAW_CHAR") {
    u8x8.drawGlyph(atoi(command->param[0].c_str()), atoi(command->param[1].c_str()), command->param[2].charAt(0));
  }
  else if (command->command == "DRAW_CHAR_12") {
    u8x8.draw1x2Glyph(atoi(command->param[0].c_str()), atoi(command->param[1].c_str()), command->param[2].charAt(0));
  }
  else if (command->command == "DRAW_CHAR_22") {
    u8x8.draw2x2Glyph(atoi(command->param[0].c_str()), atoi(command->param[1].c_str()), command->param[2].charAt(0));
  }
  else if (command->command == "DRAW_STRING") {
    u8x8.drawString(atoi(command->param[0].c_str()), atoi(command->param[1].c_str()), command->param[2].c_str());
  }
  else if (command->command == "DRAW_STRING_12") {
    u8x8.draw1x2String(atoi(command->param[0].c_str()), atoi(command->param[1].c_str()), command->param[2].c_str());
  }
  else if (command->command == "DRAW_STRING_22") {
    u8x8.draw2x2String(atoi(command->param[0].c_str()), atoi(command->param[1].c_str()), command->param[2].c_str());
  }
  else {
    return 0;
  }
  return 1;
}

/*
  Displays the Command command on the display
*/
void debugCommand(Command* command) {
  u8x8.clear();
  u8x8.drawString(0,0,command->command.c_str());
  u8x8.drawString(0,1,command->param[0].c_str());
  u8x8.drawString(0,2,command->param[1].c_str());
  u8x8.drawString(0,3,command->param[2].c_str());
  u8x8.drawString(0,4,String(command->tgtChecksum).c_str());
  u8x8.drawString(0,5,String(command->curChecksum).c_str());
  u8x8.drawString(0,6,(command->curChecksum == command->tgtChecksum) ? "Good Checksum" : "Bad Checksum");
}