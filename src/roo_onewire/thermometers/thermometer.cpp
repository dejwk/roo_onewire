#include "roo_onewire/thermometers/thermometer.h"

namespace roo_onewire {

Thermometer::Thermometer()
    : family_(DEVICE_FAMILY_UNKNOWN),
      resolution_(RESOLUTION_UNDEFINED),
      temperature_(roo_temperature::Unknown()) {}

roo_logging::Stream& operator<<(roo_logging::Stream& os, const Thermometer& t) {
  os << "{rom_code: " << t.rom_code() << ", family: " << t.family()
     << ", resolution: " << t.resolution()
     << ", temperature: " << t.temperature() << "}";
  return os;
}

}  // namespace roo_onewire