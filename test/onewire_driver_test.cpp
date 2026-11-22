#include "gtest/gtest.h"
#include "hal/gpio_ll.h"
#include "roo_onewire/internal/OneWire.h"

gpio_dev_t GPIO;

void pinMode(uint8_t pin, uint8_t mode) {
  EXPECT_EQ(mode, INPUT);
  GPIO.last_pin = static_cast<gpio_num_t>(pin);
  GPIO.output_enabled = false;
}

void delayMicroseconds(unsigned int) {}

namespace roo_onewire::internal {
namespace {

// Verifies that a GPIO accepted by the target can send and receive slots even
// when its number exceeds the original ESP32 output limit.
TEST(OneWireDriverTest, UsesTargetOutputCapabilityAboveGpio33) {
  constexpr uint8_t kPin = 40;
  GPIO = {};
  OneWire bus(kPin);
  EXPECT_EQ(GPIO.last_pin, kPin);

  bus.write(0x44, true);
  EXPECT_EQ(GPIO.last_pin, kPin);
  EXPECT_TRUE(GPIO.output_enabled);
  EXPECT_TRUE(GPIO.output_level);
  bus.depower();
  EXPECT_FALSE(GPIO.output_enabled);

  GPIO.input_level = false;
  EXPECT_EQ(bus.read_bit(), 0);
  GPIO.input_level = true;
  EXPECT_EQ(bus.read_bit(), 1);
  EXPECT_EQ(GPIO.last_pin, kPin);
}

// Verifies that an output-incapable pin never reaches the GPIO backend.
TEST(OneWireDriverTest, RejectsInputOnlyPins) {
  GPIO = {};
  OneWire bus(34);
  EXPECT_EQ(bus.reset(), 0);
  bus.write(0x44, true);
  EXPECT_EQ(GPIO.last_pin, GPIO_NUM_MAX);
  EXPECT_FALSE(GPIO.output_enabled);
}

}  // namespace
}  // namespace roo_onewire::internal
