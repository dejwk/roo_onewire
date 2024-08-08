#include "thermometer_role_store_arduino_prefs.h"

#include <cstdio>

namespace roo_onewire {

namespace {

void id2key(int id, char* key) { sprintf(key, "r_%d", id); }

}  // namespace

RomCode ArduinoPreferencesThermometerRoleStore::getRomCode(int id) {
  roo_prefs::Transaction t(collection_, true);
  char key[16];
  id2key(id, key);
  return RomCode(t.store().getULong64(key, 0));
}

void ArduinoPreferencesThermometerRoleStore::setRomCode(int id,
                                                        RomCode rom_code) {
  roo_prefs::Transaction t(collection_);
  char key[16];
  id2key(id, key);
  t.store().putULong64(key, rom_code.raw());
}

void ArduinoPreferencesThermometerRoleStore::clearRomCode(int id) {
  roo_prefs::Transaction t(collection_);
  char key[16];
  id2key(id, key);
  t.store().remove(key);
}

}  // namespace roo_onewire
