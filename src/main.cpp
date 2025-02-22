#define FASTLED_INTERNAL  // remove FastLED build warning
#include <Arduino.h>
#include <Framework.h>
#include <Iratorio.h>

Iratorio<1886U> iratorio;
Framework<1886U> framework(iratorio);

void setup() {
  Serial.begin(115200);
  FastLED.setBrightness(150);

  framework.initialize();
  // framework.setupVirtualSegmentsLength(kaleido.dots_lines);
}

unsigned long start = 0;

void loop() {


  // if (millis() - start > 2000) {
  //   start = millis();
  //   unsigned long duration = micros();
  //   Serial.println(micros() - duration);
  // }
  framework.tick();


}
