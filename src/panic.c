
#include <Arduino.h>

#define LED 13

void LED_On() { digitalWrite(LED, HIGH); }
void LED_Off() { digitalWrite(LED, LOW); }

void panic(uint16_t code) {

  while (1) {
    
    uint16_t c;
    for (c = 3; c; c--) {
      LED_On();
      delay(150);
      LED_Off();
      delay(100);
    }
    
    delay(100);
    
    for (c = 3; c; c--) {
      LED_On();
      delay(400);
      LED_Off();
      delay(100);
    }
    
    delay(100);
  
    for (c = 3; c; c--) {
      LED_On();
      delay(150);
      LED_Off();
      delay(100);
    }
  
    // pause
    delay(900);
  
    // play code
    for (c = code; c; c--) {
      LED_On();
      delay(300);
      LED_Off();
      delay(300);
    }
    
    // pause
    delay(800);
  }
}  

