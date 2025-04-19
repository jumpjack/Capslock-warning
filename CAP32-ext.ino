// With configurable external RGB led.
//  Press button 0 followed by reset to enable the device to capslock monitoring
//
//

#ifndef ARDUINO_USB_MODE
#error This ESP32 SoC has no Native USB interface
#elif ARDUINO_USB_MODE == 1
#warning This sketch should be used when USB is in OTG mode
void setup() {}
void loop() {}
#else
#include "USB.h"
#include "USBHIDKeyboard.h"
#include <Preferences.h>  // Aggiunto per la memoria persistente

#if !ARDUINO_USB_CDC_ON_BOOT
USBCDC USBSerial;
#endif

USBHID HID;
USBHIDKeyboard Keyboard;

bool capsLockState = false;  // Variabile per tracciare lo stato del Caps Lock


#define RGB_R 12
#define RGB_G 13
#define RGB_B 14

// Struttura per i colori arcobaleno
struct RainbowColor {
  const char* name;
  uint8_t r;
  uint8_t g;
  uint8_t b;
};

// Colore predefinito (rosso)
uint8_t capsR = 255;
uint8_t capsG = 0;
uint8_t capsB = 0;

bool ledOn = false;

// Mappa dei colori dell'arcobaleno
const RainbowColor rainbowColors[] = {
  {"red", 255, 0, 0},
  {"orange", 255, 165, 0},
  {"yellow", 255, 255, 0},
  {"green", 0, 255, 0},
  {"blue", 0, 0, 255},
  {"indigo", 75, 0, 130},
  {"violet", 238, 130, 238}
};
const int rainbowColorsCount = 7;

Preferences preferences;  // Oggetto per gestire le preferenze

// Imposta il colore RGB
void setRGBColor(uint8_t r, uint8_t g, uint8_t b) {
  analogWrite(RGB_R, r);
  analogWrite(RGB_G, g);
  analogWrite(RGB_B, b);
}

// Cerca un colore arcobaleno per nome
bool findRainbowColor(const String& name, uint8_t &r, uint8_t &g, uint8_t &b) {
  for (int i = 0; i < rainbowColorsCount; i++) {
    if (name.equalsIgnoreCase(rainbowColors[i].name)) {
      r = rainbowColors[i].r;
      g = rainbowColors[i].g;
      b = rainbowColors[i].b;
      return true;
    }
  }
  return false;
}

// Salva i colori nella memoria persistente
void saveColorsToPreferences() {
  preferences.begin("rgb_settings", false); // false per modalità lettura/scrittura
  preferences.putUChar("capsR", capsR);
  preferences.putUChar("capsG", capsG);
  preferences.putUChar("capsB", capsB);
  preferences.end();
  updateLEDs();
  Serial.println("Colori salvati nella memoria persistente");
}

// Carica i colori dalla memoria persistente
void loadColorsFromPreferences() {
  preferences.begin("rgb_settings", true); // true per modalità sola lettura
  capsR = preferences.getUChar("capsR", 255); // Valore di default 255 (rosso) se non trovato
  capsG = preferences.getUChar("capsG", 0);   // Valore di default 0
  capsB = preferences.getUChar("capsB", 0);   // Valore di default 0
  preferences.end();
  Serial.printf("Colori caricati dalla memoria: R=%d, G=%d, B=%d\n", capsR, capsG, capsB);
}

// Processa i comandi dalla seriale
void processSerialCommand(const String& command) {
  // Comando RGB: r,g,b
  if (command.indexOf(',') != -1) {
    int firstComma = command.indexOf(',');
    int secondComma = command.indexOf(',', firstComma + 1);
    
    if (firstComma != -1 && secondComma != -1) {
      capsR = command.substring(0, firstComma).toInt();
      capsG = command.substring(firstComma + 1, secondComma).toInt();
      capsB = command.substring(secondComma + 1).toInt();
      Serial.printf("Nuovo colore: R=%d, G=%d, B=%d\n", capsR, capsG, capsB);
      saveColorsToPreferences(); // Salva i nuovi colori
      return;
    }
  }
  
  // Comando nome colore
  uint8_t r, g, b;
  if (findRainbowColor(command, r, g, b)) {
    capsR = r;
    capsG = g;
    capsB = b;
    Serial.printf("Colore %s impostato: R=%d, G=%d, B=%d\n", command.c_str(), capsR, capsG, capsB);
    saveColorsToPreferences(); // Salva i nuovi colori
    return;
  }
  
  Serial.println("Comando non riconosciuto. Usa: r,g,b o nome colore (red, orange, yellow, green, blue, indigo, violet)");
  setRGBColor(0,255,255);
  delay(100);
  setRGBColor(255,255,255);
  delay(100);
  setRGBColor(0,255,255);
  delay(100);
  setRGBColor(255,255,255);
  delay(100);
  setRGBColor(0,255,255);
  delay(100);
  setRGBColor(255,255,255);
  delay(100);
}

// Callback per eventi USB
static void usbEventCallback(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data) {
Serial.println("Evento");
  if (event_base == ARDUINO_USB_HID_KEYBOARD_EVENTS) {
Serial.println("   event_base = Evento ARDUINO_USB_HID_KEYBOARD_EVENTS");
    arduino_usb_hid_keyboard_event_data_t *data = (arduino_usb_hid_keyboard_event_data_t *)event_data;
    if (event_id == ARDUINO_USB_HID_KEYBOARD_LED_EVENT) {
Serial.println("         event_id == ARDUINO_USB_HID_KEYBOARD_LED_EVENT");
     capsLockState = data->capslock;  // Aggiorna lo stato del Caps Lock
      updateLEDs();  // Aggiorna entrambi i LED
    }
  }
}

void updateLEDs() {
  if (capsLockState) {
    digitalWrite(LED_BUILTIN, HIGH);
    setRGBColor(255-capsR, 255-capsG, 255-capsB);  // Accendi il LED esterno con il colore selezionato
  } else {
    digitalWrite(LED_BUILTIN, LOW);
    setRGBColor(255,255,255);  // Spegni il LED esterno
  }
}

String serialInput = "";

void setup() {
  Serial.begin(115200);
  Serial.setDebugOutput(true);

  HID.onEvent(usbEventCallback);
  Keyboard.onEvent(usbEventCallback);

  USBSerial.begin();
  Keyboard.begin();
  USB.begin();

  pinMode(LED_BUILTIN, OUTPUT);
  
  // Configurazione PWM con ledcAttach (per Arduino ESP32 3.0+)
  ledcAttach(RGB_R, 5000, 8);  // Frequenza 5kHz, risoluzione 8-bit
  ledcAttach(RGB_G, 5000, 8);
  ledcAttach(RGB_B, 5000, 8);

  // Carica i colori salvati
  loadColorsFromPreferences();

  Serial.println("Sketch avviato. Invia:");
  Serial.println("- r,g,b (es. 255,100,0)");
  Serial.println("- nome colore (es. blue)");
  Serial.printf("Colore corrente: R=%d, G=%d, B=%d\n", capsR, capsG, capsB);
}

void loop() {
  while (Serial.available()) {
    char c = Serial.read();
    if (c == '\n') {
      processSerialCommand(serialInput);
      serialInput = "";
    } else if (c != '\r') {
      serialInput += c;
    }
  }
  if (ledOn) {
    setRGBColor(255-capsR, 255-capsG, 255-capsB);
  }
}

#endif /* ARDUINO_USB_MODE */
