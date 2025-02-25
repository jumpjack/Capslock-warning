# Capslock-warning
Why in the world is capslock led so tiny?!? Here is a minimal project to get a capslock-notifier as big as you want!

## Without capslock-warning

![image](https://github.com/user-attachments/assets/2d9db0d6-b711-4153-be24-ce27355a031e)

## With capslock warning :-)

![image](https://github.com/user-attachments/assets/118a62d6-a1f4-4f2f-bea3-c8a582432003)

You can use any container you like for the caps-lock led:

![image](https://github.com/user-attachments/assets/5a2deb35-899d-4497-bd71-f3c26ec3e161)

![image](https://github.com/user-attachments/assets/597c4fa3-fd73-468c-bb48-697386a2875a)

![image](https://github.com/user-attachments/assets/7ea8f7a5-9cee-4d53-8fc0-2f9bbdf81aab)

Just puth the led inside it and connect it to pin 17 of [Arduino Micro](https://docs.arduino.cc/hardware/micro/) (or any Arduino equipped with ATmega32u4)

# The code

```
#include "HID-Project.h"

const int pinLed = 17;
const int pinButton = 2;

void setup() {
  pinMode(pinLed, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(pinButton, INPUT_PULLUP);

  // Sends a clean report to the host. This is important on any Arduino type.
  BootKeyboard.begin();
}


void loop() {
  digitalWrite(pinLed, HIGH);
  TXLED1;
  if (BootKeyboard.getLeds() & LED_CAPS_LOCK) {
    digitalWrite(3, HIGH);
  } else {
    digitalWrite(3, LOW);
  }
  // Trigger caps lock manually via button
  if (!digitalRead(pinButton)) {
    BootKeyboard.write(KEY_CAPS_LOCK);

    // Simple debounce
    delay(300);
  }
}
```

That's all.

# Dependencies

This project relies on  [HID](https://github.com/NicoHood/HID) library by NicoHood, you need to install it in [Arduino IDE](https://www.arduino.cc/en/software) (minimum 1.6.7) before compiling the project.

# The hardware

 Recommended is [Arduino Pro Micro](https://docs.arduino.cc/hardware/micro/) due to very small form factor. Other devices compatible with HIDlibrary:

## As-Is
- Leonardo
- (Pro)Micro
- Any other 8u2/16u2/at90usb8/162/32u2/32u4 compatible board
- Zero
- MKR1000

## With some hacking:
   - Uno (requires HoodLoader2)
   - Mega (requires HoodLoader2)
 


# How it works

...



