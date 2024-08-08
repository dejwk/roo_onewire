#pragma once

#include "roo_onewire/thermometers/hal/arduino_prefs/thermometer_role_store_arduino_prefs.h"
#include "roo_onewire/thermometers/hal/thermometer_role_store.h"

namespace roo_onewire {

ThermometerRoleStore& DefaultStore();

}  // namespace roo_onewire