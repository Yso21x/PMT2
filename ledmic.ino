#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif
#define PIN 16
#define PIXELCOUNT 90
#define microphonePin 15
int state = 0;
Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXELCOUNT, PIN, NEO_RGB + NEO_KHZ800);
void setup() {
  strip.begin();
  strip.setBrightness(50);             // set the maximum LED intensity down to 50
  strip.show();                        // Initialize all pixels to 'off'
  delay(2000);                         // wait two seconds                      // send the new color settings to the stripe
  pinMode(microphonePin, INPUT); 
}
void loop() {
    state = digitalRead(microphonePin);

  if (state == HIGH) {
    for (int i=0; i<PIXELCOUNT; i++){
    strip.setPixelColor(i, 255,0,0); 
    }
  strip.show();
    delay(1000);
  }

  else {
    for (int i=0; i<PIXELCOUNT; i++){
    strip.setPixelColor(i, 0,0,255); 
    }
  strip.show();
  }

}
