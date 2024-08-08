#pragma once

#include <string>

#include "roo_temperature.h"
#include "roo_onewire/rom_code.h"
#include "roo_logging.h"

namespace roo_onewire {

// Abstract, application-defined role. Can be assigned a real, 'discovered'
// thermometer.
class ThermometerRole : public roo_temperature::Thermometer {
 public:
  ThermometerRole(int id, std::string name)
      : id_(id), name_(name), rom_code_() {}

  int id() const { return id_; }
  const std::string& name() const { return name_; }
  RomCode rom_code() const { return rom_code_; }

  bool isAssigned() const { return !rom_code_.isUnknown(); }

  roo_temperature::Thermometer::Reading readTemperature() const override {
    return last_reading_;
  }

 private:
  friend class ThermometerRoles;

  void assign(RomCode rom_code) { rom_code_ = rom_code; }

  void unassign() { rom_code_ = RomCode(); }

  void setLastReading(roo_temperature::Temperature reading,
                      roo_time::Uptime time) {
    last_reading_.value = reading;
    last_reading_.time = time;
  }

  int id_;
  std::string name_;
  RomCode rom_code_;

  roo_temperature::Thermometer::Reading last_reading_;
};

roo_logging::Stream& operator<<(roo_logging::Stream& out, const ThermometerRole& role);

}  // namespace roo_onewire