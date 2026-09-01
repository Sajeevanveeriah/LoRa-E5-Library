# Repository documentation rules

- Keep public API comments in `src/LoRa_E5.h` current with implementation behaviour.
- Comment AT response semantics, timing, UART ownership, payload rules and firmware assumptions, not obvious C++ syntax.
- Treat public method signatures and `library.properties` metadata as compatibility surfaces.
- Keep real LoRaWAN credentials out of examples and source control.
- Do not claim a hardware/network validation unless it was actually run against the stated ESP32, LoRa-E5 firmware and network.
- Preserve behaviour in documentation-only changes.
