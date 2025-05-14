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
```

That's all.

(See [reference](https://github.com/NicoHood/HID/blob/master/examples/Keyboard/KeyboardLed/KeyboardLed.ino) code)

# Dependencies

This project relies on  [HID](https://github.com/NicoHood/HID) library by NicoHood, you need to install it in [Arduino IDE](https://www.arduino.cc/en/software) (minimum 1.6.7) before compiling the project.

# The hardware

![image](https://github.com/user-attachments/assets/5fdf49f0-f758-4637-9025-b42b07098e8b)

## Tested

- [Arduino Pro Micro](https://docs.arduino.cc/hardware/micro/) due to very small form factor. Other devices compatible with HIDlibrary.
- [RP2040-zero](https://www.amazon.it/gp/product/B0D869F8QG/) (Raspberry PI) (9,00) ([How to program it with Arduino IDE](https://www.instructables.com/Programmare-RASPBERRY-PI-PICO-Con-ARDUINO-IDE-Come/))
- [ESP32-S2-mini](https://www.amazon.it/gp/product/B0BGPD6CV5/ref=ox_sc_act_title_4?smid=A3SCFTIO8CSK1X&psc=1) (8,00 euro)

## As-Is
- Leonardo
- Any other 8u2/16u2/at90usb8/162/32u2/32u4 compatible board
- Zero
- MKR1000

## With some hacking:
   - Uno (requires HoodLoader2)
   - Mega (requires HoodLoader2)

## To be tested

   - [AtTiny85](https://www.amazon.it/gp/product/B07MB8Y8SF/) (9,00 euro)
   - [RP2040-one](https://www.amazon.it/Waveshare-RP2040-One-Raspberry-Microcontroller-MicroPython/dp/B0BLC1HJND/)  (Raspberry PI) (12,00, no USB cable) ([How to program it with Arduino IDE](https://www.instructables.com/Programmare-RASPBERRY-PI-PICO-Con-ARDUINO-IDE-Come/))
   - [ESP32-S3-mini](https://www.amazon.it/Waveshare-ESP32-S3-Development-Board-ESP32-S3FH4R2/dp/B0CJJPJZ2H/) (12,00 euro)


# How it works

Keyboard is not just an input device, it's also an output device! You can verify it by yourself: if you connect two keyboards to same PC, upon enabling CAPSLOCK on one keyboard, the CAPSLOCK on the other keyboard will light up!

This sketch enables the Arduino board to behave like a keyboard, receiving input from the "actual" keyboard connected to the PC: upon detecting CAPSLOCK press, it lights up a big led which you can't miss.

# Serial communication

Some of the  sketches support setting up led color using command R,G,B from serial (ex: 0,0,255 will set led to blue). Usa free+portable [Tera Term for Windows](https://sourceforge.net/projects/tera-term/).


