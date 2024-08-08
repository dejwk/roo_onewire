#include "roo_onewire/thermometers/hal/defaults.h"

namespace roo_onewire {

ThermometerRoleStore& DefaultStore() {
  static ArduinoPreferencesThermometerRoleStore store;
  return store;
}

}