// This example illustrates use of the OneWire thermometer collection in a
// simple, synchronous mode.

#include "Arduino.h"
#include "roo_onewire.h"
#include "roo_scheduler.h"

using namespace roo_onewire;
using namespace roo_scheduler;

const int kOneWirePin = 14;

Scheduler scheduler;
roo_onewire::OneWire onewire(kOneWirePin, scheduler);
Thermometers& thermometers = onewire.thermometers();

void setup() {
  // When update() is called, the library discovers all thermometers, fetches
  // their current state, and requests conversion.
  onewire.update();
  // Thermometer details, except their temperatures, are available just after
  // the call to update returns.
  LOG(INFO) << "Thermometers discovered: "
            << onewire.thermometers().rom_codes();
  for (const auto& t : thermometers) {
    LOG(INFO) << t;
  }
}

void loop() {
  if (!onewire.update()) {
    LOG(ERROR) << "OneWire update failed; possibly no thermometers attached to "
                  "the bus?";
    delay(2000);
  } else {
    LOG(INFO) << "Found " << thermometers.count() << " thermometers.";
    LOG(INFO) << "Starting conversion... ";
    // Note: even though this example is synchronous, internally the library
    // asynchronously fetches conversion results from the thermometers as soon
    // as they are available. To enable this, we must use scheduler.delayUntil()
    // instead of the regular Arduino delay().
    scheduler.delayUntil(thermometers.getPendingConversionTime());
    LOG(INFO) << "Conversion complete.";
    for (const auto& t : thermometers) {
      LOG(INFO) << "  " << t.rom_code() << ": " << t.temperature();
    }
    // Note: the readings are cached in memory until the next update. Re-reading
    // from the thermometers object does not cause any OneWire communication.
  }
}
