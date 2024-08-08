#pragma once

#include "roo_onewire/bus.h"
#include "roo_onewire/rom_code.h"
#include "roo_onewire/thermometers.h"
#include "roo_scheduler.h"

namespace roo_onewire {

class OneWire {
 public:
  OneWire(uint8_t pin, roo_scheduler::Scheduler& scheduler);

  // Re-discovers devices on the bus, fetches their state, and requests
  // temperature conversion for thermometers. Returns true if the conversion
  // request has been issued; false otherwise (e.g. if no thermometers have been
  // identified on the bus.) If the conversion is already in progress,
  // immediately returns true.
  bool update();

  // Returns the collection of all thermometers that have been recently
  // discovered, along with their cached state (e.g. temperature readings.)
  Thermometers& thermometers() { return thermometers_; }

  // Const version of the above.
  const Thermometers& thermometers() const { return thermometers_; }

 private:
  friend class Thermometers;

  RomCodeSet discoverAll();

  void readPowerSupply();

  Bus& bus() { return onewire_; }

  // The bus.
  Bus onewire_;

  Thermometers thermometers_;
};

}  // namespace roo_onewire