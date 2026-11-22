#pragma once

#include <stdint.h>

enum gpio_num_t : uint8_t { GPIO_NUM_MAX = 49 };

inline bool GPIO_IS_VALID_OUTPUT_GPIO(int gpio) {
  return (gpio >= 0 && gpio < 34) || gpio == 40;
}
