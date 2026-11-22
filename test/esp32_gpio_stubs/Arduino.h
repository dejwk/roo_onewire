#pragma once

#include <stdint.h>

#define IRAM_ATTR
#define PROGMEM
#define portMUX_INITIALIZER_UNLOCKED 0

using portMUX_TYPE = int;

constexpr uint8_t INPUT = 0;

inline void portENTER_CRITICAL(portMUX_TYPE*) {}
inline void portEXIT_CRITICAL(portMUX_TYPE*) {}

inline uint8_t pgm_read_byte(const uint8_t* address) { return *address; }

void pinMode(uint8_t pin, uint8_t mode);
void delayMicroseconds(unsigned int duration);
