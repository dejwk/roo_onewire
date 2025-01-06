#include "roo_onewire/device_family.h"

namespace roo_onewire {

roo_logging::Stream& operator<<(roo_logging::Stream& os, DeviceFamily family) {
  switch (family) {
    case DEVICE_FAMILY_DS18S20: {
      os << "DS18S20";
      break;
    }
    case DEVICE_FAMILY_DS18B20: {
      os << "DS18B20";
      break;
    }
    case DEVICE_FAMILY_DS1822: {
      os << "DS1822";
      break;
    }
    case DEVICE_FAMILY_DS1825: {
      os << "DS1825";
      break;
    }
    case DEVICE_FAMILY_MAX31850: {
      os << "MAX31850";
      break;
    }
    case DEVICE_FAMILY_DS28EA00: {
      os << "DS28EA00";
      break;
    }
    case DEVICE_FAMILY_BROADCAST: {
      os << "BROADCAST";
      break;
    }
    default: {
      os << "UNKNOWN";
      break;
    }
  }
  return os;
}

}  // namespace roo_onewire