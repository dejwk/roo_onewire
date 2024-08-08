# BUILD file for use with https://github.com/dejwk/roo_testing.

cc_library(
    name = "roo_onewire",
    visibility = ["//visibility:public"],
    srcs = glob([
            "src/**/*.cpp",
            "src/**/*.h"
        ]),
    includes = [
        "src",
    ],
    deps = [
        "//lib/roo_collections",
        "//lib/roo_scheduler",
        "//lib/roo_temperature",
        "//lib/roo_logging",
        "//lib/roo_prefs",
        "//roo_testing/buses/onewire",
        "//roo_testing/devices/onewire/thermometer",
        "//roo_testing/frameworks/arduino-esp32-2.0.4/cores/esp32",
    ],
)
