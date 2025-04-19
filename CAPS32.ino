/////////////////////////////////////////
// Turns on the onboard led upon detecting CAPS-LOCK on.
// Works only on ESP32-S2 and -S3, because they are the only
// ESP32 variant supporting USB-OTG.
/////////////////////////////////////////
//
// You must put the ESP32-S2 into USB-OTG mode before uploading this sketch:
// - Connect USB cable
// - Keep button 0 pressed
// - Press reset button
// - Release reset button
// - Release button 0
// Now you should be in UBS-OTG mode, able to upload this skecth.
// To get back into standard mode, just press and release reset button:
// if ESP32 is in standard mode, you cannot upload this sketch, but
// you can successfully upload BLINK sketch.



#ifndef ARDUINO_USB_MODE
#error This ESP32 SoC has no Native USB interface
#elif ARDUINO_USB_MODE == 1
#warning This sketch should be used when USB is in OTG mode
void setup() {}
void loop() {}
#else
#include "USB.h"
//#include "USBHIDMouse.h"
#include "USBHIDKeyboard.h"

#if !ARDUINO_USB_CDC_ON_BOOT
USBCDC USBSerial;
#endif

USBHID HID;
USBHIDKeyboard Keyboard;

static void usbEventCallback(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data) {
  if (event_base == ARDUINO_USB_HID_KEYBOARD_EVENTS) {
    arduino_usb_hid_keyboard_event_data_t *data = (arduino_usb_hid_keyboard_event_data_t *)event_data;
    switch (event_id) {
      case ARDUINO_USB_HID_KEYBOARD_LED_EVENT:
        Serial.printf("HID KEYBOARD LED: NumLock:%u, CapsLock:%u, ScrollLock:%u\n", data->numlock, data->capslock, data->scrolllock);
        digitalWrite(LED_BUILTIN,  data->capslock);
        break;

      default: break;
    }
  } else {
    //
  }
}

void setup() {
  Serial.begin(115200);
  Serial.setDebugOutput(true);

  HID.onEvent(usbEventCallback);
  Keyboard.onEvent(usbEventCallback);

  USBSerial.begin();
  Keyboard.begin();
  USB.begin();

  pinMode(LED_BUILTIN, OUTPUT);  
}

void loop() {
  // who needs a loop?!?
}
#endif /* ARDUINO_USB_MODE */
