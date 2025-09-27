#pragma once

#include <functional>

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

using Scratchpad = uint8_t[9];

class Thermometers {
 public:
  class EventListener {
   public:
    virtual ~EventListener() = default;

    // Called after the OneWire discovery protocol finishes. The list of
    // thermometers may now be different than before.
    virtual void discoveryCompleted() const {}

    // Called when new temperature readings are available on the thermometers.
    virtual void conversionCompleted() const {}
  };

  // Convenience implementation of the EventListener, which ignores discovery
  // events (new thermometers), but triggers on new readings, executing the
  // specified callback.
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

  // Returns true if the bus uses parasite power; false otherwise. Updated by
  // `update()`.
  bool isParasite() const { return parasite_; }

  // Returns the count of supported thermometers that have been identified on
  // the bus.
  int count() const { return rom_codes_.size(); }

  // Returns the rom code of an ith identified thermometer. The thermometers are
  // ordered by rom code.
  RomCode rom_code(int idx) const { return rom_codes_[idx]; }

  // Returns a thermometer with the specified rom code, or nullptr if such
  // thermometer has not been identified on the bus.
  const Thermometer* thermometerByRomCode(RomCode rom_code) const {
    const auto itr = thermometers_.find(rom_code);
    return (itr == thermometers_.end()) ? nullptr : &*itr;
  }

  // Returns the ith identified thermometer. The thermometers are
  // ordered by rom code.
  const Thermometer& thermometer(int idx) const {
    return *thermometerByRomCode(rom_code(idx));
  }

  // Returns the last completed conversion time, or Uptime::Start() if there was
  // no complete converion yet.
  //
  // Thermometers will generally report the same time from `conversion_time()`,
  // although they may report older values in case they failed to read after the
  // most recent conversion request.
  roo_time::Uptime lastReadingTime() const {
    return last_completed_conversion_;
  }

  // Adds a listener to be notified when new thermometers are discovered or when
  // convesion completes.
  void addEventListener(EventListener* listener);

  // Removes a previously added event listener.
  void removeEventListener(EventListener* listener);

  // Returs true if a conversion is in progress. You can check when the
  // conversion will complete by calling getPendingConversionTime().
  bool isConversionPending() const {
    return pending_conversion_ != roo_time::Uptime::Start();
  }

  // Returns the time (usually in the future) when the pending conversion is
  // expected to complete. If there is no pending conversion, returns
  // Uptime::Start().
  roo_time::Uptime getPendingConversionTime() const {
    return pending_conversion_;
  }

  // Returns the vector of all rom codes, sorted lexicographically.
  const std::vector<RomCode>& rom_codes() const { return rom_codes_; }

  // Returns an iterator pointing at the first thermometer.
  ConstIterator begin() const { return ConstIterator(this, 0); }

  // Returns an iterator pointing past the last thermometer.
  ConstIterator end() const { return ConstIterator(this, count()); }

  // Returns the deadline after which a thermometer that is no longer
  // discoverable gets removed by the `update()` call. Defaults to 5 seconds.
  // Helpful in overcoming flicker due to flakiness of the OneWire protocol on
  // weak singal lines.
  roo_time::Interval rememberance() const { return rememberance_; }

  // Sets the deadline after which thermometers that are no longer discoverable
  // get removed by the `update()` call.
  void setRememberance(roo_time::Interval rememberance) {
    rememberance_ = rememberance;
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

  bool update();

  void updateThermometers();

  bool readScratchpad(RomCode rom_code, Scratchpad& scratchpad);

  bool beginConversion();

  void conversionCompleted();

  // If conversion_time is zero, it is assumed that there has been no conversion
  // yet.
  bool initThermometer(RomCode rom_code, const Scratchpad& scratchpad,
                       Thermometer& t, roo_time::Uptime conversion_time);

  void readPowerSupply();

  // The bus.
  OneWire& onewire_;

  // When did the last conversion finish.
  roo_time::Uptime last_completed_conversion_;

  // When will the current conversion finish. Zero means none is pending.
  roo_time::Uptime pending_conversion_;

  // Whether the bus uses parasite power. Auto-detected.
  bool parasite_;

  roo_scheduler::SingletonTask conversion_completion_task_;

  // List of discovered rom codes, sorted ascending.
  std::vector<RomCode> rom_codes_;

  // Map that allows retrieval of thermometers by rom code.
  ThermometersHT thermometers_;

  roo_collections::FlatSmallHashSet<EventListener*> event_listeners_;

  // How long to report a previously present thermometer as still present, even
  // if it doesn't report during discovery. Defaults to 5 seconds. Can be
  // changed by setRememberance.
  roo_time::Interval rememberance_;
};

}  // namespace roo_onewire