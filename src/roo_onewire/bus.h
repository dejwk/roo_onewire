#pragma once

#ifdef ROO_TESTING
#include "roo_testing/buses/onewire/OneWire.h"
#include "roo_testing/buses/onewire/fake_onewire.h"
#else
#include "roo_onewire/internal/OneWire.h"
#endif

namespace roo_onewire {

#ifdef ROO_TESTING
using Bus = ::FakeOneWire;
#else
using Bus = ::roo_onewire::internal::OneWire;
#endif

}  // namespace roo_onewire
