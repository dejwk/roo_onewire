#pragma once

/// Umbrella header for the roo_onewire module.
///
/// Provides OneWire bus access and thermometer-discovery/conversion helpers.

#include "roo_onewire/bus.h"
#include "roo_onewire/rom_code.h"
#include "roo_onewire/thermometers.h"
#include "roo_scheduler.h"

namespace roo_onewire {

class OneWire {
 public:
  /// Creates a OneWire bus master that uses the specified scheduler for
  /// asynchronous operations.
  explicit OneWire(roo_scheduler::Scheduler& scheduler);

  /// @deprecated Use `OneWire(scheduler)` followed by `begin(pin)` instead.
  ///
  /// Behavior of `pinMode()` during static initialization is not guaranteed.
  OneWire(uint8_t pin, roo_scheduler::Scheduler& scheduler);

  /// Initializes OneWire bus on the specified GPIO pin.
  void begin(uint8_t pin);

  /// Re-discovers devices on the bus, refreshes their state, and requests
  /// thermometer conversion.
  ///
  /// Returns `true` if conversion has been issued and is pending, or if a
  /// conversion is already in progress (in which case this call is effectively
  /// a no-op and returns immediately).
  ///
  /// Returns `false` when conversion could not be started (for example, when no
  /// supported thermometers are currently identified on the bus).
  bool update();

  /// Returns all recently discovered thermometers, including cached readings.
  Thermometers& thermometers() { return thermometers_; }

  /// Const overload of `thermometers()`.
  const Thermometers& thermometers() const { return thermometers_; }

 private:
  friend class Thermometers;

  RomCodeSet discoverAll();

  void readPowerSupply();

  Bus& bus() { return onewire_; }

  /// The bus.
  Bus onewire_;

  Thermometers thermometers_;
};

}  // namespace roo_onewire