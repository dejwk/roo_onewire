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
        "@roo_collections",
        "@roo_logging",
        "@roo_quantity",
        "@roo_scheduler",
        "@roo_testing//roo_testing/buses/onewire",
        "@roo_testing//roo_testing/devices/onewire/thermometer",
        "@roo_testing//roo_testing/frameworks/arduino-esp32-2.0.4/cores/esp32",
    ],
)
