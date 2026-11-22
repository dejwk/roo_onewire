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

// Verifies the initial power state, discovery, and the scheduled reading of a
// parasite-powered DS18B20.
TEST(ThermometersTest, DetectsParasitePowerAndCompletesConversion) {
  constexpr int kPin = 18;
  constexpr char kRom[] = "28FF2A4C30180207";
  constexpr double kTemperatureC = -8.25;
  static const roo_testing_transducers::FixedThermometer sensor(
      roo_testing_transducers::Temperature::FromC(kTemperatureC));
  // The fake board retains the bus pointer for its lifetime.
  static FakeOneWireInterface bus({
      new FakeOneWireThermometer(kRom, sensor,
                                 FakeOneWireThermometer::POWER_PARASITE),
  });
  FakeEsp32().attachOneWireBus(bus, kPin);

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

  system_time_delay_micros(50000);
  scheduler.executeEligibleTasksUpToNow();
  EXPECT_FALSE(thermometers.isConversionPending());
  EXPECT_EQ(conversions, 1);
  EXPECT_DOUBLE_EQ(thermometers.thermometer(0).temperature().degCelcius(),
                   kTemperatureC);
}

}  // namespace
}  // namespace roo_onewire
