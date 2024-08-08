#pragma once

#include "roo_onewire/thermometers/hal/thermometer_role_store.h"
#include "roo_prefs.h"

namespace roo_onewire {

class ArduinoPreferencesThermometerRoleStore : public ThermometerRoleStore {
 public:
  ArduinoPreferencesThermometerRoleStore() : collection_("roo/1w/roles") {}

  roo_onewire::RomCode getRomCode(int id) override;
  void setRomCode(int id, roo_onewire::RomCode rom_code) override;
  void clearRomCode(int id) override;

 private:
  roo_prefs::Collection collection_;
};

}  // namespace roo_onewire
