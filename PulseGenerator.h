#ifndef PulseGenerator_h
#define PulseGenerator_h

// LEDC (PWM) Configuration
#define LEDC_TIMER LEDC_TIMER_0
#define LEDC_MODE LEDC_LOW_SPEED_MODE
#define LEDC_CHANNEL LEDC_CHANNEL_0
#define LEDC_DUTY_RESOLUTION LEDC_TIMER_13_BIT // 13-bit resolution for duty cycle

// Function to initialize the LEDC (PWM) module
void ledc_init() {
  // Configure LEDC timer
  ledc_timer_config_t ledc_timer = {
    .speed_mode = LEDC_MODE,
    .duty_resolution = LEDC_DUTY_RESOLUTION,
    .timer_num = LEDC_TIMER,
    .freq_hz = LEDC_FREQUENCY, // Initial frequency
    .clk_cfg = LEDC_AUTO_CLK,
  };
  ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));

  // Configure LEDC channel
  ledc_channel_config_t ledc_channel = {
    .gpio_num = LEDC_OUTPUT_GPIO,
    .speed_mode = LEDC_MODE,
    .channel = LEDC_CHANNEL,
    .intr_type = LEDC_INTR_DISABLE,
    .timer_sel = LEDC_TIMER,
    .duty = (1 << (LEDC_DUTY_RESOLUTION - 1)), // 50% duty cycle
    .hpoint = 0,
  };
  ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel));
}

#endif
