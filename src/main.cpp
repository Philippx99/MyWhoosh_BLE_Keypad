// ESP32 BLE Tastatur-Emulator
#include <ArduinoJson.h>
#include <FS.h>
#include <LittleFS.h>
#include <BleKeyboard.h>


enum ButtonState { BTN_IDLE, BTN_DEBOUNCE, BTN_PRESSED, BTN_WAIT_DOUBLE, BTN_LONG, BTN_RELEASED };
struct ButtonConfig {
  int pin;
  char key_normal;
  char key_double;
  char key_long;
  String mode;
  int debounce;
  int lastState;
  unsigned long lastChange;
  unsigned long pressStart;
  unsigned long lastRelease;
  ButtonState state;
  bool doubleClickPending;
};
ButtonConfig buttons[12];
int buttonCount = 0;
String bleName = "ESP32 Keyboard";
BleKeyboard bleKeyboard;

// Globale Zeiten für Doppelklick und Langklick
unsigned long doubleClickTime = 400; // ms
unsigned long longPressTime = 800; // ms

void loadConfig() {
  if (!LittleFS.begin(true)) {
    Serial.println("[DEBUG] LittleFS konnte nicht initialisiert werden!");
    return;
  }
  File file = LittleFS.open("/config.json");
  if (!file) {
    Serial.println("[DEBUG] /config.json konnte nicht geöffnet werden!");
    return;
  }
  StaticJsonDocument<1024> doc;
  DeserializationError err = deserializeJson(doc, file);
  if (err) {
    Serial.print("[DEBUG] Fehler beim Parsen von config.json: ");
    Serial.println(err.c_str());
    return;
  }
  if (doc.containsKey("ble_name")) {
    bleName = doc["ble_name"].as<String>();
  }
  if (doc.containsKey("doubleClickTime")) {
    doubleClickTime = doc["doubleClickTime"].as<unsigned long>();
  }
  if (doc.containsKey("longPressTime")) {
    longPressTime = doc["longPressTime"].as<unsigned long>();
  }
  buttonCount = doc["buttons"].size();
  for (int i = 0; i < buttonCount; i++) {
    buttons[i].pin = doc["buttons"][i]["pin"].as<int>();
    // Neue Struktur: key_normal, key_double, key_long
    if (doc["buttons"][i].containsKey("key_normal"))
      buttons[i].key_normal = doc["buttons"][i]["key_normal"].as<const char*>()[0];
    else if (doc["buttons"][i].containsKey("key"))
      buttons[i].key_normal = doc["buttons"][i]["key"].as<const char*>()[0];
    else
      buttons[i].key_normal = 'A';
    if (doc["buttons"][i].containsKey("key_double"))
      buttons[i].key_double = doc["buttons"][i]["key_double"].as<const char*>()[0];
    else
      buttons[i].key_double = buttons[i].key_normal;
    if (doc["buttons"][i].containsKey("key_long"))
      buttons[i].key_long = doc["buttons"][i]["key_long"].as<const char*>()[0];
    else
      buttons[i].key_long = buttons[i].key_normal;
    if (doc["buttons"][i].containsKey("mode")) {
      buttons[i].mode = doc["buttons"][i]["mode"].as<String>();
    } else {
      buttons[i].mode = "pullup";
    }
    if (doc["buttons"][i].containsKey("debounce")) {
      buttons[i].debounce = doc["buttons"][i]["debounce"].as<int>();
    } else {
      buttons[i].debounce = 100;
    }
    buttons[i].lastState = HIGH;
    buttons[i].lastChange = 0;
    buttons[i].pressStart = 0;
    buttons[i].lastRelease = 0;
    buttons[i].state = BTN_IDLE;
    buttons[i].doubleClickPending = false;
  }
  file.close();
  Serial.println("[DEBUG] Geladene Konfiguration:");
  Serial.print("[DEBUG] BLE-Name: ");
  Serial.println(bleName);
  Serial.print("[DEBUG] ButtonCount: ");
  Serial.println(buttonCount);
  for (int i = 0; i < buttonCount; i++) {
    Serial.print("[DEBUG] Button ");
    Serial.print(i);
    Serial.print(": Pin ");
    Serial.print(buttons[i].pin);
    Serial.print(", Key_normal '");
    Serial.print(buttons[i].key_normal);
    Serial.print("', Key_double '");
    Serial.print(buttons[i].key_double);
    Serial.print("', Key_long '");
    Serial.print(buttons[i].key_long);
    Serial.print("', Mode: ");
    Serial.println(buttons[i].mode);
  }
}

