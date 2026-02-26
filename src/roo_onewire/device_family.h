#pragma once

#include "roo_logging.h"

namespace roo_onewire {

/// Known OneWire device families supported by this library.
enum DeviceFamily {
  DEVICE_FAMILY_UNKNOWN,
  DEVICE_FAMILY_DS18S20,  // also covers DS1820.
  DEVICE_FAMILY_DS18B20,  // also covers MAX31820.
  DEVICE_FAMILY_DS1822,
  DEVICE_FAMILY_DS1825,
  DEVICE_FAMILY_MAX31850,
  DEVICE_FAMILY_DS28EA00,
  DEVICE_FAMILY_BROADCAST,
};

/// Streams a human-readable device family label.
roo_logging::Stream& operator<<(roo_logging::Stream& os, DeviceFamily family);

}