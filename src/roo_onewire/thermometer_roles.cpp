#include "roo_onewire/thermometer_roles.h"

#include "roo_logging.h"
#include "roo_onewire.h"
#include "roo_onewire/thermometers/hal/defaults.h"

namespace roo_onewire {

ThermometerRoles::ThermometerRoles(OneWire& onewire,
                                   const std::vector<Spec>& roles)
    : ThermometerRoles(onewire, DefaultStore(), roles) {}

ThermometerRoles::ThermometerRoles(OneWire& onewire,
                                   ThermometerRoleStore& store,
                                   const std::vector<Spec>& roles)
    : onewire_(onewire), listener_(*this) {
  int i = 0;
  for (const auto& t : roles) {
    thermometer_roles_.emplace_back(t.id, t.name);
    idx_by_id_[t.id] = i;
    ++i;
  }
  onewire_.thermometers().addEventListener(&listener_);
  setStore(&store);
}

ThermometerRoles::~ThermometerRoles() {
  onewire_.thermometers().removeEventListener(&listener_);
}

void ThermometerRoles::setStore(ThermometerRoleStore* store) {
  store_ = store;
  for (auto& t : thermometer_roles_) {
    RomCode rom_code = store_->getRomCode(t.id());
    if (rom_code.isUnknown()) {
      id_by_rom_code_.erase(rom_code);
    } else {
      t.assign(rom_code);
      id_by_rom_code_[rom_code] = t.id();
    }
  }
}

const ThermometerRole& ThermometerRoles::thermometerRoleById(int id) const {
  auto itr = idx_by_id_.find(id);
  CHECK(itr != idx_by_id_.end()) << id;
  return thermometer_role(itr->second);
}

roo_temperature::Temperature ThermometerRoles::temperatureById(int id) const {
  const ThermometerRole& t = thermometerRoleById(id);
  return t.isAssigned() ? temperatureByRomCode(t.rom_code())
                        : roo_temperature::Temperature();
}

roo_temperature::Temperature ThermometerRoles::temperatureByRomCode(
    RomCode rom_code) const {
  const Thermometer* t = onewire_.thermometers().thermometerByRomCode(rom_code);
  if (t == nullptr) return roo_temperature::Temperature();
  return t->temperature();
}

void ThermometerRoles::update() { onewire_.update(); }

void ThermometerRoles::refreshUnassignedThermometers() {
  unassigned_thermometers_.clear();
  for (int i = 0; i < onewire_.thermometers().count(); ++i) {
    auto rom_code = onewire_.thermometers().rom_code(i);
    if (!id_by_rom_code_.contains(rom_code)) {
      unassigned_thermometers_.push_back(rom_code);
    }
  }
}

void ThermometerRoles::assign(int id, RomCode rom_code) {
  DCHECK(!id_by_rom_code_.contains(rom_code));
  thermometer_roles_[idx_by_id_[id]].assign(rom_code);
  id_by_rom_code_[rom_code] = id;
  CHECK_NOTNULL(store_)->setRomCode(id, rom_code);
  refreshUnassignedThermometers();
}

void ThermometerRoles::unassign(int id) {
  ThermometerRole& t = thermometer_roles_[idx_by_id_[id]];
  if (t.isAssigned()) {
    id_by_rom_code_.erase(t.rom_code());
    t.unassign();
    CHECK_NOTNULL(store_)->clearRomCode(id);
    refreshUnassignedThermometers();
  }
}

void ThermometerRoles::updateTemperatures() {
  for (int i = 0; i < thermometer_roles_count(); ++i) {
    ThermometerRole& role = thermometer_role(i);
    if (!role.isAssigned()) continue;
    const Thermometer* t =
        onewire_.thermometers().thermometerByRomCode(role.rom_code());
    if (t == nullptr || t->temperature().isUnknown()) continue;
    role.setLastReading(t->temperature(),
                        onewire_.thermometers().lastReadingTime());
  }
}

void ThermometerRoles::discoveryCompleted() {
  refreshUnassignedThermometers();
  for (EventListener* listener : event_listeners_) {
    listener->discoveryCompleted();
  }
}

void ThermometerRoles::conversionCompleted() {
  updateTemperatures();
  for (EventListener* listener : event_listeners_) {
    listener->conversionCompleted();
  }
}

void ThermometerRoles::addEventListener(EventListener* listener) {
  auto result = event_listeners_.insert(listener);
  CHECK(result.second) << "Event listener " << listener
                       << " was registered already.";
}

void ThermometerRoles::removeEventListener(EventListener* listener) {
  event_listeners_.erase(listener);
}

}  // namespace roo_onewire