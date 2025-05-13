// Tested on Wavehsare RP2040 Pico Zero

#include <Keyboard.h>
#include <Adafruit_NeoPixel.h>

#define PIXEL_PIN 16
#define NUM_PIXELS 1

Adafruit_NeoPixel pixel(NUM_PIXELS, PIXEL_PIN, NEO_GRB + NEO_KHZ800);

// Variabili per il colore Caps Lock
uint8_t capsR = 255;  // Rosso di default
uint8_t capsG = 0;
uint8_t capsB = 0;

void handleSerialInput();
void processRGB(String input);
void printHelp();

// Callback per i LED della tastiera
void ledCallback(bool numLock, bool capsLock, bool scrollLock, bool compose, bool kana, void* data) {
  (void)numLock; (void)scrollLock; (void)compose; (void)kana; (void)data;
  
  pixel.setPixelColor(0, capsLock ? pixel.Color(capsR, capsG, capsB) : 0);
  pixel.show();
  Serial.print("Caps Lock: ");
  Serial.println(capsLock ? "ON" : "OFF");
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
  
  printHelp();
  Serial.println("Sistema pronto. Invia comandi RGB via seriale.");
}

void loop() {
  if (Serial.available() > 0) {
    handleSerialInput();
  }
  delay(10);
}

void printHelp() {
  Serial.println("\nComandi disponibili:");
  Serial.println("OFF - Spegne il LED");
  Serial.println("R,G,B - Imposta colore (es. '255,0,0' per rosso)");
  Serial.println("HELP - Mostra questo messaggio");
  Serial.printf("Colore attuale Caps Lock: R=%d, G=%d, B=%d\n", capsR, capsG, capsB);
}

void handleSerialInput() {
  String input = Serial.readStringUntil('\n');
  input.trim();
  
  if (input.equalsIgnoreCase("OFF")) {
    pixel.setPixelColor(0, 0);
    pixel.show();
    Serial.println("LED spento");
  }
  else if (input.equalsIgnoreCase("HELP")) {
    printHelp();
  }
  else if (input.indexOf(',') != -1) {
    processRGB(input);
  }
  else {
    Serial.println("Comando non riconosciuto. Invia 'HELP' per assistenza.");
  }
}

void processRGB(String input) {
  int firstComma = input.indexOf(',');
  int lastComma = input.lastIndexOf(',');
  
  if (firstComma != -1 && lastComma != -1 && firstComma != lastComma) {
    capsR = input.substring(0, firstComma).toInt();
    capsG = input.substring(firstComma + 1, lastComma).toInt();
    capsB = input.substring(lastComma + 1).toInt();
    
    // Verifica i valori
    capsR = constrain(capsR, 0, 255);
    capsG = constrain(capsG, 0, 255);
    capsB = constrain(capsB, 0, 255);
    
    Serial.print("Nuovo colore Caps Lock impostato: ");
    Serial.print(capsR); Serial.print(", ");
    Serial.print(capsG); Serial.print(", ");
    Serial.println(capsB);
    
    // Aggiorna immediatamente il LED se Caps Lock è attivo
    if (pixel.getPixelColor(0)) {
      pixel.setPixelColor(0, pixel.Color(capsR, capsG, capsB));
      pixel.show();
    }
  } else {
    Serial.println("Formato non valido. Usa R,G,B (es. '255,0,0')");
  }
}
