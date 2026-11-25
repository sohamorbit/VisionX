# Wiring and first power-up

Unplug USB while wiring. Connect the Uno 5V header to the positive rail and one GND header to the negative rail. Join split halves of each rail with a jumper if continuity testing shows a break. All signal voltages are relative to that same ground. Follow module silkscreen because header order varies.

| Part | Connection at the part | Connection at the Uno or rail |
|---|---|---|
| HC-SR04 | VCC / TRIG / ECHO / GND | +5 V / D6 / D5 / GND |
| 5 V rated digital IR module | VCC / DO or OUT / GND | +5 V / D2 / GND. Leave AO free. |
| Normally open button | One contact / switched contact | D3 / GND. Internal pull-up holds released input high. |
| Low-current passive piezo | + / − | D9 / GND |
| Green LED | D10 to 220 Ω to anode / cathode | D10 series circuit / GND |
| Yellow LED | D11 to 220 Ω to anode / cathode | D11 series circuit / GND |
| Red LED | D12 to 220 Ω to anode / cathode | D12 series circuit / GND |

### Placement in the full breadboard illustration

Use the [full layout](full-breadboard-layout.png) for these hole coordinates. In each numbered column, a–e and f–j are two separate groups. The compact [LED assembly guide](breadboard-wiring.png) uses an alternative placement with the same electrical connections.

| Uno | Breadboard lead | Component lead |
|---|---|---|
| D2 | a3, bridge e3 to f3 | IR OUT → j3 |
| D3 | a6, bridge e6 to f6 | Button switched contact → j6; other contact → ground rail |
| D5 | a9, bridge e9 to f9 | HC-SR04 ECHO → j9 |
| D6 | a10, bridge e10 to f10 | HC-SR04 TRIG → j10 |
| D9 | a14, bridge e14 to f14 | Piezo positive → j14; negative → ground rail |
| D10 | a18; resistor c18–g18 | Green LED anode j18, cathode j19; h19 → ground rail |
| D11 | a22; resistor c22–g22 | Yellow LED anode j22, cathode j23; h23 → ground rail |
| D12 | a26; resistor c26–g26 | Red LED anode j26, cathode j27; h27 → ground rail |

Use a separate 220 Ω resistor for each LED. Power the HC-SR04 and 5 V-rated IR module from the positive rail and return both grounds to the negative rail. OLED SDA goes to A4 and SCL goes to A5. Use the supply and logic ratings described below.

Use a continuity meter to identify two button pins that connect only when pressed. For a slide switch, connect its common contact to D3 and one throw to GND, leaving the other throw open. Set latchingSwitch=true. Open selects Standard and closed selects Precision after debounce.

## Buzzer choice

The base schematic assumes a 5 V compatible, low-current passive piezo element rated at 10 mA or less at the operating frequency. Check the part number and datasheet. A kit label alone cannot verify current or polarity. Keep an Uno I/O at or below its 20 mA operating rating. For an active buzzer set passivePiezo=false. Cadences still identify states, while the component determines pitch.

For an unknown load, identify its rated voltage and current before connection. For a 5 V buzzer drawing up to 30 mA, use a rated NPN low-side driver, e.g. 2N2222A. D9 → 1 kΩ → base, emitter → common GND, collector → buzzer negative, buzzer positive → +5 V. Verify the transistor package pinout. For a magnetic buzzer put a flyback diode across it, cathode at +5 V. A three-pin driver module may already contain this circuit. Follow its own wiring and input polarity.

## Calibrate at the bench

1. With power off check that +5 V and GND have no short. Connect a single USB supply. Observe the lamp and sound sequence. A successful sequence checks the outputs by observation.
2. Open Serial Monitor at 115200 baud. With a flat card at 150 cm expect cm near 150 and IR=0. If the IR logic is inverted, change irActive and upload again.
3. Hold a matte card 5–10 cm in front of the IR pair. Turn its trimmer slowly until IR=1. Remove the card and verify release. Repeat with dark and glossy cards at 5, 10, 20 and 30 cm under demonstration lighting. Reduce sensitivity if the far card or empty scene triggers. Record the useful range for this module.
4. Check all three LED colors, sound cues, and button states. Swap the computer for a 5 V USB power bank at the Uno USB socket. Leave direct 5V injection and the barrel jack unused.

## Four-wire OLED

Unplug USB. With the header at the bottom as in the supplied photo, the pins read SDA, SCL, VCC, GND from left to right. Follow the printed labels if you turn the board around.

| OLED pin | Uno R3 connection |
|---|---|
| SDA | A4 for 5 V logic-compatible modules |
| SCL | A5 for 5 V logic-compatible modules |
| VCC | 5V only for a module explicitly rated for 5 V supply. For a 3.3 V-only board use a rated 3.3 V supply with sufficient current. |
| GND | Common GND |

For 3.3 V-only logic, connect A4/A5 to the high side of a bidirectional I²C level shifter, SDA/SCL to its corresponding low-side channels, HV to 5V, LV to 3.3V and both grounds together. Verify onboard pull-ups are compatible with the respective bus rails. The front photo cannot establish supply or logic ratings.

Install SSD1306Ascii by Bill Greiman using Arduino IDE Library Manager and upload the updated sketch. The software assumes a 128×64 SSD1306 and probes 0x3C and 0x3D at boot. If your specification identifies a 128×64 SH1106, change displayType from &Adafruit128x64 to &SH1106_128x64. Reset after wiring changes. Look for VisionX, Echo, Limit and a status. With no echo the screen shows --. During confirmed IR detection it shows IR ALERT even if Echo shows a distant wall.
