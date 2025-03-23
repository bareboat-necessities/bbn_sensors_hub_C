#ifndef PulseCounter_h
#define PulseCounter_h

#include <driver/pulse_cnt.h>
#include <driver/gpio.h>
#include <esp_timer.h>

// Configuration
#define PCNT_H_LIMIT 10000          // Upper limit for the pulse counter
#define PCNT_L_LIMIT -1             // Lower limit for the pulse counter

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

#endif
