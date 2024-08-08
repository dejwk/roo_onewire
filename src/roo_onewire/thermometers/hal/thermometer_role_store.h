#pragma once

#include "roo_onewire/rom_code.h"

namespace roo_onewire {

// Stores (e.g. in Preferences) the mapping from IDs to rom codes.
class ThermometerRoleStore {
 public:
  virtual RomCode getRomCode(int id) = 0;
  virtual void setRomCode(int id, RomCode rom_code) = 0;
  virtual void clearRomCode(int id) = 0;
};

}  // namespace roo_onewire
