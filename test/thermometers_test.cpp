#include "gtest/gtest.h"
#include "roo_onewire.h"
#include "roo_scheduler.h"
#include "roo_testing/buses/onewire/fake_onewire.h"
#include "roo_testing/devices/onewire/thermometer/thermometer.h"
#include "roo_testing/microcontrollers/esp32/fake_esp32.h"
#include "roo_testing/system/timer.h"
#include "roo_testing/transducers/temperature/temperature.h"

namespace roo_onewire {
namespace {

class TestThermometer : public FakeOneWireThermometer {
 public:
  using FakeOneWireThermometer::FakeOneWireThermometer;

  void reset() override {
    ++reset_count;
    FakeOneWireThermometer::reset();
  }

  void start_function_command() override {
    if (function_command() == 0x44) ++conversion_count;
    FakeOneWireThermometer::start_function_command();
  }

  bool read_function_bit() override {
    bool bit = FakeOneWireThermometer::read_function_bit();
    if (function_command() != 0xBE) return bit;
    if (zero_scratchpad) return false;
    return corrupt_crc && bits_sent() == 71 ? !bit : bit;
  }

  int reset_count = 0;
  int conversion_count = 0;
  bool zero_scratchpad = false;
  bool corrupt_crc = false;
};

// Verifies the initial power state, discovery, and the scheduled reading of a
// parasite-powered DS18B20.
TEST(ThermometersTest, DetectsParasitePowerAndCompletesConversion) {
  constexpr int kPin = 18;
  constexpr char kRom[] = "28FF2A4C30180207";
  constexpr double kTemperatureC = -8.25;
  static const roo_testing_transducers::FixedThermometer sensor(
      roo_testing_transducers::Temperature::FromC(kTemperatureC));
  static TestThermometer* const device =
      new TestThermometer(kRom, sensor, FakeOneWireThermometer::POWER_PARASITE);
  // The fake board retains the bus pointer for its lifetime.
  static FakeOneWireInterface bus({device});
  FakeEsp32().attachOneWireBus(bus, kPin);

  const int previous_conversions = device->conversion_count;
  int conversions = 0;
  Thermometers::ConversionListener listener([&conversions] { ++conversions; });
  roo_scheduler::SchedulingService scheduler;
  OneWire onewire(scheduler);
  Thermometers& thermometers = onewire.thermometers();
  EXPECT_FALSE(thermometers.isParasite());

  onewire.begin(kPin);
  thermometers.addEventListener(&listener);
  ASSERT_TRUE(onewire.update());
  EXPECT_TRUE(thermometers.isParasite());
  EXPECT_TRUE(thermometers.isConversionPending());
  ASSERT_EQ(thermometers.count(), 1);
  EXPECT_EQ(thermometers.rom_code(0), RomCode::FromString(kRom));
  EXPECT_TRUE(thermometers.thermometer(0).temperature().isUnknown());

  system_time_delay_micros(700000);
  scheduler.executeEligibleTasksUpToNow();
  EXPECT_TRUE(thermometers.isConversionPending());
  EXPECT_EQ(conversions, 0);

  const int resets_before_reading = device->reset_count;
  system_time_delay_micros(50000);
  scheduler.executeEligibleTasksUpToNow();
  EXPECT_FALSE(thermometers.isConversionPending());
  EXPECT_EQ(conversions, 1);
  EXPECT_EQ(device->reset_count, resets_before_reading + 1);
  EXPECT_EQ(device->conversion_count, previous_conversions + 1);
  EXPECT_DOUBLE_EQ(thermometers.thermometer(0).temperature().degCelcius(),
                   kTemperatureC);
}

// Verifies that invalid scratchpads are rejected and presence alone never
// triggers a conversion.
TEST(ThermometersTest, RejectsInvalidScratchpads) {
  constexpr int kPin = 19;
  static const roo_testing_transducers::FixedThermometer sensor(
      roo_testing_transducers::Temperature::FromC(23.5));
  static TestThermometer* const all_zero = new TestThermometer(
      "28FF2A4C30180207", sensor, FakeOneWireThermometer::POWER_PARASITE);
  static TestThermometer* const bad_crc =
      new TestThermometer("28FFA585301801EB", sensor);
  static FakeOneWireInterface bus({all_zero, bad_crc});
  all_zero->zero_scratchpad = true;
  bad_crc->corrupt_crc = true;
  FakeEsp32().attachOneWireBus(bus, kPin);

  roo_scheduler::SchedulingService scheduler;
  OneWire onewire(scheduler);
  onewire.begin(kPin);
  EXPECT_FALSE(onewire.update());
  EXPECT_TRUE(onewire.thermometers().isParasite());
  EXPECT_FALSE(onewire.thermometers().isConversionPending());
  EXPECT_EQ(onewire.thermometers().count(), 0);
  EXPECT_EQ(all_zero->conversion_count, 0);
  EXPECT_EQ(bad_crc->conversion_count, 0);
}

}  // namespace
}  // namespace roo_onewire
