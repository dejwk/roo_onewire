// Using roles, you can isolate your application from specific thermometer
// devices, so that you can replace thermometer devices easily without having to
// recompile the entire program.
//
// Thermometer role is an abstract thermometer that can be assigned (and
// unassigned, and re-assigned) to a specific OneWire thermometer at any time.
// Your application depends on the abstract role, and the assignments are
// handled separately. By default, the assignments are persisted in Flash, so
// that once the assignment is done, it persists over program restarts.

#include "Arduino.h"
#include "roo_onewire.h"
#include "roo_onewire/thermometer_roles.h"
#include "roo_scheduler.h"
#include "roo_time.h"

using namespace roo_onewire;
using namespace roo_scheduler;
using namespace roo_time;

const int kOneWirePin = 14;

Scheduler scheduler;
roo_onewire::OneWire onewire(kOneWirePin, scheduler);

// We use two hypothetical roles in this example.
enum RoleId { KITCHEN, BEDROOM };

// The spec vector gives roles names, which is useful for logging or UI.
std::vector<ThermometerRoles::Spec> roles = {{KITCHEN, "Kitchen"},
                                             {BEDROOM, "Bedroom"}};

// This is the main object that you will use to manage the assignments.
ThermometerRoles thermometer_roles(onewire, roles);

// These are the specific abstract thermometers that your application may depend on.
// Alternatively, your program can refer to thermometer_roles directly, and use
// lookup by role ID (e.g. calling thermometer_roles.thermometerRoleById(KITCHEN)).
ThermometerRole& kitchen = thermometer_roles.thermometer_role(0);
ThermometerRole& bedroom = thermometer_roles.thermometer_role(1);

// Triggers conversion every two seconds.
RepetitiveTask converter(
    scheduler,
    []() {
      LOG(INFO) << "Attempting conversion...";
      if (!onewire.update()) {
        LOG(WARNING)
            << "OneWire update failed; possibly no thermometers attached to "
               "the bus?";
      }
    },
    Seconds(2));

// Periodic temperature reading. This is to illustrate that you can read the
// most recently measured temperatures at any time, and you can get both the
// value and the time of conversion.
RepetitiveTask reader(
    scheduler,
    []() {
      LOG(INFO) << "Periodic read...";
      LOG(INFO) << "  Kitchen: " << kitchen.readTemperature();
      LOG(INFO) << "  Bedroom: " << bedroom.readTemperature();
    },
    Seconds(5));

// Called immediately after the conversion completes.
ThermometerRoles::ConversionListener listener([]() {
  LOG(INFO) << "Conversion complete.";
  LOG(INFO) << "  Kitchen temperature: " << kitchen.readTemperature().value;
  LOG(INFO) << "  Bedroom temperature: " << bedroom.readTemperature().value;
});

void setup() {
  // By default, the assignments are stored in Flash. (To override, use a
  // 3-parameter constructor when creating ThermometerRoles). Because of this,
  // the second time you run the sketch, the thermometers will be already
  // assigned.
  LOG(INFO) << kitchen;
  LOG(INFO) << bedroom;
  // Trigger discovery, to see if we may have some unassigned thermometers.
  onewire.update();
  LOG(INFO) << "Unassigned: " << thermometer_roles.unassigned();
  if (!kitchen.isAssigned() || !bedroom.isAssigned()) {
    // Normally you'd want some interactive protocol to allow the user to assign
    // roles to thermometers, but in this simple example, we assign eagerly and
    // arbitrarily.
    if (!kitchen.isAssigned() && !thermometer_roles.unassigned().empty()) {
      // Assign the first available unassigned thermometer to the 'kitchen'.
      LOG(INFO) << "Assigning " << kitchen.name() << " to "
                << thermometer_roles.unassigned().front();
      thermometer_roles.assign(KITCHEN, thermometer_roles.unassigned().front());
    }
    if (!bedroom.isAssigned() && !thermometer_roles.unassigned().empty()) {
      // Assign the first available unassigned thermometer to the 'bedroom'.
      LOG(INFO) << "Assigning " << bedroom.name() << " to "
                << thermometer_roles.unassigned().front();
      thermometer_roles.assign(BEDROOM, thermometer_roles.unassigned().front());
    }
    LOG(INFO) << kitchen;
    LOG(INFO) << bedroom;
    LOG(INFO) << "Unassigned: " << thermometer_roles.unassigned();
  }

  // Register our listener so that it gets executed every time conversion
  // completes.
  thermometer_roles.addEventListener(&listener);
  // Get the converter going, so that it triggers conversion every 2 seconds.
  converter.startInstantly();
  // Also start the periodic reader, with the first read after 5 seconds.
  reader.start();
}

void loop() {
  // Do whetever else you need to do in the loop(), just remember to call the
  // scheduler to execute pending tasks when they are ready.
  scheduler.executeEligibleTasksUpToNow();
}
