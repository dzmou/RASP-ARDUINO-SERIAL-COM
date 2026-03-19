// ════════════════════════════════════════════════════════════════
//  WebConsole_Serial_Communication_with_Raspberry_Pi
//  Arduino Sensor Station — Default / Interactive / Hybrid Mode
//  github.com/dzmou/RASP-ARDOUINO-SERIAL-COM
// ════════════════════════════════════════════════════════════════

#include "modes.h"
#include "sensors.h"
#include "led_control.h"

// ── Globals ───────────────────────────────────────────────────
int           currentMode    = MODE_DEFAULT;
unsigned long streamInterval = 2000;       // ms between stream packets
unsigned long lastStream     = 0;
String        cmdBuffer      = "";
LedState      leds;

// ── Setup ─────────────────────────────────────────────────────
void setup() {
  Serial.begin(9600);
  setupLeds();
  randomSeed(analogRead(A5));

  delay(500);   // Let serial settle after reset
  Serial.println(F("[BOOT] WebConsole_Serial_Communication_with_Raspberry_Pi v1.0"));
  Serial.println(F("[BOOT] Mode: default — streaming started"));
  Serial.print(F("[BOOT] Interval: "));
  Serial.print(streamInterval);
  Serial.println(F(" ms"));
}

// ── Main Loop ─────────────────────────────────────────────────
void loop() {
  readSerial();
  updateSimulatedSensors();

  unsigned long now = millis();

  if (currentMode == MODE_DEFAULT || currentMode == MODE_HYBRID) {
    if (now - lastStream >= streamInterval) {
      lastStream = now;
      streamPacket();
    }
  }
}

// ── Stream JSON Packet (Default / Hybrid) ─────────────────────
void streamPacket() {
  Serial.print(F("{\"mode\":\""));
  Serial.print(modeName(currentMode));
  Serial.print(F("\",\"ts\":"));
  Serial.print(millis());
  Serial.print(F(",\"temp\":"));
  Serial.print(readTemp(), 1);
  Serial.print(F(",\"hum\":"));
  Serial.print(readHumidity(), 1);
  Serial.print(F(",\"wind_spd\":"));
  Serial.print(readWindSpeed(), 1);
  Serial.print(F(",\"wind_dir\":\""));
  Serial.print(readWindDir());
  Serial.print(F("\",\"lux\":"));
  Serial.print(readLux());
  Serial.print(F(","));
  printLedJson();
  Serial.println(F("}"));
}

// ── Serial Command Reader ─────────────────────────────────────
void readSerial() {
  while (Serial.available()) {
    char c = Serial.read();
    if (c == '\n' || c == '\r') {
      cmdBuffer.trim();
      if (cmdBuffer.length() > 0) {
        handleCommand(cmdBuffer);
        cmdBuffer = "";
      }
    } else {
      cmdBuffer += c;
    }
  }
}

// ── Command Handler ───────────────────────────────────────────
void handleCommand(String cmd) {
  cmd.toLowerCase();
  cmd.trim();

  // ── Mode switching ──
  if (cmd == "default" || cmd == "mode default") {
    currentMode = MODE_DEFAULT;
    lastStream  = 0;
    Serial.println(F("[MODE] Switched to DEFAULT (streaming)"));
    return;
  }
  if (cmd == "interactive" || cmd == "mode interactive") {
    currentMode = MODE_INTERACTIVE;
    Serial.println(F("[MODE] Switched to INTERACTIVE"));
    printMenu();
    return;
  }
  if (cmd == "hybrid" || cmd == "mode hybrid") {
    currentMode = MODE_HYBRID;
    lastStream  = 0;
    Serial.println(F("[MODE] Switched to HYBRID (stream + listen)"));
    return;
  }

  // ── Interactive / Hybrid commands ──
  if (cmd == "menu") {
    printMenu();
    return;
  }

  if (cmd == "ping") {
    Serial.print(F("[PONG] alive mode="));
    Serial.print(modeName(currentMode));
    Serial.print(F(" uptime="));
    Serial.print(millis() / 1000);
    Serial.println(F("s"));
    return;
  }

  if (cmd == "status") {
    Serial.print(F("[STATUS] mode="));      Serial.println(modeName(currentMode));
    Serial.print(F("[STATUS] interval="));  Serial.print(streamInterval); Serial.println(F("ms"));
    Serial.print(F("[STATUS] uptime="));    Serial.print(millis()/1000);  Serial.println(F("s"));
    Serial.print(F("[STATUS] fw=v1.0 port=9600"));
    Serial.println();
    printLedStatus();
    return;
  }

  if (cmd == "read") {
    updateSimulatedSensors();
    printAllSensors();
    printLedStatus();
    return;
  }
  if (cmd == "read temp") {
    Serial.print(F("[DATA] temp=")); Serial.println(readTemp(), 1);
    return;
  }
  if (cmd == "read hum") {
    Serial.print(F("[DATA] hum=")); Serial.println(readHumidity(), 1);
    return;
  }
  if (cmd == "read wind") {
    Serial.print(F("[DATA] wind_spd=")); Serial.print(readWindSpeed(), 1);
    Serial.print(F(" wind_dir="));       Serial.println(readWindDir());
    return;
  }
  if (cmd == "read lux") {
    Serial.print(F("[DATA] lux=")); Serial.println(readLux());
    return;
  }

  // ── LED: led <color> <on|off> ──
  if (cmd.startsWith("led ")) {
    String args  = cmd.substring(4);
    int    space = args.indexOf(' ');
    if (space == -1) {
      Serial.println(F("[ERR] Usage: led <color> on/off"));
      return;
    }
    String color = args.substring(0, space);
    String state = args.substring(space + 1);
    state.trim(); color.trim();

    if (state != "on" && state != "off") {
      Serial.println(F("[ERR] State must be 'on' or 'off'"));
      return;
    }
    bool on = (state == "on");
    setLed(color.c_str(), on);
    Serial.print(F("[LED] ")); Serial.print(color);
    Serial.print(F(" -> ")); Serial.println(state);
    return;
  }

  // ── Interval: interval <ms> ──
  if (cmd.startsWith("interval ")) {
    long val = cmd.substring(9).toInt();
    if (val < 500 || val > 60000) {
      Serial.println(F("[ERR] Interval out of range (500-60000 ms)"));
    } else {
      streamInterval = (unsigned long)val;
      Serial.print(F("[CFG] Interval set to "));
      Serial.print(streamInterval);
      Serial.println(F(" ms"));
    }
    return;
  }

  // ── Reset ──
  if (cmd == "reset") {
    Serial.println(F("[RESET] Restarting in 1s…"));
    delay(1000);
    // Soft reset via watchdog — uncomment if avr/wdt.h is available
    // wdt_enable(WDTO_15MS); while(1) {}
    // Fallback: jump to 0
    asm volatile ("jmp 0");
    return;
  }

  // ── Unknown ──
  Serial.print(F("[ERR] Unknown command: "));
  Serial.println(cmd);
  if (currentMode == MODE_INTERACTIVE) {
    Serial.println(F("      Type 'menu' for available commands."));
  }
}
