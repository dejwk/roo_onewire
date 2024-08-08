#pragma once

#include <Arduino.h>
#include <inttypes.h>

#include "roo_collections/flat_small_hash_set.h"
#include "roo_logging.h"
#include "roo_onewire/device_family.h"

#ifdef ROO_TESTING

#include "roo_testing/buses/onewire/OneWire.h"
#include "roo_testing/buses/onewire/fake_onewire.h"

#else

#include <OneWire.h>

#endif

namespace roo_onewire {

using OneWireDeviceAddress = uint8_t[8];

// Identifies a device on the bus. Can be passed by value.
class RomCode {
 public:
  constexpr RomCode(uint64_t code) : rom_code_(code) {}
  constexpr RomCode() : rom_code_(0) {}

  bool isBroadcast() const { return rom_code_ == 0xFFFFFFFFFFFFFFFFLL; }

  bool isUnknown() const { return rom_code_ == 0; }

  bool isValidUnicast() const;

  uint8_t getFamily() const { return rom_code_ & 0xFF; }

  bool operator==(const RomCode &other) const {
    return rom_code_ == other.rom_code_;
  }

  bool operator!=(const RomCode &other) const {
    return rom_code_ != other.rom_code_;
  }

  bool operator<(const RomCode &other) const { return raw() < other.raw(); }

  String toString() const;

  // Writes exactly 16 characters into the array.
  void toCharArray(char *out) const;

  uint64_t raw() const { return rom_code_; }

 private:
  friend class OneWire;
  friend class Thermometers;
  friend struct RomCodeHashFn;

  friend class BusMaster;

  RomCode(const OneWireDeviceAddress &addr);

  void toOneWireDeviceAddress(OneWireDeviceAddress &addr) const;

  uint64_t rom_code_;
};

static constexpr RomCode kBroadcastCode = RomCode(-1);

struct RomCodeHashFn {
  uint32_t operator()(const RomCode &val) const {
    return (val.rom_code_ >> 32) * 5 + val.rom_code_;
  }
};

using RomCodeSet = roo_collections::FlatSmallHashSet<RomCode, RomCodeHashFn>;

inline roo_logging::Stream &operator<<(roo_logging::Stream &out,
                                       RomCode rom_code) {
  out.print(rom_code.toString());
  return out;
}

};  // namespace roo_onewire