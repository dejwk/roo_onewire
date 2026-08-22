// This example illustrates a simplistic, naive use of the OneWire thermometer
// in a synchronous mode.
//
// In production applications, consider using asynchronous mode instead,
// specifying the callback to be executed (by the scheduler) when new readings
// become available. See Asynchronous.ino for an example.

#include "Arduino.h"

static constexpr int kOneWirePin = 18;

#ifdef ROO_TESTING

#include "roo_testing/buses/onewire/fake_onewire.h"
#include "roo_testing/devices/onewire/thermometer/thermometer.h"
#include "roo_testing/microcontrollers/esp32/fake_esp32.h"
#include "roo_testing/transducers/temperature/temperature.h"

using roo_testing_transducers::FixedThermometer;
using roo_testing_transducers::Temperature;

struct Emulator {
  FixedThermometer indoor;
  FixedThermometer outdoor;
  FakeOneWireInterface bus;

  Emulator()
      : indoor(Temperature::FromC(23.5)),
        outdoor(Temperature::FromC(8.25)),
        bus({
            new FakeOneWireThermometer("28FF2A4C30180207", indoor),
            new FakeOneWireThermometer("28FFA585301801EB", outdoor),
        }) {
    FakeEsp32().attachOneWireBus(bus, kOneWirePin);
  }
} emulator;

#endif

#include "roo_onewire.h"
#include "roo_scheduler.h"

using namespace roo_onewire;
using namespace roo_scheduler;

Scheduler scheduler;
roo_onewire::OneWire onewire(scheduler);
Thermometers& thermometers = onewire.thermometers();

void setup() {
  onewire.begin(kOneWirePin);
}

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
