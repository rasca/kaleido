#include <Arduino.h>
#include <Framework.h>
#include <Kaleido.h>

BigKaleido kaleido;
Framework<1886U> framework(kaleido);

void setup() {
  Serial.begin(115200);
  FastLED.setBrightness(20);

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
