// This example illustrates a simplistic, naive use of the OneWire thermometer
// in a synchronous mode.
//
// In production applications, consider using asynchronous mode instead,
// specifying the callback to be executed (by the scheduler) when new readings
// become available. See Asynchronous.ino for an example.

#include "Arduino.h"
#include "roo_onewire.h"
#include "roo_scheduler.h"

using namespace roo_onewire;
using namespace roo_scheduler;

const int kOneWirePin = 18;

Scheduler scheduler;
roo_onewire::OneWire onewire(kOneWirePin, scheduler);
Thermometers& thermometers = onewire.thermometers();

void setup() {}

long next_update_time = 0;
bool readings_fetched = true;

void loop() {
  long now = millis();
  if (now > next_update_time) {
    if (onewire.update()) {
      LOG(INFO) << "Found " << thermometers.count()
                << " thermometers. Starting conversion...";
      readings_fetched = false;
    } else {
      LOG(ERROR)
          << "OneWire update failed; possibly no thermometers attached to "
             "the bus?";
    }
    next_update_time += 1000;
  }
  if (!readings_fetched && !onewire.thermometers().isConversionPending()) {
    LOG(INFO) << "Conversion complete.";
    readings_fetched = true;
    for (const auto& t : thermometers) {
      LOG(INFO) << "  " << t.rom_code() << ": " << t.temperature();
    }
    // Note: the readings are cached in memory until the next update.
    // Re-reading from the thermometers object does not cause any OneWire
    // communication.
  }

  // Must be called to make sure that the readings get fetched when due.
  scheduler.executeEligibleTasks();
}
