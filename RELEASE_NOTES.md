# roo_onewire 2.0.12

- Update `roo_quantity` from 1.1.9 to 1.1.10 in Bazel and raise the PlatformIO minimum dependency version to 1.1.10.

---

# roo_onewire 2.0.11

- Upgrade Roo dependencies to `roo_collections` 1.4.7, `roo_logging` 1.5.10, and `roo_scheduler` 2.2.0 in Bazel and PlatformIO.
- Update Bazel dependencies to `rules_cc` 0.2.25 and `googletest` 1.18.0.bcr.1.
- Update `roo_testing` and the shared CI workflow to 2.1.2.
- Add consolidated release notes for previous versions.

---

# [roo_onewire 2.0.10](https://github.com/dejwk/roo_onewire/releases/tag/2.0.10)

Published 2026-08-29.

This release modernizes the development and CI workflow around `roo_testing` 2.0 and makes the included Arduino examples runnable under host emulation.

### Added

- Runnable Bazel targets for the synchronous and asynchronous examples.
- Host-emulated OneWire thermometer setup for both examples.
- README documentation for host emulation and running examples with Bazelisk.

### Changed

- Updated CI to use the shared `roo_testing` 2.0 workflow, including pull-request and manual-run support.
- Centralized Arduino ESP32 and AddressSanitizer Bazel configuration.
- Updated Roo dependencies:
  - `roo_collections` 1.4.6
  - `roo_logging` 1.5.8
  - `roo_scheduler` 2.1.10
  - `roo_quantity` 1.1.9
  - `roo_testing` 2.1.0

No library API changes are expected.

**Full Changelog:** https://github.com/dejwk/roo_onewire/compare/2.0.9...2.0.10

---

# [roo_onewire 2.0.9](https://github.com/dejwk/roo_onewire/releases/tag/2.0.9)

Published 2026-02-26.

* Doxygen documentation,
* Updated dependencies,
* Cleaned build warnings.

**Full Changelog**: https://github.com/dejwk/roo_onewire/compare/2.0.8...2.0.9

---

# [roo_onewire 2.0.8](https://github.com/dejwk/roo_onewire/releases/tag/2.0.8)

Published 2026-01-26.

Fixed tests after change of behavior of Bazel.

**Full Changelog**: https://github.com/dejwk/roo_onewire/compare/2.0.7...2.0.8

---

# [roo_onewire 2.0.7](https://github.com/dejwk/roo_onewire/releases/tag/2.0.7)

Published 2026-01-06.

Updated dependencies.

---

# [roo_onewire 2.0.6](https://github.com/dejwk/roo_onewire/releases/tag/2.0.6)

Published 2026-01-06.

Updated dependencies.

**Full Changelog**: https://github.com/dejwk/roo_onewire/compare/2.0.5...2.0.6

---

# [roo_onewire 2.0.5](https://github.com/dejwk/roo_onewire/releases/tag/2.0.5)

Published 2025-11-12.

* Added conditional debug logging.
* Added begin(pin), because pinMode() in a constructor is problematic.

**Full Changelog**: https://github.com/dejwk/roo_onewire/compare/2.0.4...2.0.5

---

# [roo_onewire 2.0.4](https://github.com/dejwk/roo_onewire/releases/tag/2.0.4)

Published 2025-10-31.

* Better CI, added .gitignore
* Refreshed dependencies.

**Full Changelog**: https://github.com/dejwk/roo_onewire/compare/2.0.3...2.0.4

---

# [roo_onewire 2.0.3](https://github.com/dejwk/roo_onewire/releases/tag/2.0.3)

Published 2025-10-19.

Cleaning up log messages which were left in the code by mistake.

---

# [roo_onewire 2.0.2](https://github.com/dejwk/roo_onewire/releases/tag/2.0.2)

Published 2025-10-05.

* Updated dependencies.

**Full Changelog**: https://github.com/dejwk/roo_onewire/compare/2.0.1...2.0.2

---

# [roo_onewire 2.0.1](https://github.com/dejwk/roo_onewire/releases/tag/2.0.1)

Published 2025-09-27.

Fix: truly removing the external dependency on the OneWire library.

---

# [roo_onewire 1.0.1](https://github.com/dejwk/roo_onewire/releases/tag/1.0.1)

Published 2024-08-08.

Fixing dependencies.

---

# [roo_onewire 1.0.0](https://github.com/dejwk/roo_onewire/releases/tag/1.0.0)

Published 2024-08-08.

Initial release.

---

