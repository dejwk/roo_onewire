// This example illustrates use of the OneWire thermometer collection in an
// asychronous mode, where you can continue doing other work (in the same
// thread) while the conversion is in progress. Once the conversion finishes,
// the library triggers a designated function that can respond to it.

#include "Arduino.h"
#include "roo_onewire.h"
#include "roo_scheduler.h"
#include "roo_time.h"

using namespace roo_onewire;
using namespace roo_scheduler;
using namespace roo_time;

const int kOneWirePin = 14;

Scheduler scheduler;
roo_onewire::OneWire onewire(kOneWirePin, scheduler);

Thermometers& thermometers = onewire.thermometers();

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

// Called when conversion completes.
Thermometers::ConversionListener listener([]() {
  LOG(INFO) << "Conversion complete.";
  for (const auto& t : thermometers) {
    LOG(INFO) << "  " << t.rom_code() << ": " << t.temperature();
  }
});

void setup() {
  // Register our listener so that it gets executed every time conversion
  // completes.
  thermometers.addEventListener(&listener);
  // Get the converter going, so that it triggers conversion every 2 seconds.
  converter.startInstantly();
}

void loop() {
  // Do whetever else you need to do in the loop(), just remember to call the
  // scheduler to execute pending tasks when they are ready.
  //
  // At any given time, the scheduler queue may contain up to two tasks:
  // 1. the converter task (scheduled every 2s)
  // 2. the internal task that gets scheduled at a fixed interval after the
  //    converter, that fetches conversion results from the thermometers, and
  //    then calls your listener, notifying it that the conversion has
  //    completed.
  scheduler.executeEligibleTasksUpToNow();
}
