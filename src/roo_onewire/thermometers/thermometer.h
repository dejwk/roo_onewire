#pragma once

#include <inttypes.h>

#include "roo_logging.h"
#include "roo_onewire/device_family.h"
#include "roo_onewire/rom_code.h"
#include "roo_onewire/thermometers/resolution.h"
#include "roo_quantity.h"
#include "roo_quantity/temperature.h"

namespace roo_onewire {

/// Snapshot of a thermometer reading and its metadata.
class Thermometer {
 public:
  /// Creates an empty thermometer instance.
  Thermometer();

  /// Returns the rom code identifying this device.
  const RomCode& rom_code() const { return rom_code_; }
  /// Returns the device family.
  DeviceFamily family() const { return family_; }
  /// Returns the reported resolution.
  Resolution resolution() const { return resolution_; }
  /// Returns the latest temperature reading.
  roo_quantity::Temperature temperature() const { return temperature_; }
  /// Returns the conversion time of the reading.
  roo_time::Uptime conversion_time() const { return conversion_time_; }

 private:
  friend class Thermometers;

  void set(RomCode rom_code, DeviceFamily family, Resolution resolution,
           roo_quantity::Temperature temperature,
           roo_time::Uptime conversion_time) {
    rom_code_ = rom_code;
    family_ = family;
    resolution_ = resolution;
    temperature_ = temperature;
    conversion_time_ = conversion_time;
  }

  RomCode rom_code_;
  DeviceFamily family_;
  Resolution resolution_;
  roo_quantity::Temperature temperature_;
  roo_time::Uptime conversion_time_;
};

/// Streams a human-readable thermometer summary.
roo_logging::Stream& operator<<(roo_logging::Stream& os, const Thermometer& t);

}  // namespace roo_onewire