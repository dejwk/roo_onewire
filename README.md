# roo_onewire

OneWire abstractions and ESP32/Arduino integration.

## Host emulation

Host builds use the roo_testing 2.0 Arduino ESP32 profile. With Bazelisk 1.21
or newer, a plain command defaults to that profile and prints a notice:

    bazel test ...
    bazel test ... --config=asan
    bazel test ... --config=roo_testing_arduino_esp32

A representative sketch is a first-class runnable target (stop it with
Ctrl-C):

    bazel run //examples/Synchronous:Synchronous

The files under .roo_testing/bazelrc/esp32 are vendored from roo_testing;
follow their canonical-source headers when refreshing them.
