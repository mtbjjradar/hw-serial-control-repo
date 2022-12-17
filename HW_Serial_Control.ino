#include <EEPROM.h>

const uint16_t BAUD_RATE = 57600;

const byte KP_EEPROM_ADDR = 0;
const byte KI_EEPROM_ADDR = 20;
const byte KD_EEPROM_ADDR = 40;
const byte VSETPOINT_EEPROM_ADDR = 60;
const byte LOOPDELAY_EEPROM_ADDR = 80;

const byte MAX_ARG_LENGTH = 5;
const char INVALID_COMMAND_CHAR = '!';  //character is transmitted when the processed command was invalid
const char COMMAND_DELIMITER = '@';  //command delimiter that signifies the beginning of a command transmission

volatile float Kp = 0;
volatile float Ki = 0;
volatile float Kd = 0;
volatile float vSetPoint = 0;
volatile float loopDelay = 0;

void setup() {
  Serial.begin(BAUD_RATE);
  EEPROM.get(KP_EEPROM_ADDR, Kp);
  EEPROM.get(KI_EEPROM_ADDR, Ki);
  EEPROM.get(KD_EEPROM_ADDR, Kd);
  EEPROM.get(VSETPOINT_EEPROM_ADDR, vSetPoint);
  EEPROM.get(LOOPDELAY_EEPROM_ADDR, loopDelay);
}

void loop() {
  while(1) {
    //Check to see if anything is available in the serial receive buffer
    PORTB ^= (1 << PB0);
    while (Serial.available()) {
      PORTB |= (1 << PB1);
      //Create a place to hold the incoming command
      static char command;
      static char arg[MAX_ARG_LENGTH];
      static unsigned int argPos = 0;
      static bool receivingCommand = false;
      static bool receivingArg = false;

      //Read the next available byte in the serial receive buffer
      char inByte = Serial.read();
      if(!receivingArg) {  //Debugging line, comment out after debugging.
        Serial.print("\n//////Serial.available()Loop//////\ninByte: ");  //Debugging line, comment out after debugging.
        Serial.println(inByte);  //Debugging line, comment out after debugging.
        Serial.print("receivingCommand: ");  //Debugging line, comment out after debugging.
        Serial.println(receivingCommand ?  "true" : "false");  //Debugging line, comment out after debugging.
        Serial.print("receivingArg: ");  //Debugging line, comment out after debugging.
        Serial.println(receivingArg ?  "true" : "false");  //Debugging line, comment out after debugging.
        Serial.print("argPos: ");  //Debugging line, comment out after debugging.
        Serial.println(argPos);  //Debugging line, comment out after debugging.
      }  //Debugging line, comment out after debugging.

      if(inByte == COMMAND_DELIMITER) {
        receivingCommand = true;
        Serial.println("receivingCommand = true;");  //Debugging line, comment out after debugging.
      } else if(inByte == '(') {
        receivingArg = true;
        Serial.println("receivingArg = true;");  //Debugging line, comment out after debugging.
      } else {
        //command coming in (check not terminating character)
        if (receivingCommand && inByte != ')' && inByte != '\n') {
          //Add the incoming byte to our command
          Serial.print("\n//////ReceivingCommandLoop//////\ninByte: ");  //Debugging line, comment out after debugging.
          Serial.println(inByte);  //Debugging line, comment out after debugging.
          switch(receivingArg) {
            case false : 
              command = inByte;
              Serial.print("command = ");  //Debugging line, comment out after debugging.
              Serial.print(command);  //Debugging line, comment out after debugging.
              Serial.println(";");  //Debugging line, comment out after debugging.
              break;
            case true : 
              arg[argPos] = inByte;
              Serial.print("arg[");  //Debugging line, comment out after debugging.
              Serial.print(argPos);  //Debugging line, comment out after debugging.
              Serial.print("] = ");  //Debugging line, comment out after debugging.
              Serial.print(inByte);  //Debugging line, comment out after debugging.
              Serial.println(";");  //Debugging line, comment out after debugging.
              argPos++;
              break;
          }
        } else  if(inByte == ')') {
          //Print the command (or do other things)
          Serial.print("\n//////ProcessCommandLoop//////\n");  //Debugging line, comment out after debugging.
          Serial.print(command);  //Debugging line, comment out after debugging.
          Serial.print("(");  //Debugging line, comment out after debugging.
          Serial.print(arg);  //Debugging line, comment out after debugging.
          Serial.println(")");  //Debugging line, comment out after debugging.

          float arg_f = atof(arg);
          Serial.print("arg_f: ");  //Debugging line, comment out after debugging.
          Serial.println(arg_f);  //Debugging line, comment out after debugging.

          switch(command) {
            case 'P' : 
              EEPROM.put(KP_EEPROM_ADDR, arg_f);
              Kp = arg_f;
              break;
            case 'p' : 
              Serial.print("Kp: ");  //Debugging line, comment out after debugging.
              Serial.println(Kp);
              break;
            case 'I' : 
              EEPROM.put(KI_EEPROM_ADDR, arg_f);
              Ki = arg_f;
              break;
            case 'i' : 
              Serial.print("Ki: ");  //Debugging line, comment out after debugging.
              Serial.println(Ki);
              break;
            case 'D' : 
              EEPROM.put(KD_EEPROM_ADDR, arg_f);
              Kd = arg_f;
              break;
            case 'd' : 
              Serial.print("Kd: ");  //Debugging line, comment out after debugging.
              Serial.println(Kd);
              break;
            case 'V' : 
              EEPROM.put(VSETPOINT_EEPROM_ADDR, arg_f);
              vSetPoint = arg_f;
              break;
            case 'v' : 
              Serial.print("VsetPoint: ");  //Debugging line, comment out after debugging.
              Serial.println(vSetPoint);
              break;
            case 'T' : 
              EEPROM.put(LOOPDELAY_EEPROM_ADDR, arg_f);
              loopDelay = arg_f;
              break;
            case 't' : 
              Serial.print("loopDelay: ");  //Debugging line, comment out after debugging.
              Serial.println(loopDelay);
              break;
            default :
              //Serial.println(INVALID_COMMAND_CHAR);  //Comment this line out for debugging, MAKE SURE IT IS NOT COMMENTED BEFORE PRODUCTION UPLOAD
              Serial.print("'");  //Debugging line, comment out after debugging.
              Serial.print(command);  //Debugging line, comment out after debugging.
              Serial.print("' is not a valid command character!");  //Debugging line, comment out after debugging.
          }
          receivingCommand = false;
          Serial.println("receivingCommand = false;");  //Debugging line, comment out after debugging.
          receivingArg = false;
          Serial.println("receivingArg = false;");  //Debugging line, comment out after debugging.
          command = '\0';
          for(int i = 0; i < MAX_ARG_LENGTH; i++) {
            arg[i] = '\0';
          }
          argPos = 0;
        }
      }
      PORTB &= ~(1 << PB1);
    }
  }
}
