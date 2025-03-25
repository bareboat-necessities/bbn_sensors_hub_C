#ifndef i2c_bh1750fvi_tr_h
#define i2c_bh1750fvi_tr_h

#include <Wire.h>
#include <M5_DLight.h>   // https://github.com/m5stack/M5-DLight/

#include "NmeaXDR.h"
#include "Nmea0183Msg.h"

#define BH1750FVI_TR_I2C_ADDR 0x23

M5_DLight i2c_bh1750fvi_tr_sensor(BH1750FVI_TR_I2C_ADDR);

void i2c_bh1750fvi_tr_report() {
  uint16_t lux = i2c_bh1750fvi_tr_sensor.getLUX();
  gen_nmea0183_xdr("$BBXDR,X,%.1f,L,ILLU", (float)lux);
}

bool i2c_bh1750fvi_tr_try_init() {
  bool i2c_bh1750fvi_tr_found = false;
  for (int i = 0; i < 3; i++) {
    Wire1.beginTransmission(BH1750FVI_TR_I2C_ADDR);
    i2c_bh1750fvi_tr_found = !Wire1.endTransmission();
    if (i2c_bh1750fvi_tr_found) {
      i2c_bh1750fvi_tr_sensor.begin(&Wire1, G38, G39, 100000UL);
      break;
    }
    delay(10);
  }
  if (i2c_bh1750fvi_tr_found) {
    gen_nmea0183_msg("$BBTXT,01,01,01,ENVIRONMENT found bh1750fvi-tr sensor at address=0x%s", String(BH1750FVI_TR_I2C_ADDR, HEX).c_str());
    // CONTINUOUSLY_H_RESOLUTION_MODE
    // CONTINUOUSLY_H_RESOLUTION_MODE2
    // CONTINUOUSLY_L_RESOLUTION_MODE
    // ONE_TIME_H_RESOLUTION_MODE
    // ONE_TIME_H_RESOLUTION_MODE2
    // ONE_TIME_L_RESOLUTION_MODE
    i2c_bh1750fvi_tr_sensor.setMode(CONTINUOUSLY_L_RESOLUTION_MODE);
    app.onRepeat(2000, []() {
      i2c_bh1750fvi_tr_report();
    });
  }
  return i2c_bh1750fvi_tr_found;
}

#endif



