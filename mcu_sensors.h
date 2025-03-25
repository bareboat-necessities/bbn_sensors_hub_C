#ifndef mcu_sensors_h
#define mcu_sensors_h

#include "i2c_qmp6988.h"
#include "i2c_bmp280.h"
#include "i2c_bme680.h"
#include "i2c_dht12.h"
#include "i2c_ads1115.h"
#include "i2c_bh1750fvi_tr.h"
#include "i2c_ain_4_20ma.h"
#include "i2c_sgp30.h"
#include "i2c_vl53l0x.h"
#include "i2c_ina219.h"

#include "gpio_open_flame_ir.h"
#include "gpio_RPM_sensor.h"
#include "gpio_max6675.h"
#include "gpio_240_30_ohm_sensor.h"

void mcu_sensors_scan() {
  i2c_ina219_try_init(&Wire1);
  i2c_ads1115_try_init(&Wire1, G38, G39, 100000UL);
  i2c_ain_4_20ma_try_init(&Wire1, G38, G39, 100000UL);
  i2c_bmp280_try_init();
  i2c_qmp6988_try_init();
  i2c_bme680_try_init();
  i2c_dht12_try_init();
  i2c_sgp30_try_init();
  i2c_bh1750fvi_tr_try_init();
  i2c_vl53l0x_try_init();

  gpio_open_flame_ir_try_init();
  gpio_RPM_sensor_try_init();
  gpio_240_30_ohm_try_init();
  gpio_max6675_try_init();
}

void mcu_sensors_update() {
}

#endif
