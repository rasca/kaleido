#include <Arduino.h>
#include <Framework.h>
#include <Kaleido.h>

BigKaleido kaleido;
Framework<1886U> framework(kaleido);

void setup() {
  // Serial.begin(9600);

  framework.initialize();
  // leds.setup();
}

unsigned long start = 0;

void loop() {

  if (millis() - start > 20) {
    start = millis();
    framework.tick();
  }

}
