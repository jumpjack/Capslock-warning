// Version for WaveShare RP2040 zero (Raspberry PI)
// External RGB LED support.

#include <Keyboard.h>
#include <Adafruit_NeoPixel.h>

#define PIXEL_PIN 16
#define NUM_PIXELS 1
#define PIN_R 13
#define PIN_G 11
#define PIN_B 10
#define PIN_COMMON 12

// For unbalanced RGB leds (code not working!)
#define  HIGH_RED 255
#define  HIGH_GREEN 255
#define  HIGH_BLUE 255

#define COMMON_VALUE HIGH
int  RED_ON = (COMMON_VALUE == LOW) ? HIGH_RED : LOW;
int  RED_OFF = (COMMON_VALUE == LOW) ? LOW : HIGH_RED;
int  GREEN_ON = (COMMON_VALUE == LOW) ? HIGH_GREEN : LOW;
int  GREEN_OFF = (COMMON_VALUE == LOW) ? LOW : HIGH_GREEN;
int  BLUE_ON = (COMMON_VALUE == LOW) ? HIGH_BLUE : LOW;
int  BLUE_OFF = (COMMON_VALUE == LOW) ? LOW : HIGH_BLUE;

Adafruit_NeoPixel pixel(NUM_PIXELS, PIXEL_PIN, NEO_GRB + NEO_KHZ800);

// Variabili per il colore Caps Lock
uint8_t capsR = HIGH;  // Rosso di default
uint8_t capsG = 0;
uint8_t capsB = 0;

uint8_t capsR_LED = (COMMON_VALUE == LOW) ? RED_ON : LOW; 
uint8_t capsG_LED = 0;
uint8_t capsB_LED = 0;



void handleSerialInput();
void processRGB(String input);
void printHelp();

// Callback per i LED della tastiera
void ledCallback(bool numLock, bool capsLock, bool scrollLock, bool compose, bool kana, void* data) {
  (void)numLock; (void)scrollLock; (void)compose; (void)kana; (void)data;
  
  pixel.setPixelColor(0, capsLock ? pixel.Color(capsR, capsG, capsB) : 0);
  if (capsLock) {
    analogWrite(PIN_R,capsR_LED);
    analogWrite(PIN_G,capsG_LED);
    analogWrite(PIN_B,capsB_LED);
  } else {
    analogWrite(PIN_R,RED_OFF);
    analogWrite(PIN_G,GREEN_OFF);
    analogWrite(PIN_B,BLUE_OFF);    
  }
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

  pinMode(PIN_R,OUTPUT);
  pinMode(PIN_G,OUTPUT);
  pinMode(PIN_B,OUTPUT);
  pinMode(PIN_COMMON,OUTPUT);


  digitalWrite(PIN_COMMON,COMMON_VALUE); 

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
    analogWrite(PIN_R,RED_OFF);
    analogWrite(PIN_G,GREEN_OFF);
    analogWrite(PIN_B,BLUE_OFF);    
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
    
    if (COMMON_VALUE == LOW)  {
      capsR_LED = capsR;
      capsG_LED = capsG;
      capsB_LED = capsB;
    } else {
      capsR_LED = HIGH_RED - capsR;
      capsG_LED = HIGH_GREEN - capsG;
      capsB_LED = HIGH_BLUE - capsB;
    }

    /*capsR_LED = constrain(capsR_LED, 0,HIGH_RED);
    capsG_LED = constrain(capsG_LED, 0,HIGH_GREEN);
    capsB_LED = constrain(capsB_LED, 0,HIGH_BLUE);*/

    Serial.print("Nuovo colore Caps Lock impostato: ");
    Serial.print(capsR); Serial.print(", ");
    Serial.print(capsG); Serial.print(", ");
    Serial.println(capsB);

   Serial.print("LED impostato: ");
    Serial.print(capsR_LED); Serial.print(", ");
    Serial.print(capsG_LED); Serial.print(", ");
    Serial.println(capsB_LED);
    
    // Aggiorna immediatamente il LED se Caps Lock è attivo
    if (pixel.getPixelColor(0)) {
      pixel.setPixelColor(0, pixel.Color(capsR, capsG, capsB));
      analogWrite(PIN_R,capsR_LED);
      analogWrite(PIN_G,capsG_LED);
      analogWrite(PIN_B,capsB_LED);      
      pixel.show();
    }
  } else {
    Serial.println("Formato non valido. Usa R,G,B (es. '255,0,0')");
  }
}
