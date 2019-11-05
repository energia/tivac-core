/*
 * EEPROM Clear
 *
 * Sets all of the bytes of the EEPROM to 0.
 * Please see eeprom_iteration for a more in depth
 * look at how to traverse the EEPROM.
 *
 * This example code is in the public domain.
 */

#include <EEPROM.h>

#define LED RED_LED
void setup() {
  // initialize the LED pin as an output.
  pinMode(LED, OUTPUT);
  
  /***
    Iterate through each byte of the EEPROM storage.
    Tiva C series has a ROM of 2k bytes.
    Rather than hard-coding the length, you should use the pre-provided length function.
  ***/

  for (int i = 0 ; i < EEPROM.length() ; i++) {
    EEPROM.write(i, 0);
  }

  // turn the LED on when we're done
  digitalWrite(LED, HIGH);
}

void loop() {
  /** Empty loop. **/
}
