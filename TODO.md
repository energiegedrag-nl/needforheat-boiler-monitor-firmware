# TODO – needforheat-boiler-monitor-firmware

Open work for this repository, distilled from the earlier NeedForHeat backlog at Windesheim, including the discussions on it (paraphrased and regrouped), and from the 2026 maintenance review.

Items that touch several repositories are written out in full in one *lead* repository; the others list them with a link (→). Keep the details up to date in the lead repository only.

## Now: needed to run at home this winter

- [ ] → Release workflow without the Windesheim signing key — see [needforheat-generic-firmware](https://github.com/energiegedrag-nl/needforheat-generic-firmware/blob/main/TODO.md#unsigned)

## Soon: robustness and correctness

- [ ] → Upgrade to ESP-IDF 5.5 (WPA3, Wi-Fi 6) — see [needforheat-generic-firmware](https://github.com/energiegedrag-nl/needforheat-generic-firmware/blob/main/TODO.md#idf)
- [ ] → Let generic and device-specific code share buttons — see [needforheat-generic-firmware](https://github.com/energiegedrag-nl/needforheat-generic-firmware/blob/main/TODO.md#buttons)
- [ ] → Use the ESP-IDF certificate bundle — see [needforheat-generic-firmware](https://github.com/energiegedrag-nl/needforheat-generic-firmware/blob/main/TODO.md#crtbundle)

## Later: features and ideas

- [ ] <a id="bmfw"></a>**Boiler monitor firmware backlog.** Aggregate several sensor values into one measurement (write a spec first); read on/off status through the OpenTherm splitter; detect wire functions with the new 4-wire splitter; investigate odd setpoints seen with some boiler/thermostat combinations; update the README and credit the DS18B20 library. Also affects: needforheat-boiler-monitor-hardware.
- [ ] → Finish the rename from Twomes to NeedForHeat — see [needforheat-generic-firmware](https://github.com/energiegedrag-nl/needforheat-generic-firmware/blob/main/TODO.md#rename)
- [ ] → CONTRIBUTING.md with the branching workflow — see [needforheat-generic-firmware](https://github.com/energiegedrag-nl/needforheat-generic-firmware/blob/main/TODO.md#contrib)
- [ ] → Fix links and errors in device manuals — see [needforheat-manuals](https://github.com/energiegedrag-nl/needforheat-manuals/blob/prd/TODO.md#manlinks)
- [ ] → BoilerBASE electronics — see [needforheat-boiler-monitor-hardware](https://github.com/energiegedrag-nl/needforheat-boiler-monitor-hardware/blob/main/TODO.md#bmpcb)
- [ ] → Other boiler protocols — see [needforheat-boiler-monitor-hardware](https://github.com/energiegedrag-nl/needforheat-boiler-monitor-hardware/blob/main/TODO.md#protocols)

## Recently done

- [x] Firmware builds again in CI: include case `openTherm.h` (the OpenTherm code was never missing), Python 3.10 and current actions, updated TLS roots (#1)
