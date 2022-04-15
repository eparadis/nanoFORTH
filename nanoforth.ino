/**
 *  @file examples/0_blink.ino
 *  @brief nanoFORTH example - Blink pin 13
 *
 *  Our first Sketch - demostrates nanoFORTH support multi-tasking
 *  + a user task that blinks built-in pin 13
 *  + nanoFORTH itself runs in parallel
 *
 *  open Serial Monitor (or your favorate terminal emulator) as the console input to nanoFORTH
 *  + baud rate set to 115200
 *  + line ending set to Both NL & CR (if using emulator, set Add CR on, ECHO on)
 *
 *  Once compiled/uploaded, you should see
 *  + some nanoFORTH init system info
 *  + ok prompt
 *  + try type WRD and hit return on the input above
 */
#include <Arduino.h>
#include <AceTMI.h> // SimpleTmi1638Interface
#include <AceSegment.h> // Tm1638Module
#include <AceSegmentWriter.h> // NumberWriter

using ace_tmi::SimpleTmi1638Interface;
using ace_segment::LedModule;
using ace_segment::Tm1638AnodeModule;
using ace_segment::NumberWriter;

// Replace these with the PIN numbers of your dev board.
// The TM1638 protocol is very similar to SPI, so I often use the SPI pins.
const uint8_t CLK_PIN = 0;
const uint8_t DIO_PIN = 1;
const uint8_t STB_PIN = 2;
const uint8_t NUM_DIGITS = 8;

// My TM1638 LED module contains no filtering capacitor on the DIO, CLK, and STB
// lines. So it should support the highest clock frequency using a 1 micro
// transition delay.
const uint8_t DELAY_MICROS = 1;

using TmiInterface = SimpleTmi1638Interface;
TmiInterface tmiInterface(DIO_PIN, CLK_PIN, STB_PIN, DELAY_MICROS);
Tm1638AnodeModule<TmiInterface, NUM_DIGITS> ledModule(tmiInterface);

NumberWriter<LedModule> numberWriter(ledModule);

#include "src/nanoforth.h"

//N4_TASK(blink)                    ///< create blinking task (i.e. built-in LED on pin 13)
//{
//    digitalWrite(LED_BUILTIN, HIGH);
//    N4_DELAY(500);
//    digitalWrite(LED_BUILTIN, LOW);
//    N4_DELAY(500);
//}
//N4_END;

N4_TASK(display_count)
{
  numberWriter.writeUnsignedDecimalAt(0, millis()/1000, 8);
  ledModule.flush();
  N4_DELAY(1000);
}
N4_END;

NanoForth n4;                     ///< create NanoForth instance
void setup()
{
    tmiInterface.begin();
    ledModule.begin();
  
    Serial.begin(115200);         ///< init Serial IO, make sure it is set to 'Both BL & CR' to capture input
    if (n4.begin()) {             /// default: (Serial,0x480,0x80), try reducing if memory is constrained
        Serial.print(F("ERROR: memory allocation failed!"));
    }

    numberWriter.writeHexChars2At(3, 0xE, 0xD);
    ledModule.setBrightness(2);
    ledModule.flush();
    delay(500);

    n4.add(display_count);
//    pinMode(LED_BUILTIN, OUTPUT);
}

void loop()
{
    n4.exec();                   // execute one nanoForth VM cycle
}
