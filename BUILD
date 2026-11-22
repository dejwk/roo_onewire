# BUILD file for use with https://github.com/dejwk/roo_testing.

load("@rules_cc//cc:cc_library.bzl", "cc_library")
load("@rules_cc//cc:cc_test.bzl", "cc_test")

cc_library(
    name = "roo_onewire",
    srcs = glob([
        "src/**/*.cpp",
        "src/**/*.h",
    ]),
    includes = [
        "src",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "@roo_collections",
        "@roo_logging",
        "@roo_quantity",
        "@roo_scheduler",
        "@roo_testing//roo_testing/buses/onewire",
        "@roo_testing//roo_testing/devices/onewire/thermometer",
        "@roo_testing//roo_testing/frameworks/arduino-esp32-2.0.4/cores/esp32",
    ],
)

cc_test(
    name = "rom_code_test",
    srcs = [
        "test/rom_code_test.cpp",
    ],
    copts = ["-Iexternal/gtest/include"],
    includes = ["src"],
    linkstatic = 1,
    deps = [
        ":roo_onewire",
        "@googletest//:gtest",
        "@roo_testing//:arduino_gtest_main",
    ],
)

cc_test(
    name = "thermometers_test",
    size = "small",
    srcs = ["test/thermometers_test.cpp"],
    deps = [
        ":roo_onewire",
        "@googletest//:gtest",
        "@roo_scheduler",
        "@roo_testing//:arduino_gtest_main",
        "@roo_testing//roo_testing/buses/onewire",
        "@roo_testing//roo_testing/devices/onewire/thermometer",
        "@roo_testing//roo_testing/microcontrollers/esp32",
        "@roo_testing//roo_testing/system:manual_time_mode",
        "@roo_testing//roo_testing/system:timer",
        "@roo_testing//roo_testing/transducers/temperature",
    ],
)

# Exercise the bundled GPIO driver with instrumented Arduino and ESP-IDF calls.
cc_test(
    name = "onewire_driver_test",
    size = "small",
    srcs = [
        "src/roo_onewire/internal/OneWire.cpp",
        "src/roo_onewire/internal/OneWire.h",
        "src/roo_onewire/internal/util/OneWire_direct_gpio.h",
        "src/roo_onewire/internal/util/OneWire_direct_regtype.h",
        "test/esp32_gpio_stubs/Arduino.h",
        "test/esp32_gpio_stubs/driver/gpio.h",
        "test/esp32_gpio_stubs/hal/gpio_ll.h",
        "test/onewire_driver_test.cpp",
    ],
    copts = ["-UROO_TESTING"],
    includes = [
        "src",
        "test/esp32_gpio_stubs",
    ],
    local_defines = [
        "ARDUINO=10819",
        "ARDUINO_ARCH_ESP32=1",
    ],
    deps = ["@googletest//:gtest_main"],
)
