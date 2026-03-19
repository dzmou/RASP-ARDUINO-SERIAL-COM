#ifndef SENSORS_H
#define SENSORS_H

// ── Pin Definitions ───────────────────────────────────────────
#define DHT_PIN       4    // DHT22 data pin
#define WIND_SPD_PIN  A0   // Anemometer analog input
#define WIND_DIR_PIN  A1   // Wind vane analog input
#define LUX_PIN       A2   // LDR / BH1750 analog input

// ── Simulated Sensor State (replace with real reads) ──────────
static float  _temp     = 22.0;
static float  _hum      = 55.0;
static float  _windSpd  = 0.0;
static int    _windDirIdx = 0;
static int    _lux      = 500;
static int    _simTick  = 0;

const char* windDirs[] = {"N","NE","E","SE","S","SW","W","NW"};

// ── Simulate drifting sensor values ───────────────────────────
inline void updateSimulatedSensors() {
  _simTick++;
  // Gentle random walk
  _temp    += (random(-10, 11)) * 0.05;
  _hum     += (random(-10, 11)) * 0.1;
  _windSpd  = max(0.0, _windSpd + (random(-5, 6)) * 0.1);
  _lux     += random(-30, 31);

  // Clamp
  _temp    = constrain(_temp,    -10.0, 50.0);
  _hum     = constrain(_hum,      0.0, 100.0);
  _windSpd = constrain(_windSpd,  0.0, 30.0);
  _lux     = constrain(_lux,      0,   65000);

  // Rotate wind direction slowly
  if (_simTick % 10 == 0) _windDirIdx = (_windDirIdx + 1) % 8;
}

// ── Read Functions ─────────────────────────────────────────────
inline float readTemp()        { return _temp; }
inline float readHumidity()    { return _hum; }
inline float readWindSpeed()   { return _windSpd; }
inline const char* readWindDir() { return windDirs[_windDirIdx]; }
inline int   readLux()         { return _lux; }

// ── Print helpers ──────────────────────────────────────────────
inline void printAllSensors() {
  Serial.print(F("[DATA] temp="));    Serial.print(readTemp(), 1);
  Serial.print(F(" hum="));           Serial.print(readHumidity(), 1);
  Serial.print(F(" wind_spd="));      Serial.print(readWindSpeed(), 1);
  Serial.print(F(" wind_dir="));      Serial.print(readWindDir());
  Serial.print(F(" lux="));           Serial.println(readLux());
}

#endif
