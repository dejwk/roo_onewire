#pragma once

#include <functional>

#include "roo_collections.h"
#include "roo_collections/flat_small_hash_map.h"
#include "roo_onewire/bus.h"
#include "roo_onewire/device_family.h"
#include "roo_onewire/rom_code.h"
#include "roo_onewire/thermometers/resolution.h"
#include "roo_onewire/thermometers/thermometer.h"
#include "roo_scheduler.h"
#include "roo_time.h"

namespace roo_onewire {

class OneWire;

/// Scratchpad read buffer for DS18xx-family devices.
using Scratchpad = uint8_t[9];

/// Collection of discovered thermometers with async conversion support.
class Thermometers {
 public:
  /// Listener for discovery and conversion events.
  class EventListener {
   public:
    virtual ~EventListener() = default;

    /// Called after discovery finishes; the set of thermometers may change.
    virtual void discoveryCompleted() const {}

    /// Called when new temperature readings are available.
    virtual void conversionCompleted() const {}
  };

  /// Convenience listener that invokes a callback after conversion.
  class ConversionListener : public EventListener {
   public:
    ConversionListener(std::function<void()> fn) : fn_(fn) {}
    void conversionCompleted() const override { fn_(); }

   private:
    std::function<void()> fn_;
  };

  class ConstIterator {
   public:
    ConstIterator(ConstIterator&& other) = default;
    ConstIterator(const ConstIterator& other) = default;

    const Thermometer& operator*() const {
      return thermometers_->thermometer(idx_);
    }

    const Thermometer* operator->() const {
      return &thermometers_->thermometer(idx_);
    }

    ConstIterator& operator++() {
      ++idx_;
      return *this;
    }

    ConstIterator operator++(int) {
      ConstIterator tmp = *this;
      ++idx_;
      return tmp;
    }

    friend bool operator==(const ConstIterator& a, const ConstIterator& b) {
      return a.thermometers_ == b.thermometers_ && a.idx_ == b.idx_;
    }

    friend bool operator!=(const ConstIterator& a, const ConstIterator& b) {
      return a.thermometers_ != b.thermometers_ || a.idx_ != b.idx_;
    }

   private:
    friend class Thermometers;

    ConstIterator(const Thermometers* thermometers, int idx)
        : thermometers_(thermometers), idx_(idx) {}

    const Thermometers* thermometers_;
    int idx_;
  };

  /// Returns true if the bus uses parasite power; false otherwise.
  ///
  /// Updated by `update()`.
  bool isParasite() const { return parasite_; }

  /// Returns the count of supported thermometers found on the bus.
  int count() const { return rom_codes_.size(); }

  /// Returns the rom code of the ith thermometer.
  ///
  /// The thermometers are ordered by rom code.
  RomCode rom_code(int idx) const { return rom_codes_[idx]; }

  /// Returns a thermometer with the specified rom code, or nullptr if missing.
  const Thermometer* thermometerByRomCode(RomCode rom_code) const {
    const auto itr = thermometers_.find(rom_code);
    return (itr == thermometers_.end()) ? nullptr : &*itr;
  }

  /// Returns the ith identified thermometer.
  ///
  /// The thermometers are ordered by rom code.
  const Thermometer& thermometer(int idx) const {
    return *thermometerByRomCode(rom_code(idx));
  }

  /// Returns the last completed conversion time, or Uptime::Start() if none.
  ///
  /// Thermometers generally report the same `conversion_time()`, but may
  /// return older values if they failed to read after the latest conversion.
  roo_time::Uptime lastReadingTime() const {
    return last_completed_conversion_;
  }

  /// Adds a listener for discovery or conversion events.
  void addEventListener(EventListener* listener);

  /// Removes a previously added event listener.
  void removeEventListener(EventListener* listener);

  /// Returns true if a conversion is in progress.
  ///
  /// Use `getPendingConversionTime()` to see when it should complete.
  bool isConversionPending() const {
    return pending_conversion_ != roo_time::Uptime::Start();
  }

  /// Returns the expected conversion completion time, or Uptime::Start().
  roo_time::Uptime getPendingConversionTime() const {
    return pending_conversion_;
  }

  /// Returns all rom codes, sorted lexicographically.
  const std::vector<RomCode>& rom_codes() const { return rom_codes_; }

  /// Returns an iterator pointing at the first thermometer.
  ConstIterator begin() const { return ConstIterator(this, 0); }

  /// Returns an iterator pointing past the last thermometer.
  ConstIterator end() const { return ConstIterator(this, count()); }

  /// Returns the deadline after which missing devices are pruned.
  ///
  /// Defaults to 5 seconds. Helps reduce flicker on weak signal lines.
  roo_time::Duration pruningGracePeriod() const {
    return pruning_grace_period_;
  }

  /// Sets the deadline after which missing devices are pruned.
  void setPruningGracePeriod(roo_time::Duration pruning_grace_period) {
    pruning_grace_period_ = pruning_grace_period;
  }

 private:
  friend class OneWire;

  struct ThermometerKeyFn {
    const RomCode& operator()(const Thermometer& t) const {
      return t.rom_code();
    }
  };

  class ThermometersHT
      : public roo_collections::FlatSmallHashtable<
            Thermometer, RomCode, RomCodeHashFn, ThermometerKeyFn> {
   public:
    ConstIterator find(const RomCode& rom_code) const {
      return roo_collections::FlatSmallHashtable<
          Thermometer, RomCode, RomCodeHashFn,
          ThermometerKeyFn>::find(rom_code);
    }
    Iterator find(const RomCode& rom_code) { return lookup(rom_code); }
  };

  Thermometers(OneWire& onewire, roo_scheduler::Scheduler& scheduler);

  Bus& bus();

  /// Refreshes discovery/state and starts conversion when possible.
  ///
  /// If a conversion is already pending, this is a no-op and returns true.
  /// Returns false only when a new conversion cannot be started.
  bool update();

  void updateThermometers();

  bool readScratchpad(RomCode rom_code, Scratchpad& scratchpad);

  bool beginConversion();

  void conversionCompleted();

  /// Initializes a thermometer using a scratchpad read.
  ///
  /// If `conversion_time` is zero, it is assumed that no conversion has
  /// completed yet.
  bool initThermometer(RomCode rom_code, const Scratchpad& scratchpad,
                       Thermometer& t, roo_time::Uptime conversion_time);

  void readPowerSupply();

  /// The bus.
  OneWire& onewire_;

  /// When did the last conversion finish.
  roo_time::Uptime last_completed_conversion_;

  /// When will the current conversion finish. Zero means none is pending.
  roo_time::Uptime pending_conversion_;

  /// Whether the bus uses parasite power. Auto-detected.
  bool parasite_;

  roo_scheduler::SingletonTask conversion_completion_task_;

  /// List of discovered rom codes, sorted ascending.
  std::vector<RomCode> rom_codes_;

  /// Map that allows retrieval of thermometers by rom code.
  ThermometersHT thermometers_;

  roo_collections::FlatSmallHashSet<EventListener*> event_listeners_;

  /// How long to keep a previously present thermometer as still present.
  ///
  /// Defaults to 5 seconds.
  roo_time::Duration pruning_grace_period_;
};

}  // namespace roo_onewire