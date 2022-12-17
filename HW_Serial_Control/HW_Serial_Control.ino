/*
FLASH Utilized: 4790 bytes
SRAM  Utilized: 229 bytes
*/
#include <EEPROM.h>

const uint16_t BAUD_RATE = 57600;  //Available Baud Rates: 600, 750, 1200, 2400, 4800, 9600, 19200, 38400, 57600

//Memory address to store vars in non-volatile EEPROM, each address is incremented by 20 bytes to ensure sufficient separation between vars in memory
const uint8_t KP_EEPROM_ADDR = 0;
const uint8_t KI_EEPROM_ADDR = 20;
const uint8_t KD_EEPROM_ADDR = 40;
const uint8_t VSETPOINT_EEPROM_ADDR = 60;
const uint8_t LOOPDELAY_EEPROM_ADDR = 80;

const uint8_t MAX_ARG_LENGTH = 6;   //max number of characters in the commands argument
const uint8_t INVALID_COMMAND_CHAR = '!';  //character is transmitted when the processed command was invalid
const uint8_t COMMAND_DELIMITER = '@';  //delimiter that signifies the beginning of a command transmission
const uint8_t ARG_DELIMITER = '(';  // delimiter that signifies the beginning of the argument transmission
const uint8_t TERMINATION_CHAR = ')';  //signifies the end of the command

float Kp = 0;   //proportional parameter in PID loop
float Ki = 0;   //integral parameter in PID loop
float Kd = 0;   //derivative parameter in PID loop
float vSetPoint = 0;   //target output voltage to maintain
uint16_t loopDelay = 0;  //delay added to stabilize PID control loop

void setup() {
  Serial.begin(BAUD_RATE);
  EEPROM.get(KP_EEPROM_ADDR, Kp);
  EEPROM.get(KI_EEPROM_ADDR, Ki);
  EEPROM.get(KD_EEPROM_ADDR, Kd);
  EEPROM.get(VSETPOINT_EEPROM_ADDR, vSetPoint);
  EEPROM.get(LOOPDELAY_EEPROM_ADDR, loopDelay);
}

void loop() {
  //while(1) loop takes less time per iteration than the void loop() by bypassing unnecessary system checks integrated into the void loop()
  while(1) {
    //Check to see if anything is available in the serial receive buffer
    while (Serial.available()) {
      static uint8_t command;   //var to hold incoming command, commands are only one byte
      static uint8_t arg[MAX_ARG_LENGTH];   //array to hold command arguments
      static uint8_t argPos = 0;  //position counter tracks which bytes of the arg have been received
      static bool receivingCommand = false;  //flag is set when the command delimiter has been received and is reset when the termination char is received
      static bool receivingArg = false;     //flag is set when the command delimiter has been received and is reset when the termination char is received

      //Read the next available byte in the serial receive buffer
      uint8_t inByte = Serial.read();

      if(inByte == COMMAND_DELIMITER) {
        receivingCommand = true;
      } else if(inByte == ARG_DELIMITER) {
        receivingArg = true;
      } else {
        //command coming in (check not terminating character)
        if (receivingCommand && inByte != TERMINATION_CHAR) {
          //Add the incoming byte to our command
          if(receivingArg) {
              arg[argPos] = inByte;
              argPos++;
          } else {
              command = inByte;
          }
        } else  if(inByte == ')') {
          //Process the command
          float arg_f = atof(arg);
          switch(command) {
            case 'P' : 
              EEPROM.put(KP_EEPROM_ADDR, arg_f);
              Kp = arg_f;
              break;
            case 'p' : 
              Serial.println(Kp);
              break;
            case 'I' : 
              EEPROM.put(KI_EEPROM_ADDR, arg_f);
              Ki = arg_f;
              break;
            case 'i' : 
              Serial.println(Ki);
              break;
            case 'D' : 
              EEPROM.put(KD_EEPROM_ADDR, arg_f);
              Kd = arg_f;
              break;
            case 'd' : 
              Serial.println(Kd);
              break;
            case 'V' : 
              EEPROM.put(VSETPOINT_EEPROM_ADDR, arg_f);
              vSetPoint = arg_f;
              break;
            case 'v' : 
              Serial.println(vSetPoint);
              break;
            case 'T' : 
              EEPROM.put(LOOPDELAY_EEPROM_ADDR, arg_f);
              loopDelay = arg_f;
              break;
            case 't' : 
              Serial.println(loopDelay);
              break;
            default :
              Serial.println(INVALID_COMMAND_CHAR);
          }
          //Reset vars
          receivingCommand = false;
          receivingArg = false;
          command = '\0';    
          for(uint8_t n = 0; n < MAX_ARG_LENGTH; n++) {
            arg[n] = '\0';
          }
          argPos = 0;
        }
      }
    }
  }
}


