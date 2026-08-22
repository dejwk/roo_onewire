// This example illustrates use of the OneWire thermometer collection in an
// asychronous mode, where you can continue doing other work (in the same
// thread) while the conversion is in progress. Once the conversion finishes,
// the library triggers a designated function that can respond to it.

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
#include "roo_time.h"

using namespace roo_onewire;
using namespace roo_scheduler;
using namespace roo_time;

Scheduler scheduler;
roo_onewire::OneWire onewire(scheduler);

Thermometers& thermometers = onewire.thermometers();

// Triggers conversion every two seconds.
// We are using a scheduled task so that we don't have to worry about observing
// millis() and calling the function when needed - it is handled by the
// repetitive task implementation.
RepetitiveTask converter(scheduler, Seconds(2), []() {
  LOG(INFO) << "Attempting conversion...";
  if (!onewire.update()) {
    LOG(WARNING)
        << "OneWire update failed; possibly no thermometers attached to "
           "the bus?";
  }
});

// Called when conversion completes.
Thermometers::ConversionListener listener([]() {
  LOG(INFO) << "Conversion complete.";
  for (const auto& t : thermometers) {
    LOG(INFO) << "  " << t.rom_code() << ": " << t.temperature();
  }
});

void setup() {
  onewire.begin(kOneWirePin);

  // Register our listener so that it gets executed every time conversion
  // completes.
  thermometers.addEventListener(&listener);

  // Get the converter going, so that it triggers conversion every 2 seconds.
  converter.startInstantly();
}

void loop() {
  // Do whetever else you need to do in the loop(), just remember to call the
  // scheduler so that pending conversion tasks get executed when they become
  // due.
  scheduler.executeEligibleTasksUpToNow();
}
