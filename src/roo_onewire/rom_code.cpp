#include "roo_onewire/rom_code.h"

namespace roo_onewire {

namespace {

inline char NibbleToChar(uint8_t nibble) {
  return nibble >= 10 ? nibble - 10 + 'A' : nibble + '0';
}

inline bool CharToNibble(char ch, uint8_t &nibble) {
  if (ch >= '0' && ch <= '9') {
    nibble = ch - '0';
  } else if (ch >= 'A' && ch <= 'F') {
    nibble = ch - 'A' + 10;
  } else if (ch >= 'a' && ch <= 'f') {
    nibble = ch - 'a' + 10;
  } else {
    return false;
  }
  return true;
}

}  // namespace

bool RomCode::isValidUnicast() const {
  if (rom_code_ == 0 || rom_code_ == 0xFFFFFFFFFFFFFFFFLL) {
    return false;
  }
  OneWireDeviceAddress addr;
  toOneWireDeviceAddress(addr);
  return OneWire::crc8(addr, 7) == addr[7];
}

String RomCode::toString() const {
  String result;
  uint64_t val = rom_code_;
  for (int i = 0; i < 8; ++i) {
    uint8_t b = val & 0xFF;
    result += NibbleToChar(b >> 4);
    result += NibbleToChar(b & 15);
    val >>= 8;
  }
  return result;
}

RomCode RomCode::FromString(const char *str) {
  LOG(INFO) << str;
  uint64_t code = 0;
  for (size_t i = 0; i < 8; ++i) {
    uint8_t lo, hi;
    if (!CharToNibble(str[i * 2], hi) || !CharToNibble(str[i * 2 + 1], lo)) {
      return RomCode();
    }
    LOG(INFO) << (int)hi << ", " << (int)lo;
    code |= ((uint64_t)(hi << 4 | lo) << (8 * i));
  }
  return RomCode(code);
}

void RomCode::toCharArray(char *out) const {
  uint64_t val = rom_code_;
  for (int i = 0; i < 8; ++i) {
    uint8_t b = val & 0xFF;
    out[i * 2] = NibbleToChar(b >> 4);
    out[i * 2 + 1] = NibbleToChar(b & 15);
    val >>= 8;
  }
}

RomCode::RomCode(const OneWireDeviceAddress &addr)
    : rom_code_((((uint64_t)addr[0]) << 0) | ((uint64_t)(addr[1]) << 8) |
                ((uint64_t)(addr[2]) << 16) | ((uint64_t)(addr[3]) << 24) |
                ((uint64_t)(addr[4]) << 32) | ((uint64_t)(addr[5]) << 40) |
                ((uint64_t)(addr[6]) << 48) | ((uint64_t)(addr[7]) << 56)) {}

void RomCode::toOneWireDeviceAddress(OneWireDeviceAddress &addr) const {
  addr[0] = (rom_code_ >> 0);
  addr[1] = (rom_code_ >> 8);
  addr[2] = (rom_code_ >> 16);
  addr[3] = (rom_code_ >> 24);
  addr[4] = (rom_code_ >> 32);
  addr[5] = (rom_code_ >> 40);
  addr[6] = (rom_code_ >> 48);
  addr[7] = (rom_code_ >> 56);
}

}  // namespace roo_onewire