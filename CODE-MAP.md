# LoRa-E5 Library Code Map

## `src/LoRa_E5.h`

Public Arduino API and persistent object state. Treat method names/signatures as the compatibility boundary for sketches that include this library. The header documents that RSSI/SNR are cached response values, not live radio queries.

## `src/LoRa_E5.cpp`

AT-command implementation.

- constructor: captures caller-owned `HardwareSerial` and UART settings;
- `begin()`: configures UART and probes for `OK`;
- `joinNetwork()`: OTAA mode/credential sequence plus asynchronous join-result loop;
- `sendMessage()`: payload validation, optional port command, confirmed/unconfirmed uplink and response parsing;
- `sendATCommand()`: generic line/token command helper;
- `readResponse()`: line-oriented serial reader with timeout;
- `flushSerial()`: removes stale UART input before a new transaction;
- `formatHexPayload()`: validates even-length hex and reformats it into module byte syntax;
- `captureSignalMetrics()`: parses RSSI/SNR fields embedded in message responses.

The expected-response strings are part of the module-firmware contract. Change them only after checking the response emitted by the target LoRa-E5 firmware.

## `examples/BasicExample/BasicExample.ino`

Reference integration sketch. Shows UART ownership, OTAA join, uplink sending and interpretation of cached RSSI/SNR. Keep credentials as placeholders; real AppKeys must not be committed.

## `library.properties`

Arduino Library Manager metadata such as name, version, sentence, architecture compatibility and repository URL. Version changes belong here when publishing a new Arduino library release.

## `keywords.txt`

Arduino IDE syntax-highlighting metadata. It should follow public API naming but has no runtime effect.

## `NOTES_Rev00.md`

Historical/design notes. Treat them as supporting context rather than a more authoritative source than the current source and README.

## `LICENSE`

Repository licence. Do not replace or reinterpret it as part of ordinary documentation maintenance.

## Change-risk map

| Change | Main risk | Minimum check |
| --- | --- | --- |
| Public method signature | Breaks sketches | Compile existing example/caller |
| UART setup | Module communication failure | Real `AT`/`OK` test |
| Expected response token | Firmware-specific parse failure | Capture actual module output |
| Join timing | False timeout or hang | OTAA join on real network |
| Payload formatter | Corrupt/rejected uplink | Known hex vectors |
| RSSI/SNR parser | Stale/wrong metrics | Recorded response strings |
| Arduino metadata | Install/discovery problems | Arduino library install/compile |
