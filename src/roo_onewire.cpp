#include "roo_onewire.h"

#include "roo_onewire/rom_code.h"

static const uint8_t kReadPowerSupply = 0xB4;

#ifdef ROO_TESTING
#include "roo_testing/devices/microcontroller/esp32/fake_esp32.h"
#endif
namespace roo_onewire {

namespace {

bool IsThermometerFamilySupported(uint8_t family) {
  return family == 0x10 || family == 0x28 || family == 0x22 || family == 0x3B ||
         family == 0x42;
}

}  // namespace

#ifdef ROO_TESTING

namespace {
FakeOneWireInterface* findOrFail(uint8_t pin) {
  auto itr = FakeEsp32().onewire_buses().find(pin);
  CHECK(itr != FakeEsp32().onewire_buses().end())
      << "No OneWire bus on pin " << (int)pin;
  return itr->second;
}
}  // namespace

OneWire::OneWire(uint8_t pin, roo_scheduler::Scheduler& scheduler)
    : onewire_(findOrFail(pin)), thermometers_(*this, scheduler) {}
#else
OneWire::OneWire(uint8_t pin, roo_scheduler::Scheduler& scheduler)
    : onewire_(pin), thermometers_(*this, scheduler) {}
#endif

RomCodeSet OneWire::discoverAll() {
  RomCodeSet result(thermometers_.count() > 0 ? thermometers_.count() : 8);
  onewire_.reset_search();
  OneWireDeviceAddress addr;
  while (onewire_.search(addr)) {
    RomCode rom_code(addr);
    if (rom_code.isValidUnicast() &&
        IsThermometerFamilySupported(rom_code.getFamily())) {
      result.insert(rom_code);
    }
  }
  return result;
}

bool OneWire::update() { return thermometers_.update(); }

}  // namespace roo_onewire
