#ifndef i2c_sgp30_h
#define i2c_sgp30_h

#include <Adafruit_SGP30.h>

#include "NmeaXDR.h"
#include "Nmea0183Msg.h"

//#define SGP30_I2CADDR_DEFAULT  0x58 // SGP30 has only one I2C address

Adafruit_SGP30 i2c_sgp30_sensor;

int sgp_counter = 0;

void i2c_sgp30_report() {
  // If you have a temperature / humidity sensor, you can set the absolute humidity
  // to enable the humidity compensation for the air quality signals

  //float temperature = 22.1; // [°C]
  //float humidity = 45.2;    // [%RH]
  //i2c_sgp30_sensor.setHumidity(getAbsoluteHumidity(temperature, humidity));

  // Commands the sensor to take a single eCO2/VOC measurement.
  if (i2c_sgp30_sensor.IAQmeasure()) {
    gen_nmea0183_xdr("$BBXDR,X,%.0f,,TVOC", i2c_sgp30_sensor.TVOC);        // ppb
    gen_nmea0183_xdr("$BBXDR,X,%.0f,,eCO2", i2c_sgp30_sensor.eCO2);        // ppm
  }
  if (i2c_sgp30_sensor.IAQmeasureRaw()) {
    gen_nmea0183_xdr("$BBXDR,X,%.0f,,rawH2", i2c_sgp30_sensor.rawH2);
    gen_nmea0183_xdr("$BBXDR,X,%.0f,,rawEthanol", i2c_sgp30_sensor.rawEthanol);
  }
  sgp_counter++;
  if (sgp_counter == 6) {
    sgp_counter = 0;
    uint16_t TVOC_base, eCO2_base;
    i2c_sgp30_sensor.getIAQBaseline(&eCO2_base, &TVOC_base);
  }
}

bool i2c_sgp30_try_init() {
  bool i2c_sgp30_found = false;
  for (int i = 0; i < 3; i++) {
    i2c_sgp30_found = i2c_sgp30_sensor.begin(&Wire1, true);
    if (i2c_sgp30_found) {
      break;
    }
    delay(10);
  }
  if (i2c_sgp30_found) {
    gen_nmea0183_msg("$BBTXT,01,01,01,ENVIRONMENT found sgp30 sensor at address=0x%s", String(SGP30_I2CADDR_DEFAULT, HEX).c_str());
    app.onRepeat(5000, []() {
      i2c_sgp30_report();
    });
  }
  return i2c_sgp30_found;
}

#endif
