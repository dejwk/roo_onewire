#pragma once

#include "roo_collections/flat_small_hash_map.h"
#include "roo_logging.h"
#include "roo_onewire/thermometers.h"
#include "roo_onewire/thermometers/hal/thermometer_role_store.h"
#include "roo_onewire/thermometers/thermometer_role.h"
#include "roo_scheduler.h"

namespace roo_onewire {

class ThermometerRoles {
 public:
  class EventListener {
   public:
    virtual ~EventListener() = default;
    virtual void discoveryCompleted() {}
    virtual void conversionCompleted() {}
  };

  class ConversionListener : public EventListener {
   public:
    ConversionListener(std::function<void()> fn) : fn_(fn) {}
    void conversionCompleted() override { fn_(); }

   private:
    std::function<void()> fn_;
  };

  struct Spec {
    int id;
    std::string name;
  };

  ThermometerRoles(OneWire& onewire, const std::vector<Spec>& roles);

  ThermometerRoles(OneWire& onewire, ThermometerRoleStore& store,
                   const std::vector<Spec>& roles);

  ~ThermometerRoles();

  int thermometer_roles_count() const { return thermometer_roles_.size(); }

  ThermometerRole& thermometer_role(int idx) { return thermometer_roles_[idx]; }

  const ThermometerRole& thermometer_role(int idx) const {
    return thermometer_roles_[idx];
  }

  const ThermometerRole& thermometerRoleById(int id) const;

  roo_temperature::Temperature temperatureById(int id) const;

  roo_temperature::Temperature temperatureByRomCode(RomCode rom_code) const;

  // Schedules a conversion (if not already scheduled) to read temperatures of
  // assigned roles.
  void update();

  // Assigns the role with the given `id` to the thermometer with the specified
  // rom code.
  void assign(int id, RomCode rom_code);

  // Unassigns the thermometer from the role with the given `id`.
  void unassign(int id);

  const std::vector<RomCode> unassigned() const {
    return unassigned_thermometers_;
  }

  void addEventListener(EventListener* listener);
  void removeEventListener(EventListener* listener);

 protected:
  void setStore(ThermometerRoleStore* store);

 private:
  friend class Listener;

  // Forwards events from the generic Thermometers object to ThermometerRoles.
  class Listener : public Thermometers::EventListener {
   public:
    Listener(ThermometerRoles& roles) : roles_(roles) {}

    void discoveryCompleted() const override { roles_.discoveryCompleted(); }
    void conversionCompleted() const override { roles_.conversionCompleted(); }

   private:
    ThermometerRoles& roles_;
  };

  void refreshUnassignedThermometers();
  void updateTemperatures();

  void discoveryCompleted();
  void conversionCompleted();

  OneWire& onewire_;
  ThermometerRoleStore* store_;
  Listener listener_;

  // Maps the application-defined thermometer IDs to the indexes into the
  // thermometers_ vector. Used to look up thermometers by ID.
  roo_collections::FlatSmallHashMap<int, int> idx_by_id_;

  // For those thermometers that are assigned a rom code, maps the rom code back
  // to the application-defined thermometer ID.
  roo_collections::FlatSmallHashMap<RomCode, int, RomCodeHashFn>
      id_by_rom_code_;

  std::vector<RomCode> unassigned_thermometers_;

  std::vector<ThermometerRole> thermometer_roles_;

  roo_collections::FlatSmallHashSet<EventListener*> event_listeners_;
};

}  // namespace roo_onewire
