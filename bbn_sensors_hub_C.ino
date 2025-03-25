#include <M5Unified.h>
#include <Arduino.h>
#include <Wire.h>
#include <ReactESP.h>  // https://github.com/mairas/ReactESP

using namespace reactesp;
ReactESP app;

#include "NmeaXDR.h"
#include "Nmea0183Msg.h"
#include "mcu_sensors.h"

static const char* firmware_tag = "bbn_sensors_hub_C";

void setup() {
  auto cfg = M5.config();
  M5.begin(cfg);
  Wire1.begin(G38, G39, 100000UL);
  Serial.begin(38400);
  gen_nmea0183_msg("$BBTXT,01,01,01,FirmwareTag: %s", firmware_tag);
  mcu_sensors_scan();
}

void loop() {
  M5.update();
  app.tick();
  mcu_sensors_update();
}
