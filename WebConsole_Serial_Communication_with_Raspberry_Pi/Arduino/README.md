# 🤖 Arduino — WebConsole_Serial_Communication_with_Raspberry_Pi

Sensor station firmware with three operating modes.

## 📂 Files

| File | Description |
|------|-------------|
| `RASP_ARDUINO_SERIAL_COM.ino` | Main sketch — setup, loop, command handler |
| `modes.h` | Mode constants, menu printer |
| `sensors.h` | Sensor read functions (simulated + real stubs) |
| `led_control.h` | LED pin control + state tracking |

## ⚙️ Modes

### Default Mode
Arduino streams a JSON packet every `streamInterval` ms (default: 2000 ms):
```json
{"mode":"default","ts":4521,"temp":24.3,"hum":58.1,"wind_spd":3.2,"wind_dir":"NE","lux":812,"leds":{"red":false,"green":true,"blue":false}}
```
Trigger: power-on or send `default` / `mode default`

### Interactive Mode
Arduino waits for commands and responds.  
Trigger: send `interactive` or `mode interactive`

Type `menu` to see all available commands.

### Hybrid Mode
Streams data AND responds to commands simultaneously.  
Trigger: send `hybrid` or `mode hybrid`

## 🔌 Pin Map

| Pin | Component |
|-----|-----------|
| D4  | DHT22 data |
| D8  | Blue LED |
| D9  | Green LED |
| D10 | Red LED |
| A0  | Anemometer (analog) |
| A1  | Wind vane (analog) |
| A2  | LDR luminosity |

## 🛠️ Upload

1. Open `RASP_ARDUINO_SERIAL_COM.ino` in Arduino IDE
2. **Tools → Board** → Arduino Uno (or your board)
3. **Tools → Port** → select your COM / ttyUSB port
4. Click **Upload**
5. Open Serial Monitor at `9600` baud to observe

## 🔧 Replacing Simulated Sensors

In `sensors.h`, replace the body of each `readXxx()` function with real sensor reads:

```cpp
// Example: DHT22 real read
#include <DHT.h>
DHT dht(DHT_PIN, DHT22);
inline float readTemp()     { return dht.readTemperature(); }
inline float readHumidity() { return dht.readHumidity(); }
```
