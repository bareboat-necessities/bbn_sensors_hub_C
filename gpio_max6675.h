#ifndef gpio_max6675_h
#define gpio_max6675_h

#include "Max6675.h"

int thermoSO = 5;
int thermoCS = 6;
int thermoCLK = 7;

MAX6675 thermocouple(thermoCLK, thermoCS, thermoSO);

void gpio_max6675_report() {
  // basic readout, just print the current temp
  gen_nmea0183_xdr("$BBXDR,C,%.2f,C,THERMOCOUPLE", thermocouple.readCelsius());        // C
}

void gpio_max6675_try_init() {
  pinMode(thermoSO, INPUT);
  pinMode(thermoCS, OUTPUT);
  pinMode(thermoCLK, OUTPUT);
  // wait for MAX chip to stabilize
  delay(500);
  // For the MAX6675 to update, you must delay AT LEAST 250ms between reads!
  app.onRepeat(1000, []() {
    gpio_max6675_report();
  });
}

#endif
