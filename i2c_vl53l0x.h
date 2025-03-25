#ifndef i2c_vl53l0x_h
#define i2c_vl53l0x_h

#include <Adafruit_VL53L0X.h>

#include "NmeaXDR.h"
#include "Nmea0183Msg.h"

Adafruit_VL53L0X i2c_vl53l0x_sensor = Adafruit_VL53L0X();

const char* VL53L0X_NAME = "VL53L0X";

void i2c_vl53l0x_report() {
  VL53L0X_RangingMeasurementData_t measure;
  i2c_vl53l0x_sensor.rangingTest(&measure, false); // pass in 'true' to get debug data printout!
  if (measure.RangeStatus != 4) {                  // phase failures have incorrect data
    gen_nmea0183_xdr("$BBXDR,D,%.4f,M,RANGE_VL53L0X", measure.RangeMilliMeter / 1000.0);
  } else {
    // out of range
  }
}

bool i2c_vl53l0x_try_init() {
  bool i2c_vl53l0x_found = false;
  for (int i = 0; i < 3; i++) {
    i2c_vl53l0x_found = i2c_vl53l0x_sensor.begin(VL53L0X_I2C_ADDR, false, &Wire1);
    if (i2c_vl53l0x_found) {
      break;
    }
    delay(10);
  }
  if (i2c_vl53l0x_found) {
    gen_nmea0183_msg("$BBTXT,01,01,01,RANGE found vl53l0x sensor at address=0x%s", String(VL53L0X_I2C_ADDR, HEX).c_str());
    app.onRepeat(2000, []() {
      i2c_vl53l0x_report();
    });
  }
  return i2c_vl53l0x_found;
}

#endif
