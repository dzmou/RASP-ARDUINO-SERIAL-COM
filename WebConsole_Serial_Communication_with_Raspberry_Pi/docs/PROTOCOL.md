# 📡 Serial Communication Protocol

**Port:** `/dev/ttyUSB0`  **Baud:** `9600`  **Line ending:** `\n`

---

## 1. Default Mode — Streaming Payload

Arduino sends a JSON line every `STREAM_INTERVAL` ms (default 2000 ms):

```json
{"mode":"default","ts":12345,"temp":24.3,"hum":58.1,"wind_spd":3.2,"wind_dir":"NE","lux":812,"leds":{"red":false,"green":true,"blue":false}}
```

| Field | Type | Unit | Description |
|-------|------|------|-------------|
| `mode` | string | — | Current mode (`default`/`interactive`/`hybrid`) |
| `ts` | int | ms | `millis()` timestamp |
| `temp` | float | °C | Temperature |
| `hum` | float | % | Humidity |
| `wind_spd` | float | m/s | Wind speed |
| `wind_dir` | string | — | Cardinal direction (N/NE/E/SE/S/SW/W/NW) |
| `lux` | int | lux | Sun luminosity |
| `leds` | object | — | LED states |

---

## 2. Interactive Mode — Commands & Responses

### Commands sent by Raspberry Pi → Arduino

| Command | Description |
|---------|-------------|
| `menu` | Print interactive menu |
| `read` | Read all sensors once |
| `read temp` | Read temperature only |
| `read hum` | Read humidity only |
| `read wind` | Read wind speed + direction |
| `read lux` | Read luminosity only |
| `led red on` | Turn red LED on |
| `led red off` | Turn red LED off |
| `led green on` | Turn green LED on |
| `led green off` | Turn green LED off |
| `led blue on` | Turn blue LED on |
| `led blue off` | Turn blue LED off |
| `led all on` | All LEDs on |
| `led all off` | All LEDs off |
| `interval <ms>` | Set stream interval (e.g. `interval 5000`) |
| `status` | Device info + current mode |
| `mode default` | Switch to default mode |
| `mode interactive` | Switch to interactive mode |
| `mode hybrid` | Switch to hybrid mode |
| `reset` | Soft reset Arduino |
| `ping` | Health check |

### Arduino Responses (Interactive)

```
[MENU]
  1. read          - Read all sensors
  2. read <sensor> - Read specific sensor (temp/hum/wind/lux)
  3. led <color> <on/off> - Control LED
  4. led all <on/off>     - All LEDs
  5. interval <ms>        - Set stream interval
  6. status        - Device info
  7. mode <default/interactive/hybrid> - Switch mode
  8. reset         - Soft reset
  9. ping          - Health check
[END MENU]
```

---

## 3. Hybrid Mode

- Arduino streams data at the set interval
- Simultaneously parses incoming commands between stream cycles
- Commands that arrive mid-stream are queued and executed after next packet

---

## 4. Error Responses

```
[ERR] Unknown command: <cmd>
[ERR] Invalid LED color: <color>
[ERR] Interval out of range (500–60000 ms)
```
