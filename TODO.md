# TODO – needforheat-boiler-monitor-firmware

Open work for this repository, distilled from the old NeedForHeat Trello board, including the discussions on the cards (paraphrased and regrouped) and from the 2026 maintenance review.

Items that touch several repositories are written out in full in one *lead* repository; the others list them with a link (→). Keep the details up to date in the lead repository only.

## Now: needed to run at home this winter

- [ ] → Release workflow without the Windesheim signing key — see [needforheat-generic-firmware](https://github.com/energiegedrag-nl/needforheat-generic-firmware/blob/main/TODO.md#unsigned) *(Trello #480)*

## Soon: robustness and correctness

- [ ] → Upgrade to ESP-IDF 5.5 (WPA3, Wi-Fi 6) — see [needforheat-generic-firmware](https://github.com/energiegedrag-nl/needforheat-generic-firmware/blob/main/TODO.md#idf) *(Trello #806)*
- [ ] → Let generic and device-specific code share buttons — see [needforheat-generic-firmware](https://github.com/energiegedrag-nl/needforheat-generic-firmware/blob/main/TODO.md#buttons) *(Trello #833)*
- [ ] → Use the ESP-IDF certificate bundle — see [needforheat-generic-firmware](https://github.com/energiegedrag-nl/needforheat-generic-firmware/blob/main/TODO.md#crtbundle)

## Later: features and ideas

- [ ] <a id="bmfw"></a>**Boiler monitor firmware backlog.** Aggregate several sensor values into one measurement (write a spec first); read on/off status through the OpenTherm splitter; detect wire functions with the new 4-wire splitter; investigate odd setpoints seen with some boiler/thermostat combinations; update the README and credit the DS18B20 library. Also affects: needforheat-boiler-monitor-hardware. *(Trello #567, #488, #563, #406)*
- [ ] → Finish the rename from Twomes to NeedForHeat — see [needforheat-generic-firmware](https://github.com/energiegedrag-nl/needforheat-generic-firmware/blob/main/TODO.md#rename) *(Trello #1030)*
- [ ] → CONTRIBUTING.md with the branching workflow — see [needforheat-generic-firmware](https://github.com/energiegedrag-nl/needforheat-generic-firmware/blob/main/TODO.md#contrib) *(Trello #394, #1032)*
- [ ] → Fix links and errors in device manuals — see [needforheat-manuals](https://github.com/energiegedrag-nl/needforheat-manuals/blob/prd/TODO.md#manlinks) *(Trello #718, #995, #719)*
- [ ] → BoilerBASE electronics — see [needforheat-boiler-monitor-hardware](https://github.com/energiegedrag-nl/needforheat-boiler-monitor-hardware/blob/main/TODO.md#bmpcb) *(Trello #299, #657, #1054, #1053, #1056, #566)*
- [ ] → Other boiler protocols — see [needforheat-boiler-monitor-hardware](https://github.com/energiegedrag-nl/needforheat-boiler-monitor-hardware/blob/main/TODO.md#protocols) *(Trello #287)*

## Recently done

- [x] Firmware builds again in CI: include case `openTherm.h` (the OpenTherm code was never missing), Python 3.10 and current actions, updated TLS roots (#1)
