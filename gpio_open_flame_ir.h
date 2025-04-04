#ifndef gpio_open_flame_ir_h
#define gpio_open_flame_ir_h

const int open_flame_sensorPin = 2; 
int64_t last_flame_time = 0;

#define FLAME_COUNT_FOR_TRIGGER 1

void gpio_open_flame_ir_report() {
  bool flame = false;
  for (int i = 0; i < FLAME_COUNT_FOR_TRIGGER; i++) {
    int sensorValue = digitalRead(open_flame_sensorPin); // Read the sensor value
    if (sensorValue == LOW) { // If the sensor detects a flame (LOW)
      flame = true;
    } else {
      flame = false;
      break;
    }
    delay(3);
  }

  if (flame) { 
    int64_t now = esp_timer_get_time();
    if ((now - last_flame_time) > 1000000) {
      gen_nmea0183_xdr("$BBXDR,S,1,,OPEN_FLAME", 1);
      last_flame_time = now;
    }
  }
}

void gpio_open_flame_ir_try_init() {
  pinMode(open_flame_sensorPin, INPUT);
  app.onRepeat(20, []() {
    gpio_open_flame_ir_report();
  });
}

#endif
