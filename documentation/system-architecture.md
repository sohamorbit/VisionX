# Signal path and operating rules

HC-SR04 echo → Uno D5 edge capture → pulse validation → range estimate and median → acoustic risk band. IR digital output → D2 → three consecutive samples → close-object override. Button → D3 pull-up → 25 ms debounce → range and monitoring state. Combined state → buzzer D9, observer LEDs D10–D12 and USB serial diagnostics.

The Uno starts a 12 µs trigger every 65 ms. A pin-change interrupt timestamps both echo edges while the main loop runs. Echo duration divided by 58 gives nominal centimetres. The sketch rounds to the nearest centimetre. The 100 cm boundary is a threshold on that estimate. Air temperature, alignment and quantisation affect physical accuracy. Missing edges after 26 ms and pulses outside the nominal 2–400 cm envelope clear the history and yield unknown. The next valid reading starts a fresh history.

The latest value is used until three valid samples are available. Thereafter a median rejects isolated ordinary spikes. A valid reading at or below 10 cm bypasses the median. A raw reading beyond the selected range releases the ordinary warning immediately. Inside the selected range, a retreating target must move more than 2 cm beyond an internal threshold before the band relaxes. The hard outer range takes precedence. A drop of at least 3 cm between successive filtered estimates sets the serial nearer flag for that observation.

| Standard-mode estimate | Band code | Buzzer behaviour | Observer LED |
|---|---|---|---|
| Above 100 cm | 0 clear | Silent | Green |
| 76–100 cm | 1 low | 45 ms every 1000 ms | Green |
| 51–75 cm | 2 moderate | 45 ms every 500 ms | Yellow |
| 26–50 cm | 3 high | 45 ms every 250 ms | Red |
| 11–25 cm | 4 danger | 45 ms every 100 ms | Red, 400 ms cycle |
| 2–10 cm | 5 critical | Continuous | Red, 160 ms cycle |
| Invalid echo | 6 unknown | Two short ticks every 2.4 s | Yellow, 800 ms cycle |
| Confirmed IR | 7 immediate | 90 ms on, 30 ms off | Red |

IR needs three identical 8 ms samples to assert or release. It measures reflected intensity relative to a trimmer setting. It supplies no centimetre value. Confirmed IR wins over distant and invalid echoes in all monitoring modes. With IR inactive, the echo alone determines the state. A disconnected IR lead may resemble an idle sensor. Startup exercises the outputs and starts sampling. It cannot certify alignment, sound audibility or sensor health.

Tap and release to cycle Standard (100 cm), Indoor (60 cm), Precision (30 cm) and back. Retain the same bands inside each range. The new mode gives one, two or three short beeps. A 1.2 s hold toggles monitoring once per press. Pause gives a long tone and a 90 ms reminder every five seconds with a brief yellow flash every two seconds. Resume gives two fast chirps. Danger, critical and IR alerts cancel any outstanding cue when monitoring is enabled. Startup plays two short tones and sequences green, yellow and red when no urgent obstacle intervenes.

The OLED mirrors filtered echo distance, the active range limit and the fused warning state when values change, evaluated every 250 ms. It uses the A4/A5 I²C bus. A missing display at boot leaves audio monitoring active.
