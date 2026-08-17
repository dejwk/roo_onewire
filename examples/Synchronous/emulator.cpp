#include "roo_testing/buses/onewire/fake_onewire.h"
#include "roo_testing/devices/onewire/thermometer/thermometer.h"
#include "roo_testing/microcontrollers/esp32/fake_esp32.h"
#include "roo_testing/transducers/temperature/temperature.h"

namespace {

using roo_testing_transducers::FixedThermometer;
using roo_testing_transducers::Temperature;

FixedThermometer thermometer(Temperature::FromC(23.5));
FakeOneWireInterface bus({
    new FakeOneWireThermometer("28884B9B0A0000D3", thermometer),
});

struct AttachExampleBus {
  AttachExampleBus() { FakeEsp32().attachOneWireBus(bus, 18); }
};

AttachExampleBus attach_example_bus;

}  // namespace
