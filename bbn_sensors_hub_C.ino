#include <stdio.h>
#include <driver/gpio.h>
#include <esp_timer.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <ReactESP.h>  // https://github.com/mairas/ReactESP

#include "NmeaXDR.h"
#include "Nmea0183Msg.h"

#include "ResistanceSensor.h"
#include "Max6675.h"

// Configuration
#define PCNT_INPUT_GPIO 8           // GPIO pin for pulse input/output
#define DEBOUNCE_TIME_NS 40         // Debounce time in nanoseconds
#define PULSES_PER_REVOLUTION 2     // Number of pulses per engine revolution (PPR)
#define MEASUREMENT_INTERVAL_MS 250 // Measurement interval in milliseconds

#include "PulseCounter.h"

#define LEDC_OUTPUT_GPIO 7
#define LEDC_FREQUENCY 100                     // Initial frequency in Hz

#include "PulseGenerator.h"

// Variables
int64_t last_time = 0;
int16_t last_count = 0;

using namespace reactesp;
ReactESP app;

// Function to calculate RPM
float calculate_rpm() {
  int count = 0;
  ESP_ERROR_CHECK(pcnt_unit_get_count(pcnt_unit, &count));

  int64_t current_time = esp_timer_get_time();
  int64_t time_elapsed = current_time - last_time; // Time elapsed in microseconds

  // Calculate RPM
  float pulses_per_second = (count - last_count) / (time_elapsed / 1000000.0);
  float rpm = (pulses_per_second / PULSES_PER_REVOLUTION) * 60; // Convert to RPM

  // Update last count and time
  last_count = count;
  last_time = current_time;

  return rpm;
}

void setup() {

  // Initialize PCNT
  pcnt_init();

  // Initialize LEDC (PWM)
  ledc_init();

  //pinMode(PCNT_INPUT_GPIO, INPUT);
  //pinMode(LEDC_OUTPUT_GPIO, OUTPUT);

  last_time = esp_timer_get_time();
}

void loop() {
  // Delay for the measurement interval
  vTaskDelay(pdMS_TO_TICKS(MEASUREMENT_INTERVAL_MS));

  // Calculate and print the RPM
  float rpm = calculate_rpm();
  if (rpm > 0) {
    printf("Measured RPM: %.2f\n", rpm);

    // Get the generated frequency from LEDC
    uint32_t generated_freq = ledc_get_freq(LEDC_MODE, LEDC_TIMER);
    float generated_rpm = (generated_freq / PULSES_PER_REVOLUTION) * 60; // Convert to RPM
    printf("Generated RPM: %.2f\n", generated_rpm);

    // Compare generated and measured RPM
    float error = fabs(generated_rpm - rpm);
    printf("Error: %.2f RPM\n", error);
  }
}

