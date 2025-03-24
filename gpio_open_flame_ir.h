#ifndef gpio_open_flame_ir_h
#define gpio_open_flame_ir_h

const int open_flame_sensorPin = 2; 

void gpio_open_flame_ir_report() {
  int sensorValue = digitalRead(sensorPin); // Read the sensor value

  if (sensorValue == LOW) { // If the sensor detects a flame (LOW)
    Serial.println("Flame Detected!");
  } else {
     //Serial.println("No Flame Detected");
  }

  //gen_nmea0183_xdr("$BBXDR,G,%.1f,O,Ohms_esp32", resistanceSens.measured_resistance);
}

void gpio_open_flame_ir_try_init() {
  pinMode(open_flame_sensorPin, INPUT);
  app.onRepeat(30, []() {
    gpio_open_flame_ir_report();
  });
}

#endif

