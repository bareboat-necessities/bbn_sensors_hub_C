#ifndef i2c_ina219_h
#define i2c_ina219_h

#include <Adafruit_INA219.h>

#include "NmeaXDR.h"
#include "Nmea0183Msg.h"

#define INA219_I2C_ADDRESS_0 (0x40)
#define INA219_I2C_ADDRESS_1 (0x41)   // Bridge A0 (on addresses pad)
#define INA219_I2C_ADDRESS_2 (0x44)   // Bridge A1 (on addresses pad)
#define INA219_I2C_ADDRESS_3 (0x45)   // Bridge A0 and A1 (on addresses pad)

Adafruit_INA219 ina219_0(INA219_I2C_ADDRESS_0);
Adafruit_INA219 ina219_1(INA219_I2C_ADDRESS_1);
Adafruit_INA219 ina219_2(INA219_I2C_ADDRESS_2);
Adafruit_INA219 ina219_3(INA219_I2C_ADDRESS_3);

Adafruit_INA219 ina219_alt_0(INA219_I2C_ADDRESS_0);
Adafruit_INA219 ina219_alt_1(INA219_I2C_ADDRESS_1);
Adafruit_INA219 ina219_alt_2(INA219_I2C_ADDRESS_2);
Adafruit_INA219 ina219_alt_3(INA219_I2C_ADDRESS_3);

uint8_t ina219_addr[] = {INA219_I2C_ADDRESS_0, INA219_I2C_ADDRESS_1, INA219_I2C_ADDRESS_2, INA219_I2C_ADDRESS_3};

void i2c_ina219_report(Adafruit_INA219 *ina219, int bus, int index) {
  // Read voltage and current from INA219.
  float shuntvoltage = ina219->getShuntVoltage_mV();
  float busvoltage = ina219->getBusVoltage_V();

  // Compute load voltage
  float loadvoltage = busvoltage + (shuntvoltage / 1000);
  gen_nmea0183_xdr_3("$BBXDR,U,%.3f,V,VOLT_INA219_%d_%d", loadvoltage, bus, index);

  float current_mA = ina219->getCurrent_mA();
  if (!isnan(current_mA)) {
    gen_nmea0183_xdr_3("$BBXDR,I,%.3f,A,AMPS_INA219_%d_%d", current_mA / 1000, bus, index);
    // Compute power
    float power_mW = loadvoltage * current_mA;
    gen_nmea0183_xdr_3("$BBXDR,W,%.3f,W,WATT_INA219_%d_%d", power_mW / 1000, bus, index);
    if (fabs(current_mA) > 0.5) {
      // Compute resistance
      float resistance = fabs(loadvoltage * 1000 / current_mA);
      gen_nmea0183_xdr_3("$BBXDR,G,%.3f,O,OHMS_INA219_%d_%d", resistance, bus, index);
    }
  }
}

bool i2c_ina219_begin(Adafruit_INA219 *ina219, TwoWire *wire) {
  bool found = ina219->begin(wire);
  int index = 0;
  int bus = (wire == &Wire1) ? 1 : 0;
  if (bus == 1) {
    if (ina219 == &ina219_alt_1) {
      index = 1;
    } else if (ina219 == &ina219_alt_2) {
      index = 2;
    } else if (ina219 == &ina219_alt_3) {
      index = 3;
    }
  } else {
    if (ina219 == &ina219_1) {
      index = 1;
    } else if (ina219 == &ina219_2) {
      index = 2;
    } else if (ina219 == &ina219_3) {
      index = 3;
    }
  }
  if (found) {
    gen_nmea0183_msg("$BBTXT,01,01,01,ELECTRICAL found ina219 sensor at address=0x%s",
      (String(ina219_addr[index], HEX) + String((bus == 0 ? " main bus" : " alt bus"))).c_str());
    /*
      By default the INA219 will be calibrated with a range of 32V, 2A.
      However uncomment one of the below to change the range.  A smaller
      range can't measure as large of values but will measure with slightly
      better precision.
    */
    //ina219->setCalibration_32V_1A();
    if (index == 3) {  // calibrate 0x45 one differently for other use cases
      ina219->setCalibration_16V_400mA();
    }
    app.onRepeat(2000, [ina219, bus, index]() {
      i2c_ina219_report(ina219, bus, index);
    });
  }
  return found;
}

bool i2c_ina219_try_init(TwoWire *wire) {
  bool found = false;
  if (wire == &Wire1) {
    found |= i2c_ina219_begin(&ina219_alt_0, &Wire1);
    found |= i2c_ina219_begin(&ina219_alt_1, &Wire1);
    found |= i2c_ina219_begin(&ina219_alt_2, &Wire1);
    found |= i2c_ina219_begin(&ina219_alt_3, &Wire1);
  } else {
    found |= i2c_ina219_begin(&ina219_0, &Wire); // conflicts with M5Stack Encoder Unit on 0x40 on i2c0
    found |= i2c_ina219_begin(&ina219_1, &Wire);
    found |= i2c_ina219_begin(&ina219_2, &Wire);
    found |= i2c_ina219_begin(&ina219_3, &Wire);
  }
  return found;
}

#endif
