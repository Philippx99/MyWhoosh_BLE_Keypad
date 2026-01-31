# MyWhoosh ESP32 BLE Keypad für MYWhoosh Rollentrainer

Dieses Projekt implementiert ein BLE-Keypad auf Basis eines ESP32/ESP32-C3, um den Rollentrainer in der MYWhoosh-App komfortabel zu steuern. Es können pro Taste drei verschiedene Aktionen (Normalklick, Doppelklick, Langklick) ausgeführt werden. Die Tastenbelegung und Klickzeiten sind flexibel über eine Konfigurationsdatei anpassbar.

## Features

- **BLE-Tastatur-Emulation**: Funktioniert als drahtloses Keypad für MYWhoosh
- **Mehrfachbelegung pro Taste**: Normalklick, Doppelklick, Langklick je GPIO
- **Konfigurierbar**: Tasten, Modi und Zeiten über `config.json` einstellbar
- **Kompatibel mit ESP32 und ESP32-C3**
- **Einfache Anpassung**: Keine Programmierkenntnisse für Tastenbelegung nötig

## Hardware
- ESP32 oder ESP32-C3 Devboard
- Taster (beliebige Anzahl, empfohlen: 4–12)
- Optional: Gehäuse, Pullup/Pulldown-Widerstände

## Installation

1. **Repository klonen**
   ```sh
   git clone https://github.com/dein-github/mywhoosh-keypad.git
   cd mywhoosh-keypad
   ```
2. **PlatformIO installieren** (VS Code empfohlen)
3. **Board auswählen**
   - In `platformio.ini` das passende Board setzen (`esp32dev` oder `esp32-c3-devkitm-1`)
4. **Tastenbelegung anpassen**
   - Datei `data/config.json` nach Wunsch editieren
5. **Dateisystem-Image hochladen**
   ```sh
   pio run --target uploadfs
   ```
6. **Firmware flashen**
   ```sh
   pio run --target upload
   ```
7. **Mit MYWhoosh koppeln**
   - Das Keypad erscheint als BLE-Tastatur (Name konfigurierbar)

## Konfiguration (`data/config.json`)

Beispiel:
```json
{
  "ble_name": "Mywhoosh_Keypad",
  "doubleClickTime": 400,
  "longPressTime": 800,
  "buttons": [
    { "pin": 1, "key_normal": "I", "key_double": "J", "key_long": "L", "mode": "pullup", "debounce": 100 },
    { "pin": 2, "key_normal": "K", "key_double": "M", "key_long": "N", "mode": "pullup", "debounce": 100 }
  ]
}
```
- **ble_name**: Anzeigename im BLE
- **doubleClickTime**: Zeitfenster für Doppelklick (ms, global)
- **longPressTime**: Zeit für Langklick (ms, global)
- **buttons**: Liste der Tasten (GPIO, Keycodes, Modus, Entprellzeit)

## Tastenbelegung für MYWhoosh
- Die Keycodes entsprechen den Tastaturbefehlen, die MYWhoosh akzeptiert (z.B. I, K, D, A, ...)
- Jede Taste kann drei Aktionen auslösen (z.B. "key_normal": "I", "key_double": "J", "key_long": "L")

# Alle bekanten Tasten 
Hier ist eine zusammengefasste Liste aller aktuell in MyWhoosh funktionierenden Tastenkombinationen:

Kategorie |	Tasten | Funktion                        
Steuerung | ← → or A D |links / rechts auf Strecke steuern (Abbiegen derzeit nicht möglich)     
Emotes | 1 - 7 | Peace, Wave, Fist bump, Dap, Elbow flick, Toast, Thumbs up          
UI Toggle | U | Minimal UI Mode                  
Virtual Schalten |I / K | Gang hoch / runter                    

leider gibt es derzeit keine, mir bekannten Tasten, um Abzubiegen oder um einen U-Turn auszuführen

für die Cursortasten müssen folgende bezeichnungen anstatt eines Buchstabens oder einer Zahl verwendet werden: 
Pfeil nach oben: "KEY_UP_ARROW"
Pfeil nach unten: "KEY_DOWN_ARROW"
Pfeil nach links: "KEY_LEFT_ARROW"
Pfeil nach rechts: "KEY_RIGHT_ARROW"
*** alle weiteren Sondertasten entnehmen sie bitte der "BleKeybords.h" zwischen Zeile 36 und 120 aus de ESP32 BLE Keybord Library

## Lizenz
MIT License

---

**Hinweis:**
Dieses Projekt ist ein Community-Projekt und steht in keiner Verbindung zu MYWhoosh oder deren Entwicklern. Für Schäden oder Fehlfunktionen wird keine Haftung übernommen.

