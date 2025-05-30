#ifndef gpio_240_30_ohm_sensor_h
#define gpio_240_30_ohm_sensor_h

#include "ResistanceSensor.h"
#include "NmeaXDR.h"

#define Resistance_Sensor_GPIO_PIN G1

ResistanceSensor resistanceSens;

void gpio_240_30_ohm_report() {
  ResistanceSensor_read(&resistanceSens);
  gen_nmea0183_xdr("$BBXDR,G,%.1f,O,Ohms_esp32", resistanceSens.measured_resistance);
}

void gpio_240_30_ohm_try_init() {
  // for 240-30 Ohm sensors (USA standard). Good for 0-180 Ohm too (EU Standard).
  // Japanese standard is 0 to 310 Ohm range
  ResistanceSensor_init(&resistanceSens, Resistance_Sensor_GPIO_PIN, DOWNSTREAM, 3.3, 100.0);
  app.onRepeat(1000, []() {
    gpio_240_30_ohm_report();
  });
}

#endif
