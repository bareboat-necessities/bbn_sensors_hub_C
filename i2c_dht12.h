#ifndef i2c_dht12_h
#define i2c_dht12_h

#include <M5UnitENV.h>

#include "NmeaXDR.h"
#include "Nmea0183Msg.h"

DHT12 i2c_dht12_sensor;

void i2c_dht12_report() {
  if (i2c_dht12_sensor.update()) {
    gen_nmea0183_xdr("$BBXDR,H,%.2f,P,HUMI_DHT12", i2c_dht12_sensor.humidity);   // %
    gen_nmea0183_xdr("$BBXDR,C,%.2f,C,TEMP_DHT12", i2c_dht12_sensor.cTemp);      // C
  }
}

bool i2c_dht12_try_init() {
  bool i2c_dht12_found = false;
  for (int i = 0; i < 3; i++) {
    i2c_dht12_found = i2c_dht12_sensor.begin(&Wire1, DHT12_I2C_ADDR, G38, G39, 100000UL);
    if (i2c_dht12_found) {
      break;
    }
    delay(10);
  }
  if (i2c_dht12_found) {
    gen_nmea0183_msg("$BBTXT,01,01,01,ENVIRONMENT found dht12 sensor at address=0x%s", String(DHT12_I2C_ADDR, HEX).c_str());
    app.onRepeat(5000, []() {
      i2c_dht12_report();
    });
  }
  return i2c_dht12_found;
}

#endif
