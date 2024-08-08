#pragma once

#ifdef ROO_TESTING
#include "roo_testing/buses/onewire/OneWire.h"
#include "roo_testing/buses/onewire/fake_onewire.h"
#else
#include "OneWire.h"
#endif

namespace roo_onewire {

#ifdef ROO_TESTING
using Bus = ::FakeOneWire;
#else
using Bus = ::OneWire;
#endif

}  // namespace roo_onewire
