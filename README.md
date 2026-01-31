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

# Summary of All Working Shortcuts
Here's a consolidated list of all the currently functioning keyboard shortcuts in MyWhoosh:

Category        	Key(s)        	Function                        
Steering        	← → or A D         	links / rechts auf Strecke steuern      
Emotes          	1 - 7        	Peace, Wave, Fist bump, Dap, Elbow flick, Toast, Thumbs up          
UI Toggle        	U            	Minimal UI Mode                  
Virtual Shifting	I / K        	Gear Up / Down                  

leider gibt es derzeit keine, mir bekannten Tasten, um Abzubiegen oder um einen U-Turn auszuführen

## Lizenz
MIT License

---

**Hinweis:**
Dieses Projekt ist ein Community-Projekt und steht in keiner Verbindung zu MYWhoosh oder deren Entwicklern. Für Schäden oder Fehlfunktionen wird keine Haftung übernommen.
