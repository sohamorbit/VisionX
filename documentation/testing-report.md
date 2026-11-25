# Firmware verification and bench test record

Arduino IDE 1.8.19 arduino-builder compiled the unmodified sketch for arduino:avr:uno. Flash 11,286 / 32,256 bytes and static RAM 553 / 2,048 bytes. AVR8js 0.21.1 ran the compiled HEX at 16 MHz with Timer0, Timer1, Timer2, GPIO pin changes and UART. The fixture acknowledges the OLED at 0x3C and counts data writes. The workshop photos separately document the assembled hardware and its displayed readout. The fixture generates echo edges in response to D6 and drives D2 and D3. Observations below come from UART or physical-pin models. Simulated time excludes real sound propagation, acoustic sensitivity and user response.

| Stimulus | Expected | Actual observation | Status |
|---|---|---|---|
| Reset in Standard | LEDs 1, 2, 4 and sound edges, range 100 | LED masks 1,0,2,4,5 / 350 sound edges / 100 | PASS |
| 150 cm | band 0 | cm=150 med=150 IR=0 band=0 range=100 on=1 nearer=0 | PASS |
| 150 cm audio | silent | 0 edges | PASS |
| 101 cm | band 0 | cm=101 med=101 IR=0 band=0 range=100 on=1 nearer=0 | PASS |
| 101 cm audio | silent | 0 edges | PASS |
| 100 cm | band 1 | cm=100 med=100 IR=0 band=1 range=100 on=1 nearer=0 | PASS |
| 100 cm audio | 1000 ms | 999 ms | PASS |
| 75 cm | band 2 | cm=75 med=75 IR=0 band=2 range=100 on=1 nearer=0 | PASS |
| 75 cm audio | 500 ms | 500 ms | PASS |
| 50 cm | band 3 | cm=50 med=50 IR=0 band=3 range=100 on=1 nearer=0 | PASS |
| 50 cm audio | 250 ms | 250 ms | PASS |
| 25 cm | band 4 | cm=25 med=25 IR=0 band=4 range=100 on=1 nearer=0 | PASS |
| 25 cm audio | 100 ms | 99 ms | PASS |
| 10 cm | band 5 | cm=10 med=10 IR=0 band=5 range=100 on=1 nearer=0 | PASS |
| 10 cm audio | continuous | 16347 edges | PASS |
| 5 cm | band 5 | cm=5 med=5 IR=0 band=5 range=100 on=1 nearer=0 | PASS |
| 5 cm audio | continuous | 16346 edges | PASS |
| Remove target | band 0 | cm=150 med=150 IR=0 band=0 range=100 on=1 nearer=0 | PASS |
| IR latency; distant echo | first sound within 26 ms | 24.46 ms | PASS |
| Confirmed IR plus distant echo | band 7 | cm=150 med=150 IR=1 band=7 range=100 on=1 nearer=0 | PASS |
| IR plus timeout | band 7 | cm=-1 med=-1 IR=1 band=7 range=100 on=1 nearer=0 | PASS |
| Missing echo | band 6 | cm=-1 med=-1 IR=0 band=6 range=100 on=1 nearer=0 | PASS |
| Sub-minimum echo | band 6 | cm=-1 med=-1 IR=0 band=6 range=100 on=1 nearer=0 | PASS |
| Echo recovery | band 0 | cm=150 med=150 IR=0 band=0 range=100 on=1 nearer=0 | PASS |
| Short press | range 60 | 60 | PASS |
| Short press | range 30 | 30 | PASS |
| Short press | range 100 | 100 | PASS |
| Long press | on=0 | cm=150 med=150 IR=0 band=0 range=100 on=0 nearer=0 | PASS |
| Resume | on=1 | cm=150 med=150 IR=0 band=0 range=100 on=1 nearer=0 | PASS |
| Step 150 to 5 cm | first sound within 70 ms | 16.20 ms | PASS |
| Retreat 10 to 11 cm | band 5 | cm=11 med=11 IR=0 band=5 range=100 on=1 nearer=0 | PASS |
| Retreat beyond 2 cm margin | band 4 | cm=13 med=13 IR=0 band=4 range=100 on=1 nearer=0 | PASS |
| Clear before mode tests | band 0 | cm=150 med=150 IR=0 band=0 range=100 on=1 nearer=0 | PASS |
| Indoor at 60 cm | band 2 | cm=60 med=60 IR=0 band=2 range=60 on=1 nearer=0 | PASS |
| Indoor at 61 cm | band 0 | cm=61 med=61 IR=0 band=0 range=60 on=1 nearer=0 | PASS |
| Precision at 30 cm | band 3 | cm=30 med=30 IR=0 band=3 range=30 on=1 nearer=0 | PASS |
| Precision at 31 cm | band 0 | cm=31 med=31 IR=0 band=0 range=30 on=1 nearer=0 | PASS |
| Ignore 5 ms IR disturbance | band 0 | cm=31 med=31 IR=0 band=0 range=30 on=1 nearer=0 | PASS |
| Steady OLED | no redundant writes | 0 | PASS |
| Changed OLED | data written after distance change | 28 | PASS |

Codes 0–7 mean clear, low, moderate, high, danger, critical, unknown and IR override. LED mask bits 1, 2 and 4 denote green, yellow and red. A zero mask and transient combined masks can appear between sequential pin writes. The latency observations are single phase-specific trials, not worst-case guarantees. A target appearing just after a ping may wait almost 65 ms for the next trigger. Noncritical median decisions may require another sample.

The Wokwi CLI lint check passed. Hosted Wokwi execution required a token unavailable in this environment. The digital IR substitute exercises logic only. The team photographed a physical bench prototype. The emulation run used no connected device; controlled object trials and power-bank endurance are still pending.

## Physical trials awaiting hardware

Clamp the stick at its normal holding angle. Measure from the transducer face. Use three approaches and withdrawals per object at 100, 75, 50, 25 and 10 cm, logging raw distance, band, IR, light, angle and misses. Add 5 cm and the 101/150 cm controls. Record the measured distance at the first audible alert and whether warnings stop on valid removal. Use a scope or logic analyser on TRIG, ECHO and D9 for latency. Keep the observer stationary.

| Target | Expected behaviour or question | Actual detection | Status |
|---|---|---|---|
| Wall | Broad frontal return and repeatable bands | Unmeasured | Not run |
| Cardboard box | Broad face tracks; compare an edge | Unmeasured | Not run |
| Chair | Compare wide back and narrow legs | Unmeasured | Not run |
| Human-sized target | Compare a flat torso target and soft coat | Unmeasured | Not run |
| Hanging object | Alert only if target enters the forward beam | Unmeasured | Not run |
| Thin pole or stick | May miss between sweeps | Unmeasured | Not run |
| Matte black object | IR range may shorten | Unmeasured | Not run |
| Reflective object | Compare normal and oblique incidence | Unmeasured | Not run |
| Angled board | Echo may reflect away from receiver | Unmeasured | Not run |

Repeat IR calibration indoors and in sunlight. Check the cane balances without twisting, the tip and handle remain usable, the bank stays awake, and alarms are recognisable at a comfortable volume. Record actual outcomes before making any field-performance claim.

## Earlier display-layout spot check

For a 42 cm input with IR idle, an additional emulator check reconstructed the display page data from outgoing I²C bytes. The decoded text was:

```text
VisionX
Echo:  42 cm
Limit: 100 cm
HIGH
```

This verifies the transmitted text. Optical visibility on the physical panel still requires the hardware test.
