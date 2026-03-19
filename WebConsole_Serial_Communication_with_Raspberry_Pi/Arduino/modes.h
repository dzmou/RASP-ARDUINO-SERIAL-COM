#ifndef MODES_H
#define MODES_H

// ── Operating Modes ──────────────────────────────────────────
#define MODE_DEFAULT     0   // Continuous streaming
#define MODE_INTERACTIVE 1   // Command/response only
#define MODE_HYBRID      2   // Stream + listen simultaneously

extern int currentMode;
extern unsigned long streamInterval;

// ── Mode Name Helper ──────────────────────────────────────────
inline const char* modeName(int m) {
  switch (m) {
    case MODE_DEFAULT:     return "default";
    case MODE_INTERACTIVE: return "interactive";
    case MODE_HYBRID:      return "hybrid";
    default:               return "unknown";
  }
}

// ── Interactive Menu ──────────────────────────────────────────
inline void printMenu() {
  Serial.println(F("[MENU]"));
  Serial.println(F("  read              - Read all sensors"));
  Serial.println(F("  read temp         - Temperature only"));
  Serial.println(F("  read hum          - Humidity only"));
  Serial.println(F("  read wind         - Wind speed & direction"));
  Serial.println(F("  read lux          - Luminosity only"));
  Serial.println(F("  led <color> on/off- Control LED (red/green/blue)"));
  Serial.println(F("  led all on/off    - All LEDs on or off"));
  Serial.println(F("  interval <ms>     - Set stream interval (500-60000)"));
  Serial.println(F("  status            - Device info & current mode"));
  Serial.println(F("  mode default      - Switch to default mode"));
  Serial.println(F("  mode interactive  - Switch to interactive mode"));
  Serial.println(F("  mode hybrid       - Switch to hybrid mode"));
  Serial.println(F("  reset             - Soft reset device"));
  Serial.println(F("  ping              - Health check"));
  Serial.println(F("  menu              - Show this menu"));
  Serial.println(F("[END MENU]"));
}

#endif
