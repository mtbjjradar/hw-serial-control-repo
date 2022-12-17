  /*
FLASH Utilized: 4790 bytes
SRAM  Utilized: 229 bytes

Debugging With Saleae requires digital channels 0,1,2,3
0 -> pin 8 PB0
1 -> pin 9 PB1
2 -> pin 0 RX
3 -> pin 1 TX

Set capture mode to trigger on channel 1, rising edge.
Capture duration after trigger: 50ms
Trim pre-trigger data: 50ms

50ns Glitch filters on channels 0 and 1

Max sampling rate of 500MS/s prefered, minimum sampling rate to retrieve accurate timing data is 10MS/s, leaving a temporal resolution of 100ns
Memory Buffer set to 1GB, but this could be set much lower if needed.
*/
#include <EEPROM.h>

const uint16_t BAUD_RATE = 57600;  //Available Baud Rates: 600, 750, 1200, 2400, 4800, 9600, 19200, 38400, 57600

const uint8_t KP_EEPROM_ADDR = 0;
const uint8_t KI_EEPROM_ADDR = 20;
const uint8_t KD_EEPROM_ADDR = 40;
const uint8_t VSETPOINT_EEPROM_ADDR = 60;
const uint8_t LOOPDELAY_EEPROM_ADDR = 80;

const uint8_t MAX_ARG_LENGTH = 6;
const uint8_t INVALID_COMMAND_CHAR = '!';  //character is transmitted when the processed command was invalid
const uint8_t COMMAND_DELIMITER = '@';  //command delimiter that signifies the beginning of a command transmission

volatile float Kp = 0;   
volatile float Ki = 0;
volatile float Kd = 0;
volatile float vSetPoint = 0;
volatile uint16_t loopDelay = 0;

void setup() {
  Serial.begin(BAUD_RATE);
  EEPROM.get(KP_EEPROM_ADDR, Kp);
  EEPROM.get(KI_EEPROM_ADDR, Ki);
  EEPROM.get(KD_EEPROM_ADDR, Kd);
  EEPROM.get(VSETPOINT_EEPROM_ADDR, vSetPoint);
  EEPROM.get(LOOPDELAY_EEPROM_ADDR, loopDelay);
  DDRB |= (1 << PB0);
  DDRB |= (1 << PB1);
}

void loop() {.

  while(1) {
    //Check to see if anything is available in the serial receive buffer
    delay(1);
    PORTB &= ~(1 << PB0);  /////////////////////////////////////////////////////////////////////////////Debugging line, comment out after debugging.///////////////////////////////////////////////////////////////////////////
    PORTB &= ~(1 << PB1);  /////////////////////////////////////////////////////////////////////////////Debugging line, comment out after debugging.///////////////////////////////////////////////////////////////////////////
    while (Serial.available()) {
      PORTB |= (1 << PB0);  /////////////////////////////////////////////////////////////////////////////Debugging line, comment out after debugging.///////////////////////////////////////////////////////////////////////////
      //Create a place to hold the incoming command
      static uint8_t command;
      static uint8_t arg[MAX_ARG_LENGTH];
      static uint8_t argPos = 0;
      static bool receivingCommand = false;
      static bool receivingArg = false;

      //Read the next available byte in the serial receive buffer
      uint8_t inByte = Serial.read();

      if(inByte == COMMAND_DELIMITER) {
        receivingCommand = true;
      } else if(inByte == '(') {
        receivingArg = true;
      } else {
        //command coming in (check not terminating character)
        if (receivingCommand && inByte != ')' && inByte != '\n') {
          //Add the incoming byte to our command
          if(receivingArg) {
              arg[argPos] = inByte;
              argPos++;
          } else {
              command = inByte;
          }
        } else  if(inByte == ')') {
          //Print the command (or do other things)
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
          receivingCommand = false;
          receivingArg = false;
          command = '\0';
          for(uint8_t n = 0; n < MAX_ARG_LENGTH; n++) {
            arg[n] = '\0';
          }
          argPos = 0;
          PORTB |= (1 << PB1);  /////////////////////////////////////////////////////////////////////////////Debugging line, comment out after debugging.///////////////////////////////////////////////////////////////////////////
        }
      }
      PORTB &= ~(1 << PB0);  /////////////////////////////////////////////////////////////////////////////Debugging line, comment out after debugging.///////////////////////////////////////////////////////////////////////////
    }
  }
}
