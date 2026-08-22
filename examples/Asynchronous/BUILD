load("@roo_testing//roo_testing/emulation:arduino.bzl", "roo_arduino_example")

roo_arduino_example(
    name = "Asynchronous",
    sketch = "Asynchronous.ino",
    visibility = ["//visibility:public"],
    deps = [
        "//:roo_onewire",
        "@roo_scheduler",
        "@roo_testing//roo_testing/devices/onewire/thermometer",
        "@roo_testing//roo_testing/microcontrollers/esp32",
        "@roo_testing//roo_testing/transducers/temperature",
    ],
)
