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
