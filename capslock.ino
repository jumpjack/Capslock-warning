#include "HID-Project.h" // Does not work on ESP32, look at specific sketch

const int statusLed = 17; // "alive" status LED
const int warningLed = 3;

void setup() {
  pinMode(statusLed, OUTPUT);
  pinMode(warningLed, OUTPUT);

  // Sends a clean report to the host. This is important on any Arduino type.
  BootKeyboard.begin();
}


void loop() {
  digitalWrite(statusLed, HIGH); // Inform user that loop is running
  if (BootKeyboard.getLeds() & LED_CAPS_LOCK) {
    digitalWrite(warningLed, HIGH);
  } else {
    digitalWrite(warningLed, LOW);
  }
}
