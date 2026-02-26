#pragma once

#include <Arduino.h>
#include <inttypes.h>

#include "roo_collections/flat_small_hash_set.h"
#include "roo_logging.h"
#include "roo_onewire/device_family.h"

namespace roo_onewire {

/// Raw 8-byte device address used on the OneWire bus.
using OneWireDeviceAddress = uint8_t[8];

/// Identifies a device on the bus. Can be passed by value.
class RomCode {
 public:
  constexpr RomCode(uint64_t code) : rom_code_(code) {}
  constexpr RomCode() : rom_code_(0) {}

  /// Parses a 16-character hex string into a rom code.
  static RomCode FromString(const char *str);

  /// Returns true when this is the broadcast rom code.
  bool isBroadcast() const { return rom_code_ == 0xFFFFFFFFFFFFFFFFLL; }

  /// Returns true when this is the zero/unknown rom code.
  bool isUnknown() const { return rom_code_ == 0; }

  /// Returns true if this is a valid, non-broadcast rom code.
  bool isValidUnicast() const;

  /// Returns the device family byte (LSB).
  uint8_t getFamily() const { return rom_code_ & 0xFF; }

  bool operator==(const RomCode &other) const {
    return rom_code_ == other.rom_code_;
  }

  bool operator!=(const RomCode &other) const {
    return rom_code_ != other.rom_code_;
  }

  bool operator<(const RomCode &other) const { return raw() < other.raw(); }

  /// Returns a hex string representation (16 characters).
  String toString() const;

  /// Writes exactly 16 hex characters into the output buffer.
  void toCharArray(char *out) const;

  /// Returns the raw 64-bit rom code.
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

/// Broadcast rom code used to address all devices.
static constexpr RomCode kBroadcastCode = RomCode(-1);

struct RomCodeHashFn {
  uint32_t operator()(const RomCode &val) const {
    return (val.rom_code_ >> 32) * 5 + val.rom_code_;
  }
};

/// Set of rom codes with a small-vector optimized hash table.
using RomCodeSet = roo_collections::FlatSmallHashSet<RomCode, RomCodeHashFn>;

/// Streams the rom code as a 16-character hex string.
inline roo_logging::Stream &operator<<(roo_logging::Stream &out,
                                       RomCode rom_code) {
  out.print(rom_code.toString());
  return out;
}

};  // namespace roo_onewire