load("@roo_testing//roo_testing/emulation:arduino.bzl", "roo_arduino_example")

roo_arduino_example(
    name = "Synchronous",
    sketch = "Synchronous.ino",
    srcs = ["emulator.cpp"],
    deps = [
        "//:roo_onewire",
        "@roo_scheduler",
        "@roo_testing//roo_testing/devices/onewire/thermometer",
        "@roo_testing//roo_testing/microcontrollers/esp32",
        "@roo_testing//roo_testing/transducers/temperature",
    ],
)
