#pragma once

#include "driver/gpio.h"

struct gpio_dev_t {
  gpio_num_t last_pin = GPIO_NUM_MAX;
  int release_count = 0;
  bool output_enabled = false;
  bool output_level = false;
  bool input_level = true;
};

extern gpio_dev_t GPIO;

inline void gpio_ll_output_enable(gpio_dev_t* gpio, gpio_num_t pin) {
  gpio->last_pin = pin;
  gpio->output_enabled = true;
}

inline void gpio_ll_output_disable(gpio_dev_t* gpio, gpio_num_t pin) {
  gpio->last_pin = pin;
  ++gpio->release_count;
  gpio->output_enabled = false;
}

inline void gpio_ll_set_level(gpio_dev_t* gpio, gpio_num_t pin,
                              uint32_t level) {
  gpio->last_pin = pin;
  gpio->output_level = (level != 0);
}

inline int gpio_ll_get_level(gpio_dev_t* gpio, gpio_num_t pin) {
  gpio->last_pin = pin;
  return gpio->input_level;
}
