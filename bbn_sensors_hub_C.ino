#include <stdio.h>
#include <driver/pulse_cnt.h>
#include <driver/ledc.h>
#include <driver/gpio.h>
#include <esp_timer.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include "PulseGenerator.h"

// Configuration
#define PCNT_H_LIMIT 10000          // Upper limit for the pulse counter
#define PCNT_L_LIMIT -1             // Lower limit for the pulse counter
#define PCNT_INPUT_GPIO 8           // GPIO pin for pulse input/output
#define DEBOUNCE_TIME_NS 40         // Debounce time in nanoseconds
#define PULSES_PER_REVOLUTION 2     // Number of pulses per engine revolution (PPR)
#define MEASUREMENT_INTERVAL_MS 250 // Measurement interval in milliseconds

#define LEDC_OUTPUT_GPIO 7
#define LEDC_FREQUENCY 100                     // Initial frequency in Hz

// Variables
int64_t last_time = 0;
int16_t last_count = 0;

pcnt_unit_handle_t pcnt_unit = NULL;

// Function to initialize the PCNT module
void pcnt_init() {
  // Configure PCNT unit
  pcnt_unit_config_t unit_config = {
    .low_limit = PCNT_L_LIMIT,
    .high_limit = PCNT_H_LIMIT,
  };
  ESP_ERROR_CHECK(pcnt_new_unit(&unit_config, &pcnt_unit));

  // Configure PCNT channel
  pcnt_chan_config_t chan_config = {
    .edge_gpio_num = PCNT_INPUT_GPIO,
    .level_gpio_num = -1, // No level GPIO
  };
  pcnt_channel_handle_t pcnt_chan = NULL;
  ESP_ERROR_CHECK(pcnt_new_channel(pcnt_unit, &chan_config, &pcnt_chan));

  // Set edge and level actions
  ESP_ERROR_CHECK(pcnt_channel_set_edge_action(pcnt_chan, PCNT_CHANNEL_EDGE_ACTION_INCREASE, PCNT_CHANNEL_EDGE_ACTION_HOLD));
  ESP_ERROR_CHECK(pcnt_channel_set_level_action(pcnt_chan, PCNT_CHANNEL_LEVEL_ACTION_KEEP, PCNT_CHANNEL_LEVEL_ACTION_KEEP));

  // Set debounce filter
  pcnt_glitch_filter_config_t filter_config = {
    .max_glitch_ns = DEBOUNCE_TIME_NS,
  };
  ESP_ERROR_CHECK(pcnt_unit_set_glitch_filter(pcnt_unit, &filter_config));

  ESP_ERROR_CHECK(pcnt_unit_enable(pcnt_unit));
  ESP_ERROR_CHECK(pcnt_unit_clear_count(pcnt_unit));
  ESP_ERROR_CHECK(pcnt_unit_start(pcnt_unit));
}

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

