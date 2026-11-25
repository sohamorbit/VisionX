# Parts and power budget

Use one Uno R3, HC-SR04, 5 V rated digital-output reflective IR module, low-current passive piezo, normally open button, breadboard, cane, USB bank and USB cable. Add one each of red, yellow and green LEDs, three 220 Ω resistors, jumpers, a removable enclosure, straps and strain relief. See the [itemised budget](../documentation/cost-analysis.md).

| Load | 5 V planning current | Basis |
|---|---:|---|
| Uno R3 | 45–60 mA | Board allowance, revision dependent |
| HC-SR04 | 15 mA | Module datasheet working current |
| IR sensor | 10–25 mA | IR emitter and comparator allowance |
| OLED | 10–30 mA | Planning allowance; confirm against the actual module and lit pixels |
| Passive piezo | 0–10 mA | Verify actual part at its drive frequency |
| One lit LED | 8–14 mA | Forward voltage and 220 Ω series resistance |

Allow roughly 88–154 mA with a lamp on. A 5 V USB source rated for at least 500 mA has ample nominal capacity for this base circuit. Keep buzzer and LED current within individual pin ratings. Check supply voltage under sustained alarm. A higher-current active buzzer changes the estimate.

A 5000 mAh bank labelled at 3.7 V stores nominally 18.5 Wh. At an assumed 80% conversion efficiency, 14.8 Wh reaches USB. At 120 mA and 5 V, runtime is about 24.7 h. At 88–154 mA, the energy-only estimate is about 19–34 h. Aging, conversion losses and auto-shutoff may shorten it substantially.

The bank model is unspecified. To qualify it, log current with a USB meter, run the quietest normal state for at least 30 minutes and longer than its documented shutoff timer, then test critical audio and paused reminders. Repeat from low charge and run a full discharge with resets logged. Record measured runtime before advertising endurance. Use a documented continuous/low-load output if it shuts off.
