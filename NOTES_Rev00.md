# Rev 00 notes

Summary of the changes made to get OTAA join and hex uplink working on the Seeed
LoRa-E5 / Wio-E5, and the one open decision.

## Defect F: option chosen
Option 1 was implemented (the default).
- `getBatteryVoltage()` has been removed (there is no battery command on this
  module). This is the only breaking change to the public API, and it is
  required because the underlying `AT+BAT` command does not exist.
- `getDeviceStatus()` now queries `AT+VER` (a real command) and returns the
  firmware version string, instead of the non-existent `AT+STATUS`.
- `getRSSI()` and `getSNR()` keep their signatures but now return the RSSI/SNR
  parsed from the most recent uplink response, instead of the non-existent
  `AT+RSSI` / `AT+SNR` commands. The values are stored whenever an uplink
  response containing `RSSI`/`SNR` is seen.

## What changed
- `joinNetwork()` (A, B): sets `AT+MODE=LWOTAA` first, quotes the DevEui, AppEui
  and AppKey values, sends `AT+JOIN`, then reads lines until
  `+JOIN: Network joined` (returns true), `+JOIN: Join failed` (returns false),
  or the timeout elapses. It no longer waits for `OK` after `AT+JOIN`.
- `sendMessage()` (C, D): no longer appends `,port` to the message command. The
  payload is validated as even-length hex, reformatted into space-separated
  bytes, and quoted. A non-default port is set separately with `AT+PORT`. The
  confirmed path matches `+CMSGHEX` and the unconfirmed path `+MSGHEX`; `Done`
  is success, and `fail`/`ERROR`/timeout are failures.
- `setDataRate()` (E): emits `AT+DR=DR<n>` (for example `AT+DR=DR3`) and matches
  the `+DR:` response, rather than `AT+DR=<bare number>` and `OK`.
- `sendATCommand()` (G): stores the matched response line in `_lastResponse`, so
  value-returning methods read the same response that was matched instead of
  triggering a second read that consumed the wrong line.
- Packaging (H, I, J): the example was moved to
  `examples/BasicExample/BasicExample.ino`, `library.properties` and
  `keywords.txt` were added, and the README was updated to the real
  4-argument constructor, the corrected flow, the hex payload note, and with the
  duplicate `Serial1.begin()` / double init removed.

## Firmware version caveat
Some LoRa-E5 behaviour and exact response strings are firmware dependent. The
matching here follows Seeed's AT Command Specification and the Wio-E5 wiki worked
examples. Note the firmware version reported by `AT+VER` during the smoke test,
and if a step does not behave as documented, check it against that version.

## Residual risk
- `setTxPower()` and `initializeModule()` were left as they were, because they
  are outside the listed defects. `setTxPower()` (`AT+POWER`) and
  `initializeModule()` (`AT+RESET`) still match `OK`, which may not match the
  real response on every firmware. They are not used on the join/uplink path.
  The example does not call `initializeModule()`, since `AT+RESET` is not needed
  for OTAA and its response is firmware dependent.
- `sendMessage()` uses a fixed 15 second window to cover both receive windows.
  A confirmed uplink with retries on a busy network could need longer.
- `getRSSI()` / `getSNR()` reflect the last uplink only. An unconfirmed uplink
  with no downlink may not carry RSSI/SNR, so the stored values can be stale or
  zero until an uplink response includes them.
- The `AT+PORT` response token (`+PORT:`) was not verified against hardware and
  is sent best effort (its result does not gate the uplink). The default method
  port is 1; if the module default differs, call `sendMessage()` with the port
  you want.

## Verification
- Offline compile gate passes for ESP32:
  `arduino-cli compile --fqbn esp32:esp32:esp32 examples/BasicExample/BasicExample.ino`
- The final `src/` and example contain no `AT+BAT`, `AT+STATUS`, `AT+RSSI` or
  `AT+SNR` commands, no `,port` appended to `MSGHEX`/`CMSGHEX`, and `AT+JOIN` is
  not paired with an `OK` check.
