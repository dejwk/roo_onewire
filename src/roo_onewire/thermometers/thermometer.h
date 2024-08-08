#pragma once

#include <inttypes.h>

#include "roo_logging.h"
#include "roo_onewire/device_family.h"
#include "roo_onewire/rom_code.h"
#include "roo_onewire/thermometers/resolution.h"
#include "roo_temperature.h"

namespace roo_onewire {

class Thermometer {
 public:
  Thermometer();

  const RomCode& rom_code() const { return rom_code_; }
  DeviceFamily family() const { return family_; }
  Resolution resolution() const { return resolution_; }
  roo_temperature::Temperature temperature() const { return temperature_; }

 private:
  friend class Thermometers;

  void set(RomCode rom_code, DeviceFamily family, Resolution resolution,
           roo_temperature::Temperature temperature) {
    rom_code_ = rom_code;
    family_ = family;
    resolution_ = resolution;
    temperature_ = temperature;
  }

  RomCode rom_code_;
  DeviceFamily family_;
  Resolution resolution_;
  roo_temperature::Temperature temperature_;
};

roo_logging::Stream& operator<<(roo_logging::Stream& os, const Thermometer& t);

}  // namespace roo_onewire