#include "roo_onewire/thermometers.h"

#include "roo_logging.h"
#include "roo_onewire.h"

using roo_temperature::Temperature;

using roo_time::Interval;
using roo_time::Millis;
using roo_time::Uptime;

namespace roo_onewire {

namespace {

static const uint8_t kReadRom = 0x33;
static const uint8_t kMatchRom = 0x55;
static const uint8_t kSkipRom = 0xCC;
static const uint8_t kAlarmSearch = 0xEC;
static const uint8_t kConvert = 0x44;
static const uint8_t kWriteScratchpad = 0x4E;
static const uint8_t kReadScratchpad = 0xBE;
static const uint8_t kCopyScratchpad = 0x48;
static const uint8_t kRecallEEPROM = 0xB8;
static const uint8_t kReadPowerSupply = 0xB4;

Resolution Read2BitResolution(const Scratchpad& scratchpad) {
  return (Resolution)(((scratchpad[4] >> 5) & 3) + 9);
}

struct TemperatureData {
  Resolution resolution;
  Temperature temperature;
};

TemperatureData ReadTemperatureData(DeviceFamily family,
                                    const Scratchpad& scratchpad) {
  switch (family) {
    case DEVICE_FAMILY_DS18S20: {
      // 9-bit resolution.
      int16_t fixed_point = (scratchpad[1] << 8) + scratchpad[0];
      return TemperatureData{.resolution = RESOLUTION_9_BITS,
                             .temperature = roo_temperature::DegCelcius(
                                 (float)fixed_point / 2.0f)};
    }
    case DEVICE_FAMILY_DS18B20:
    case DEVICE_FAMILY_DS1822:
    case DEVICE_FAMILY_DS1825:
    case DEVICE_FAMILY_DS28EA00: {
      // 9-12-bit resolution.
      Resolution resolution = Read2BitResolution(scratchpad);
      uint16_t mask = ~((1 << (12 - resolution)) - 1);
      int16_t fixed_point = ((scratchpad[1] << 8) + scratchpad[0]) & mask;
      return TemperatureData{.resolution = resolution,
                             .temperature = roo_temperature::DegCelcius(
                                 (float)fixed_point / 16.0f)};
    }
    case DEVICE_FAMILY_MAX31850: {
      // 14-bit resolution.
      return TemperatureData{.resolution = RESOLUTION_14_BITS,
                             .temperature = roo_temperature::Unknown()};
    }
    default: {
      return TemperatureData{.resolution = RESOLUTION_UNDEFINED,
                             .temperature = roo_temperature::Unknown()};
    }
  }
}

}  // namespace

Bus& Thermometers::bus() { return onewire_.bus(); }

Thermometers::Thermometers(OneWire& onewire,
                           roo_scheduler::Scheduler& scheduler)
    : onewire_(onewire),
      last_completed_conversion_(Uptime::Start()),
      pending_conversion_(Uptime::Start()),
      conversion_completion_task_(scheduler,
                                  [this]() { conversionCompleted(); }) {}

bool Thermometers::update() {
  if (isConversionPending()) {
    return true;
  }
  readPowerSupply();
  updateThermometers();
  if (!beginConversion()) return false;
  Interval delay = Millis(750);
  conversion_completion_task_.scheduleAfter(delay);
  pending_conversion_ = Uptime::Now() + delay;
  return true;
}

void Thermometers::updateThermometers() {
  RomCodeSet discovered = onewire_.discoverAll();
  // Remove thermometers that disappeared from the bus.
  for (const auto& i : thermometers_) {
    if (!discovered.contains(i.rom_code())) {
      thermometers_.erase(i.rom_code());
    }
  }
  // Add newly discovered thermometers.
  for (const auto& i : discovered) {
    if (!thermometers_.contains(i)) {
      Scratchpad scratchpad;
      if (!readScratchpad(i, scratchpad)) continue;
      Thermometer t;
      if (!initThermometer(i, scratchpad, t, /*post_conversion*/ false))
        continue;
      thermometers_.insert(t);
    }
  }
  rom_codes_.clear();
  for (const auto& i : thermometers_) {
    rom_codes_.push_back(i.rom_code());
  }
  std::sort(rom_codes_.begin(), rom_codes_.end());
  for (EventListener* listener : event_listeners_) {
    listener->discoveryCompleted();
  }
}

bool Thermometers::readScratchpad(RomCode rom_code, Scratchpad& scratchpad) {
  if (!bus().reset()) {
    LOG(ERROR) << "Reading scratchpad failed for OneWire device " << rom_code
               << " (bus error)";
    return false;
  }

  OneWireDeviceAddress addr;
  rom_code.toOneWireDeviceAddress(addr);
  bus().select(addr);
  bus().write(kReadScratchpad);

  for (uint8_t i = 0; i < 9; i++) {
    scratchpad[i] = bus().read();
  }
  if (!bus().reset()) {
    LOG(ERROR) << "Reading scratchpad failed for OneWire device " << rom_code
               << " (protocol error)";
    return false;
  }

  // Verify CRC.
  if (bus().crc8(&scratchpad[0], 8) != scratchpad[8]) {
    LOG(ERROR) << "Reading scratchpad failed for OneWire device " << rom_code
               << " (CRC error)";
    return false;
  }
  return true;
}

bool Thermometers::initThermometer(RomCode rom_code,
                                   const Scratchpad& scratchpad, Thermometer& t,
                                   bool post_conversion) {
  DeviceFamily family;
  TemperatureData temperature;
  switch (rom_code.getFamily()) {
    case 0x10: {
      if (scratchpad[4] != 0xFF || scratchpad[5] != 0xFF) {
        // Per DS1820 and DS18S20 datasheet, reading bytes 4 and 5 always
        // returns 1s.
        // https://datasheetspdf.com/pdf-down/D/S/1/DS1820_Dallas.pdf
        // https://www.analog.com/media/en/technical-documentation/data-sheets/DS18S20.pdf
        LOG(ERROR) << "Bogus scratchpad content for (supposedly) DS18S20 "
                   << rom_code << ": " << scratchpad[4] << ", "
                   << scratchpad[5];
        return false;
      }
      family = DEVICE_FAMILY_DS18S20;
      temperature = ReadTemperatureData(family, scratchpad);
      break;
    }
    case 0x28: {
      // https://www.analog.com/media/en/technical-documentation/data-sheets/DS18B20.pdf
      // https://www.analog.com/media/en/technical-documentation/data-sheets/MAX31820.pdf
      family = DEVICE_FAMILY_DS18B20;
      temperature = ReadTemperatureData(family, scratchpad);
      break;
    }
    case 0x22: {
      // https://www.analog.com/media/cn/technical-documentation/data-sheets/2795.pdf
      family = DEVICE_FAMILY_DS1822;
      temperature = ReadTemperatureData(family, scratchpad);
      break;
    }
    case 0x3B: {
      // Can be either DS1825 or MAX31850/51, which are very different
      // devices. We tell them apart by looking at what the configuration
      // register contains.
      // https://www.analog.com/media/en/technical-documentation/data-sheets/DS1825.pdf
      // https://www.analog.com/media/en/technical-documentation/data-sheets/MAX31850-MAX31851.pdf
      if ((scratchpad[4] & 0x80) == 0) {
        family = DEVICE_FAMILY_DS1825;
        temperature = ReadTemperatureData(family, scratchpad);
      } else {
        family = DEVICE_FAMILY_MAX31850;
        temperature = ReadTemperatureData(family, scratchpad);
      }
      break;
    }
    case 0x42: {
      family = DEVICE_FAMILY_DS28EA00;
      temperature = ReadTemperatureData(family, scratchpad);
      break;
    }
    default: {
      LOG(ERROR) << "Unrecognized family code 0x" << roo_logging::hex
                 << rom_code.getFamily();
      return false;
    }
  }
  t.set(rom_code, family, temperature.resolution,
        post_conversion ? temperature.temperature : roo_temperature::Unknown());
  return true;
}

bool Thermometers::beginConversion() {
  if (!bus().reset()) return false;
  bus().skip();
  bus().write(kConvert, parasite_);
  return true;
}

void Thermometers::conversionCompleted() {
  last_completed_conversion_ = pending_conversion_;
  pending_conversion_ = Uptime::Start();
  for (const auto& i : rom_codes_) {
    Scratchpad scratchpad;
    if (readScratchpad(i, scratchpad)) {
      initThermometer(i, scratchpad, *thermometers_.find(i),
                      /*post_conversion*/ true);
    }
  }
  for (auto& listener : event_listeners_) {
    listener->conversionCompleted();
  }
}

void Thermometers::readPowerSupply() {
  bus().reset();
  bus().skip();
  bus().write(kReadPowerSupply);
  parasite_ = (bus().read_bit() == 0);
  bus().reset();
}

void Thermometers::addEventListener(EventListener* listener) {
  auto result = event_listeners_.insert(listener);
  CHECK(result.second) << "Event listener " << listener
                       << " was registered already.";
}

void Thermometers::removeEventListener(EventListener* listener) {
  event_listeners_.erase(listener);
}

}  // namespace roo_onewire