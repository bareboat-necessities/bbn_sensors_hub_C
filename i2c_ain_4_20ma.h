#ifndef i2c_ain_4_20ma_h
#define i2c_ain_4_20ma_h

#include <Wire.h>

#include "MODULE_4_20MA.h"
#include "NmeaXDR.h"
#include "NmeaChecksum.h"

MODULE_4_20MA i2c_ain_4_20ma_sensor_0;  // I2C default bus
MODULE_4_20MA i2c_ain_4_20ma_sensor_1;  // I2C additional bus

void i2c_ain_4_20ma_report(MODULE_4_20MA *i2c_ain_4_20ma_sensor) {
  if (i2c_ain_4_20ma_sensor == &i2c_ain_4_20ma_sensor_0) {
    gen_nmea0183_xdr("$BBXDR,I,%.5f,A,AMPS", (float)(i2c_ain_4_20ma_sensor->getCurrentValue(0)) / 100.0 / 1000.0); // A
  } else {
    gen_nmea0183_xdr("$BBXDR,I,%.5f,A,AMPS_1", (float)(i2c_ain_4_20ma_sensor->getCurrentValue(0)) / 100.0 / 1000.0);
  }
}

bool i2c_ain_4_20ma_try_init(TwoWire *wire = &Wire, uint8_t sda = SDA, uint8_t scl = SCL, uint32_t freq = 1000000UL) {
  bool i2c_ain_4_20ma_found = false;
  MODULE_4_20MA *i2c_ain_4_20ma_sensor = (wire == &Wire ? &i2c_ain_4_20ma_sensor_0 : &i2c_ain_4_20ma_sensor_1);
  for (int i = 0; i < 3; i++) {
    i2c_ain_4_20ma_found = i2c_ain_4_20ma_sensor->begin(wire, MODULE_4_20MA_ADDR, sda, scl, freq);
    if (i2c_ain_4_20ma_found) {
      break;
    }
    delay(10);
  }
  if (i2c_ain_4_20ma_found) {
    gen_nmea0183_msg("$BBTXT,01,01,01,ELECTRICAL found 4-20mAmp sensor at address=0x%s",
      (String(MODULE_4_20MA_ADDR, HEX) + String((wire == &Wire ? " main bus" : " alt bus"))).c_str());
    app.onRepeat(5000, [i2c_ain_4_20ma_sensor]() {
      i2c_ain_4_20ma_report(i2c_ain_4_20ma_sensor);
    });
  }
  return i2c_ain_4_20ma_found;
}

#endif
