// Version for WaveShare RP2040 zero (Raspberry PI)
// External RGB LED support.
// Added support for persistant storage of color.
// Separated LEDs version: capslock = red, scroll-lock = green, numlock = blue; keys combination lead to colors combination
// Implemented calibration level for RGB leds with unbalanced channels.


#include <Keyboard.h>
#include <Adafruit_NeoPixel.h>

#define PIXEL_PIN 16
#define NUM_PIXELS 1
#define PIN_R 13
#define PIN_G 11
#define PIN_B 10
#define PIN_COMMON 12

// Valori massimi per i LED
#define HIGH_RED 255
#define HIGH_GREEN 255
#define HIGH_BLUE 255

const uint8_t R_CALIB = 145;   
const uint8_t G_CALIB = 255;    
const uint8_t B_CALIB = 255;   

#define COMMON_VALUE HIGH

// Calcola i valori ON/OFF in base al tipo di LED (common anode o cathode)
int RED_ON = (COMMON_VALUE == LOW) ? R_CALIB : 255-R_CALIB;
int RED_OFF = (COMMON_VALUE == LOW) ? 0 : 255;

int GREEN_ON = (COMMON_VALUE == LOW) ? G_CALIB : 255-G_CALIB;
int GREEN_OFF = (COMMON_VALUE == LOW) ? 0 : 255;

int BLUE_ON = (COMMON_VALUE == LOW) ? B_CALIB : 255-B_CALIB;
int BLUE_OFF = (COMMON_VALUE == LOW) ? 0 : 255;

Adafruit_NeoPixel pixel(NUM_PIXELS, PIXEL_PIN, NEO_RGB + NEO_KHZ800);

// Colori base bilanciati per ciascun tasto
#define CAPS_RED 255  
#define CAPS_GREEN 0
#define CAPS_BLUE 0

#define SCROLL_RED 0    
#define SCROLL_GREEN 255
#define SCROLL_BLUE 0

#define NUM_RED 0      
#define NUM_GREEN 0
#define NUM_BLUE 255

void ledCallback(bool numLock, bool capsLock, bool scrollLock, bool compose, bool kana, void* data) {
  (void)compose; (void)kana; (void)data;

  uint8_t r = 0, g = 0, b = 0;
  uint8_t rLed = RED_OFF, gLed = GREEN_OFF, bLed = BLUE_OFF;

  // Calcola il colore combinato
  if (capsLock) {
    r += CAPS_RED;
    g += CAPS_GREEN;
    b += CAPS_BLUE;
    rLed = RED_ON;
  }
  if (!numLock) {  // <-- INVERTIAMO la logica del NumLock qui
    r += NUM_RED;
    g += NUM_GREEN;
    b += NUM_BLUE;
    bLed = BLUE_ON;
  }
  if (scrollLock) {
    r += SCROLL_RED;
    g += SCROLL_GREEN;
    b += SCROLL_BLUE;
    gLed = GREEN_ON;
  }

  // Applica il colore al NeoPixel
  pixel.setPixelColor(0, pixel.Color(
    constrain(r, 0, 255),
    constrain(g, 0, 255),
    constrain(b, 0, 255)
  ));

  // Gestione dei LED separati (se presenti)
  analogWrite(PIN_R, r > 0 ? rLed : RED_OFF);
  analogWrite(PIN_G, g > 0 ? gLed : GREEN_OFF);
  analogWrite(PIN_B, b > 0 ? bLed : BLUE_OFF);

  pixel.show();

  // Debug su seriale
  Serial.print("LED Status - ");
  Serial.print("NumLock: ");
  Serial.print(numLock ? "ON" : "OFF");
  Serial.print(" (LED ");
  Serial.print(numLock ? "OFF" : "ON");  // <-- Mostriamo lo stato invertito
  Serial.print("), CapsLock: ");
  Serial.print(capsLock ? "ON" : "OFF");
  Serial.print(", ScrollLock: ");
  Serial.print(scrollLock ? "ON" : "OFF");
  Serial.print(" | Color: R=");
  Serial.print(r);
  Serial.print(" G=");
  Serial.print(g);
  Serial.print(" B=");
  Serial.println(b);
}




void setup() {
  Serial.begin(115200);
  pixel.begin();
  pixel.setBrightness(255);
  pixel.show();

  // Configura il callback per i LED
  Keyboard._ledCB = ledCallback;
  Keyboard._ledCBdata = nullptr;

  Keyboard.begin();

  delay(3000);

  Serial.println("Sistema pronto. Combinazione colori:");
  Serial.println("Caps Lock: Rosso");
  Serial.println("Num Lock: Blu (comportamento INVERTITO)");
  Serial.println("Scroll Lock: Verde");
  Serial.println("Combinazioni:");
  Serial.println("- Caps+Num: Viola (rosso+blu)");
  Serial.println("- Caps+Scroll: Giallo (rosso+verde)");
  Serial.println("- Num+Scroll: Ciano (blu+verde)");
  Serial.println("- Tutti e 3: Bianco");

  pinMode(PIN_R, OUTPUT);
  pinMode(PIN_G, OUTPUT);
  pinMode(PIN_B, OUTPUT);
  pinMode(PIN_COMMON, OUTPUT);

  digitalWrite(PIN_COMMON, COMMON_VALUE);

}

void loop() {
  delay(10);
}
