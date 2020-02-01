#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

#define neopixelPIN 6
#define tiltPIN 2

// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(24, neopixelPIN, NEO_GRB + NEO_KHZ800);

// IMPORTANT: To reduce NeoPixel burnout risk, add 1000 uF capacitor across
// pixel power leads, add 300 - 500 Ohm resistor on first pixel's data input
// and minimize distance between Arduino and first pixel.  Avoid connecting
// on a live circuit...if you must, connect GND first.

bool timerSet;
int  timerMinutes;
long timerMillis;
int  timerPreq;

void setup() {
  Serial.begin (9600);

  pinMode(tiltPIN, INPUT_PULLUP);

  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
}

void loop() {

  uint16_t i, j;

  for(j=0; j<128; j++) {
    for(i=0; i<strip.numPixels(); i++) {
      if(timerSet) {
        strip.setPixelColor(i, Wheel((i+j) & 255));
      } else {
        strip.setPixelColor(i, strip.Color((i+j) / 2 + 30, (i+j) / 6 + 10, 1));
        //strip.setPixelColor(i, strip.Color(128, 40, 1)); // Warm white
      }
    }
    strip.setPixelColor(0, strip.Color(0, 128,255));
    strip.setPixelColor(3, strip.Color(52, 128, 0));
    strip.show();

    delay(70);
  }
  for(j=128; j>0; j--) {
    for(i=0; i<strip.numPixels(); i++) {
      if(timerSet) {
        strip.setPixelColor(i, Wheel((i+j) & 255));
      } else {
        strip.setPixelColor(i, strip.Color((i+j) / 2 + 30, (i+j) / 6 + 10, 1));
      }
    }
    strip.setPixelColor(0, strip.Color(0, 128,255));
    strip.setPixelColor(3, strip.Color(52, 128, 0));
    strip.show();

    delay(70);
  }  

}

void checkTiltTimer() {
  
  if(digitalRead(tiltPIN)) {

    // Increment timer while tilted
    while(digitalRead(tiltPIN)) {
      // Require 1 sec of tilt to set timer amount
      if(timerPreq > 1) {
        
        // Set new timer if not set
        if(!timerSet) {
          timerMinutes = 0;
          timerMillis = millis();
          timerSet = true;
        }
        
        clearStrip();
        strip.setPixelColor(timerMinutes % strip.numPixels(), strip.Color(255, 0, 0));
        strip.show();
  
        timerMinutes++;
        timerMillis += 60000;
        Serial.println("Timer set to " + String(timerMinutes) + " minutes");
      }
      timerPreq++;
      
      delay(500);
    }
  } else {
    timerPreq = 0;
  }

  // Alert when timer is finished
  if(timerSet && millis() > timerMillis) {
    while(!digitalRead(tiltPIN)) {
      theaterChase(strip.Color(127, 127, 127), 20);
    }
    timerSet = false;
    Serial.println("Timer turned off");
    
    clearStrip();
    while(digitalRead(tiltPIN)); // Wait until set upright again
  }
}

void clearStrip() {
    for(uint16_t i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, strip.Color(0, 0, 0));
    }
    strip.show();
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return strip.Color(128 - WheelPos , 0, WheelPos );
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(WheelPos , 128 - WheelPos , 0);
  }
  WheelPos -= 170;
  return strip.Color(0, WheelPos , 128 - WheelPos );
}


void theaterChase(uint32_t c, uint8_t wait) {
  for (int j=0; j<10; j++) {  //do 10 cycles of chasing
    for (int q=0; q < 3; q++) {
      for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, c);    //turn every third pixel on
      }
      strip.show();

      delay(wait);

      for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
  // Blink whole strip
  clearStrip();
  delay(300);  
}

