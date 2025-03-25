#ifndef i2c_bmp280_h
#define i2c_bmp280_h

#include <M5UnitENV.h>

#include "NmeaXDR.h"
#include "Nmea0183Msg.h"

BMP280 i2c_bmp280_sensor;

void i2c_bmp280_report() {
  if (i2c_bmp280_sensor.update()) {
    gen_nmea0183_xdr("$BBXDR,C,%.2f,C,TEMP_BMP280", i2c_bmp280_sensor.cTemp);        // C
    gen_nmea0183_xdr("$BBXDR,P,%.2f,P,PRES_BMP280", i2c_bmp280_sensor.pressure);     // Pa
  }
}

bool i2c_bmp280_try_init() {
  bool i2c_bmp280_found = false;
  for (int i = 0; i < 3; i++) {
    i2c_bmp280_found = i2c_bmp280_sensor.begin(&Wire1, BMP280_I2C_ADDR, G38, G39, 100000UL);
    if (i2c_bmp280_found) {
      break;
    }
    delay(10);
  }
  if (i2c_bmp280_found) {
    gen_nmea0183_msg("$BBTXT,01,01,01,ENVIRONMENT found bmp280 sensor at address=0x%s", String(BMP280_I2C_ADDR, HEX).c_str());
    /* Default settings from datasheet. */
    i2c_bmp280_sensor.setSampling(BMP280::MODE_NORMAL,     /* Operating Mode. */
                                  BMP280::SAMPLING_X2,     /* Temp. oversampling */
                                  BMP280::SAMPLING_X16,    /* Pressure oversampling */
                                  BMP280::FILTER_X16,      /* Filtering. */
                                  BMP280::STANDBY_MS_500); /* Standby time. */
    app.onRepeat(5000, []() {
      i2c_bmp280_report();
    });
  }
  return i2c_bmp280_found;
}

#endif
