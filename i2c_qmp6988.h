#ifndef i2c_qmp6988_h
#define i2c_qmp6988_h

#include <M5UnitENV.h>

#include "NmeaXDR.h"
#include "Nmea0183Msg.h"

#define QMP6988_SLAVE_ADDRESS_L (0x70)
#define QMP6988_SLAVE_ADDRESS_H (0x56)

QMP6988 i2c_qmp6988_sensor;

void i2c_qmp6988_report() {
  if (i2c_qmp6988_sensor.update()) {
    gen_nmea0183_xdr("$BBXDR,C,%.2f,C,TEMP_QMP6988", i2c_qmp6988_sensor.cTemp);          // C
    gen_nmea0183_xdr("$BBXDR,P,%.2f,P,PRES_QMP6988", i2c_qmp6988_sensor.pressure);       // Pa
  }
}

bool i2c_qmp6988_try_init() {
  bool i2c_qmp6988_found = false;
  for (int i = 0; i < 3; i++) {
    i2c_qmp6988_found = i2c_qmp6988_sensor.begin(&Wire1, QMP6988_SLAVE_ADDRESS_L, G38, G39, 100000UL);
    if (i2c_qmp6988_found) {
      break;
    }
    delay(10);
  }
  if (i2c_qmp6988_found) {
    gen_nmea0183_msg("$BBTXT,01,01,01,ENVIRONMENT found qmp6988 sensor at address=0x%s", String(QMP6988_SLAVE_ADDRESS_L, HEX).c_str());
    app.onRepeat(5000, []() {
      i2c_qmp6988_report();
    });
  }
  return i2c_qmp6988_found;
}

#endif
