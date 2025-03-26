#ifndef gpio_RPM_sensor_h
#define gpio_RPM_sensor_h

#include <stdio.h>
#include <driver/gpio.h>
#include <esp_timer.h>

#include "PulseCounter.h"
#include "PulseGenerator.h"

// Configuration
#define PCNT_INPUT_GPIO 8             // GPIO pin for pulse input
#define DEBOUNCE_TIME_NS 10000        // Debounce time in nanoseconds
#define PULSES_PER_REVOLUTION 3       // Number of pulses per engine revolution (PPR)
#define MEASUREMENT_INTERVAL_MS 500   // Measurement interval in milliseconds

#define LEDC_OUTPUT_GPIO 2            // GPIO pin for pulse output
#define LEDC_FREQUENCY 100            // Initial frequency in Hz

const bool gen_test_pulses = false;

// Variables
int64_t last_pulse_time = 0;
int16_t last_pulse_count = 0;

// Function to calculate RPM
float calculate_rpm() {
  int count = 0;
  ESP_ERROR_CHECK(pcnt_unit_get_count(pcnt_unit, &count));

  int64_t current_time = esp_timer_get_time();
  int64_t time_elapsed = current_time - last_pulse_time; // Time elapsed in microseconds

  // Calculate RPM
  float pulses_per_second = (count - last_pulse_count) / (time_elapsed / 1000000.0);
  float rpm = (pulses_per_second / PULSES_PER_REVOLUTION) * 60; // Convert to RPM

  // Update last count and time
  last_pulse_count = count;
  last_pulse_time = current_time;

  return rpm;
}

void gpio_RPM_sensor_report() {
  // Calculate and print the RPM
  float rpm = calculate_rpm();
  if (rpm >= 0) {
    gen_nmea0183_xdr("$BBXDR,T,%.1f,R,RPM", rpm);

    if (gen_test_pulses) {
      // Get the generated frequency from LEDC
      uint32_t generated_freq = ledc_get_freq(LEDC_MODE, LEDC_TIMER);
      float generated_rpm = (generated_freq / PULSES_PER_REVOLUTION) * 60; // Convert to RPM
      printf("Generated RPM: %.2f\n", generated_rpm);
  
      // Compare generated and measured RPM
      float error = fabs(generated_rpm - rpm);
      printf("Error: %.2f RPM\n", error);
    }
  }
}

void gpio_RPM_sensor_try_init() {
  // Initialize PCNT
  pcnt_init(PCNT_INPUT_GPIO, DEBOUNCE_TIME_NS);
  pinMode(PCNT_INPUT_GPIO, INPUT);

  if (gen_test_pulses) {
    // Initialize LEDC (PWM)
    ledc_init(LEDC_OUTPUT_GPIO, LEDC_FREQUENCY);    
    pinMode(LEDC_OUTPUT_GPIO, OUTPUT);
  }

  last_pulse_time = esp_timer_get_time();

  app.onRepeat(MEASUREMENT_INTERVAL_MS, []() {
    gpio_RPM_sensor_report();
  });
}

#endif