void setup() {
  //pinMode(8, OUTPUT);
  Serial.begin(115200);
  delay(5000); // Warte auf Serial-Port Initialisierung
  Serial.println("[DEBUG] setup() gestartet");
  Serial.println("[DEBUG] Serial initialisiert");
  loadConfig();
  Serial.println("[DEBUG] Konfiguration geladen");
  for (int i = 0; i < buttonCount; i++) {
    Serial.print("Init Button ");
    Serial.print(i);
    Serial.print(": Pin ");
    Serial.print(buttons[i].pin);
    Serial.print(", Key_normal '");
    Serial.print(buttons[i].key_normal);
    Serial.print("', Key_double '");
    Serial.print(buttons[i].key_double);
    Serial.print("', Key_long '");
    Serial.print(buttons[i].key_long);
    Serial.print("', Mode: ");
    Serial.print(buttons[i].mode);
    Serial.print(", Debounce: ");
    Serial.println(buttons[i].debounce);
    if (buttons[i].pin < 0 || buttons[i].pin > 39) {
      Serial.print("Warnung: Ungültiger GPIO: ");
      Serial.println(buttons[i].pin);
      continue;
    }
    if (buttons[i].mode == "pullup") {
      pinMode(buttons[i].pin, INPUT_PULLUP);
      Serial.println("[DEBUG] pinMode INPUT_PULLUP gesetzt");
    } else if (buttons[i].mode == "pulldown") {
      pinMode(buttons[i].pin, INPUT_PULLDOWN);
      Serial.println("[DEBUG] pinMode INPUT_PULLDOWN gesetzt");
    } else {
      pinMode(buttons[i].pin, INPUT);
      Serial.println("[DEBUG] pinMode INPUT gesetzt");
    }
  }
  Serial.println("[DEBUG] Alle Pins initialisiert");
  bleKeyboard.setName(bleName.c_str());
  Serial.println("[DEBUG] BLE-Name gesetzt");
  bleKeyboard.begin();
  Serial.println("[DEBUG] BLE Keyboard gestartet");
  Serial.print("Tastatur-Emulator gestartet (BLE-Modus, Name: ");
  Serial.print(bleName);
  Serial.println(")");
}

void loop() {
  /* digitalWrite(8, HIGH);
  Serial.println("[DEBUG] Builtin LED AN");
  delay(500);
  digitalWrite(8, LOW);
  Serial.println("[DEBUG] Builtin LED AUS");
  delay(500);
  */

  if (bleKeyboard.isConnected()) {
    unsigned long now = millis();
    for (int i = 0; i < buttonCount; i++) {
      int pinState = digitalRead(buttons[i].pin);
      switch (buttons[i].state) {
        case BTN_IDLE:
          if (pinState == LOW) {
            buttons[i].state = BTN_DEBOUNCE;
            buttons[i].lastChange = now;
          }
          break;
        case BTN_DEBOUNCE:
          if (pinState == LOW && (now - buttons[i].lastChange > buttons[i].debounce)) {
            buttons[i].state = BTN_PRESSED;
            buttons[i].pressStart = now;
          } else if (pinState == HIGH) {
            buttons[i].state = BTN_IDLE;
          }
          break;
        case BTN_PRESSED:
          if (pinState == HIGH) {
            // Button wurde kurz gedrückt
            if (buttons[i].doubleClickPending && (now - buttons[i].lastRelease < doubleClickTime)) {
              // Doppelklick erkannt
              Serial.print("-> Doppelklick: ");
              Serial.println(buttons[i].key_double);
              bleKeyboard.press(buttons[i].key_double);
              delay(100);
              bleKeyboard.release(buttons[i].key_double);
              buttons[i].doubleClickPending = false;
              buttons[i].state = BTN_IDLE;
            } else {
              // Warte auf zweiten Klick
              buttons[i].doubleClickPending = true;
              buttons[i].lastRelease = now;
              buttons[i].state = BTN_WAIT_DOUBLE;
            }
          } else if (now - buttons[i].pressStart > longPressTime) {
            // Langklick erkannt
            Serial.print("-> Langklick: ");
            Serial.println(buttons[i].key_long);
            bleKeyboard.press(buttons[i].key_long);
            delay(100);
            bleKeyboard.release(buttons[i].key_long);
            buttons[i].doubleClickPending = false;
            buttons[i].state = BTN_LONG;
          }
          break;
        case BTN_WAIT_DOUBLE:
          if (pinState == LOW) {
            buttons[i].state = BTN_DEBOUNCE;
            buttons[i].lastChange = now;
          } else if (now - buttons[i].lastRelease > doubleClickTime) {
            // Zeit abgelaufen, Normalklick
            Serial.print("-> Normalklick: ");
            Serial.println(buttons[i].key_normal);
            bleKeyboard.press(buttons[i].key_normal);
            delay(100);
            bleKeyboard.release(buttons[i].key_normal);
            buttons[i].doubleClickPending = false;
            buttons[i].state = BTN_IDLE;
          }
          break;
        case BTN_LONG:
          if (pinState == HIGH) {
            buttons[i].state = BTN_IDLE;
          }
          break;
        default:
          buttons[i].state = BTN_IDLE;
          break;
      }
      buttons[i].lastState = pinState;
    }
  }
}
  