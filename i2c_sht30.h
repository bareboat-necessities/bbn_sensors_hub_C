#ifndef i2c_sht30_h
#define i2c_sht30_h

#include <M5UnitENV.h>

#include "NmeaXDR.h"
#include "Nmea0183Msg.h"

#define SHT3X_I2C_ADDR 0x44

SHT3X i2c_sht30_sensor;

void i2c_sht30_report() {
  if (i2c_sht30_sensor.update()) {
    gen_nmea0183_xdr("$BBXDR,H,%.2f,P,HUMI_SHT3X", i2c_sht30_sensor.humidity);    // %
    gen_nmea0183_xdr("$BBXDR,C,%.2f,C,TEMP_SHT3X", i2c_sht30_sensor.cTemp);       // C
  }
}

bool i2c_sht30_try_init() {
  bool i2c_sht30_found = false;
  for (int i = 0; i < 3; i++) {
    i2c_sht30_found = i2c_sht30_sensor.begin(&Wire1, SHT3X_I2C_ADDR, G38, G39, 100000UL);
    if (i2c_sht30_found) {
      break;
    }
    delay(10);
  }
  if (i2c_sht30_found) {
    gen_nmea0183_msg("$BBTXT,01,01,01,ENVIRONMENT found sht30 sensor at address=0x%s", String(SHT3X_I2C_ADDR, HEX).c_str());
    app.onRepeat(5000, []() {
      i2c_sht30_report();
    });
  }
  return i2c_sht30_found;
}

#endif
