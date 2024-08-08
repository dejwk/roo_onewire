#include "roo_onewire/thermometers/thermometer_role.h"

namespace roo_onewire {

roo_logging::Stream& operator<<(roo_logging::Stream& os,
                                const ThermometerRole& role) {
  os << "{" << role.id() << ":" << role.name() << ", ";
  if (role.isAssigned()) {
    os << role.rom_code();
    if (!role.readTemperature().value.isUnknown()) {
      os << ", " << role.readTemperature().value;
    }
  } else {
    os << "<unassigned>";
  }
  os << "}";
  return os;
}

}  // namespace roo_onewire