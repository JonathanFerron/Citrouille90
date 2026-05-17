# Citrouille90 — Hardware Notebook

**Project:** Citrouille90 (AVRDU.Enc.Choc)  
**Status as of writing:** KiCad project complete and ready to send to fab. Digikey and Mouser orders staged and ready to place.  
**Document purpose:** Personal pre-build reference and permanent record. Captures all hardware decisions with rationale, the complete BOM, and assembly notes.

---

## 1. Overview

| Parameter         | Value                                              |
|-------------------|----------------------------------------------------|
| Name              | Citrouille90 (AVRDU.Enc.Choc)                      |
| Keys              | 90 (10 rows × 9 columns matrix)                    |
| Physical layout   | 5 rows × 20 columns                                |
| Switch type       | Kailh Choc V1 low-profile                          |
| Encoder           | Bourns PES12, no integrated push switch            |
| Reset             | Dedicated 6mm tact switch (separate from encoder)  |
| USB               | Full-Speed, USB Mini-B through-hole connector      |
| MCU               | AVR64DU32, TQFP-32                                 |
| PCB               | 2-layer, 387 × 126 mm                              |
| Case              | 3D-printed sandwich mount                          |
| Branding          | "Citrouille90" in Playpen Sans + waveform + pumpkin logo |
| PCB board marker  | Rev A — 2026.03                                    |
| License (PCB/case)| CERN-OHL-S v2 (strongly reciprocal)               |
| License (artwork) | CC BY-SA 4.0                                       |
| License (firmware)| GPL v3                                             |
| Copyright         | © 2026 Jonathan Ferron                             |

---

## 2. MCU

**Part:** AVR64DU32-I/PT (TQFP-32, industrial temp range)

| Parameter             | Value                              |
|-----------------------|------------------------------------|
| Flash                 | 64 KB                              |
| SRAM                  | 8 KB                               |
| Clock                 | 24 MHz internal oscillator (no crystal needed) |
| GPIO (usable)         | 23                                 |
| USB                   | Native Full-Speed, no external PHY |
| Programming interface | UPDI                               |
| VDD range             | 1.8 V – 5.5 V                      |
| USB core supply       | Internal 3.3 V regulator (VUSB)    |
| Package               | TQFP-32, 0.8 mm pitch              |

No external crystal. No external USB PHY. VUSB (3.3 V for USB core) is generated entirely on-chip from VDD — no external 3.3 V regulator needed on the PCB. The 3.3 V domain is internal to the MCU and only powers the USB phy; all GPIO and logic runs at 5 V (VDD).

---

## 3. Power Architecture

**Configuration:** USB bus powered — Datasheet Power Configuration 5b.

```
USB Host (5 V VBUS)
    │
    ├─── Polyfuse (500 mA) ───┬─── 4.7 µF bulk cap (to GND)
    │                         │
    └─────────────────────────┴─── VDD (MCU pins 18, 28)
                                        │
                                   Internal regulator
                                        │
                                      VUSB (MCU pin 6) ── 100 nF (to GND)
```

### 3.1 Capacitor Budget

USB specification: total downstream capacitance ≤ 10 µF.

| Capacitor          | Value  | Location                        |
|--------------------|--------|---------------------------------|
| Bulk VBUS          | 4.7 µF | Near USB connector, after polyfuse |
| VDD decoupling ×2  | 100 nF | At MCU VDD pins (18 and 28)     |
| VUSB decoupling    | 100 nF | At MCU VUSB pin (6), to GND only |
| ESD IC decoupling  | 100 nF | Near USBLC6-2                   |
| Reset filter cap   | 100 nF | At RESET pin (see section 8)    |
| **Total**          | **5.2 µF** | Well under 10 µF limit       |

VUSB pin is decoupled to GND only — no external load on this pin. The internal regulator is sized for the USB PHY only.

### 3.2 Polyfuse

- **Part:** Bourns PTC reset fuse, 500 mA hold, 15 V, 1812 package
- Placed on VBUS line between USB connector and bulk capacitor
- Protects host port from overcurrent faults

---

## 4. GPIO Allocation

Total usable GPIO: **23 pins** (RESET and UPDI are dedicated, not counted).

| Function         | Port / Pin  | Pin # (TQFP) | Notes                              |
|------------------|-------------|---------------|------------------------------------|
| Matrix row 0     | PA0         | 30            | Pull-up input                      |
| Matrix row 1     | PA1         | 31            | Pull-up input                      |
| Matrix row 2     | PA2         | 32            | Pull-up input                      |
| Matrix row 3     | PA3         | 1             | Pull-up input                      |
| PWM LED A        | PA4         | 2             | TCA0 WO4 output                    |
| PWM LED B        | PA5         | 3             | TCA0 WO5 output                    |
| Matrix row 4     | PA6         | 4             | Pull-up input                      |
| Encoder A        | PA7         | 5             | Plain input, no internal pull-up   |
| Encoder B        | PC3         | 9             | Plain input, no internal pull-up   |
| Matrix row 5     | PD0         | 10            | Pull-up input                      |
| Matrix row 6     | PD1         | 11            | Pull-up input                      |
| Matrix row 7     | PD2         | 12            | Pull-up input                      |
| Matrix row 8     | PD3         | 13            | Pull-up input                      |
| Matrix row 9     | PD4         | 14            | Pull-up input                      |
| Matrix col 0     | PD5         | 15            | Output, input buffer disabled      |
| Matrix col 1     | PD6         | 16            | Output, input buffer disabled      |
| Matrix col 2     | PD7         | 17            | Output, input buffer disabled      |
| Matrix col 3     | PF0         | 20            | Output, input buffer disabled      |
| Matrix col 4     | PF1         | 21            | Output, input buffer disabled      |
| Matrix col 5     | PF2         | 22            | Output, input buffer disabled      |
| Matrix col 6     | PF3         | 23            | Output, input buffer disabled      |
| Matrix col 7     | PF4         | 24            | Output, input buffer disabled      |
| Matrix col 8     | PF5         | 25            | Output, input buffer disabled      |

**Dedicated pins (not GPIO):**
- Pin 26: RESET (tact switch)
- Pin 27: UPDI (programming header)
- Pin 6: VUSB (decoupling cap only)
- Pins 18, 28: VDD
- Pins 7, 19, 29: GND

**No PORTB on AVR64DU32** — PORTB does not exist on this device.

---

## 5. USB Interface

### 5.1 Connector

- **Type:** USB Mini-B, through-hole
- **Position:** Top edge of PCB, aligned with middle of function key row

### 5.2 ESD Protection

- **Part:** USBLC6-2SC6 (SOT-23-6)
- Placed between USB connector and MCU D+/D− lines
- 100 nF decoupling cap on the IC supply pin
- No level shifting needed — PCB VDD is 5 V, MCU USB lines are 3.3 V tolerant via internal regulator

### 5.3 USB Shield RC Termination

- 1 MΩ resistor + 4.7 nF capacitor (0805, bottom layer) from USB shield to GND
- Reduces EMI from shield floating

### 5.4 Differential Pair Routing

- D+ and D− routed as matched-length differential pair
- No impedance-controlled routing required at Full-Speed (12 Mbit/s)
- Keep away from switching signals (matrix column drives, PWM)
- DRC rules: custom `.kicad_dru` with differential pair clearance and length-matching constraints

---

## 6. Key Matrix

| Parameter           | Value                                  |
|---------------------|----------------------------------------|
| Electrical layout   | 10 rows × 9 columns                    |
| Physical layout     | 5 rows × 20 columns                    |
| Switch type         | Kailh Choc V1 low-profile              |
| Diode per switch    | Yes — 1N4148W, SOD-123                 |
| Diode orientation   | COL2ROW (cathode toward column)        |
| Scan direction      | Drive column low, read row (pull-up)   |
| Scan rate           | 1000 Hz                                |
| Rollover            | 6KRO (6-key rollover + modifiers)      |

### 6.1 Diode Notes

1N4148W in SOD-123 is hand-solderable with a fine tip (T18-C2 or T18-D12) and flux. SOD-123 preferred over SOD-323 for easier hand assembly. Leaded solder (63/37, 290–315 °C).

---

## 7. Rotary Encoder

### 7.1 Part

**Bourns PES12** — mechanical rotary encoder, no integrated push switch.

| Parameter                   | Value      |
|-----------------------------|------------|
| Max contact bounce          | 5 ms       |
| Max rotation speed          | 60 RPM     |
| Detents per revolution      | 24         |
| Pulses per revolution       | 24         |
| Detent-to-detent at 60 RPM  | ~41.7 ms   |
| Quadrature transitions / detent | 4      |

### 7.2 RC Filter Circuit

Bourns-recommended RC filter on both encoder terminals A and B. This is a hardware decision that eliminates most software debounce complexity.

**Per pin:**

```
VDD (5 V) ──── 10 kΩ (pull-up) ────┬──── MCU GPIO (plain input, no internal pull-up)
                                    │
                              10 kΩ (series)
                                    │
                                Terminal (A or B)
                                    │
                              10 nF (to GND)

Terminal C ──── GND
```

**RC time constant:** 10 kΩ × 10 nF = **100 µs**

This suppresses contact bounce spikes faster than ~300–500 µs rise/fall before they reach the MCU. 100 µs is well below the 1 ms scan interval, so legitimate quadrature transitions are unaffected.

**Critical:** Configure encoder GPIO pins as plain input with **no internal pull-up**. The external 10 kΩ pull-up to VDD defines the operating point. Enabling the internal pull-up shifts the RC time constant and may degrade filtering.

### 7.3 Encoder Position

Top-left of PCB.

---

## 8. Reset Circuit

- **Switch:** 6 mm × 6 mm through-hole tact switch, SPST normally open
- **Position:** Top-right corner of PCB, silkscreen label "RESET"
- **Connection:** Switch one terminal to RESET pin (MCU pin 26), other terminal to GND

**RC filter (per AVR-DU datasheet recommendation):**

```
RESET pin (MCU pin 26) ──── 330 Ω ──── tact switch ──── GND
                │
              100 nF
                │
               GND
```

The 330 Ω series resistor limits current during ESD events. The 100 nF cap filters glitches. This is the AVR-DU datasheet recommended reset circuit.

**Behavior:** Hardware reset, always functional regardless of firmware state. Does not enter bootloader (GPR cleared on power-on reset; RESET pin behavior to be confirmed by GPR retention test on Curiosity Nano — see firmware notebook section 18.2).

---

## 9. LEDs

### 9.1 Power LED

| Parameter      | Value                              |
|----------------|------------------------------------|
| Type           | 3 mm through-hole amber, diffused  |
| Drive          | Fixed resistor from VDD — no GPIO, no PWM |
| Resistor       | 2.2 kΩ                             |
| Target current | ~1.4 mA                            |
| Brightness intent | Dim but visible in low light; dim in bright room |
| Resistor placement | Near LED, not near MCU (reduces EMI, simplifies rework) |

**Calculation:** (5 V − 2.1 V forward voltage) / 2200 Ω ≈ 1.3 mA. Conservative — this LED is purely a power-on indicator.

### 9.2 Indicator LEDs (×2)

| Parameter      | Value                              |
|----------------|------------------------------------|
| Type           | 3 mm through-hole amber, diffused  |
| Drive          | MCU GPIO with PWM (TCA0)           |
| Resistor each  | 390 Ω                              |
| Target current | ~7.5 mA at 100% duty cycle         |
| Brightness     | Firmware-controlled; clearly visible in bright room at full brightness |
| Viewing angle  | 60–100° preferred (soft glow, not a sharp spot) |

**Calculation:** (5 V − 2.1 V) / 390 Ω ≈ 7.4 mA. Reasonable for indicator use; well within MCU GPIO drive capability.

### 9.3 PWM Implementation

**Timer:** TCA0 in split mode — drives both LEDs independently from a single timer.

**Frequency:** 1465 Hz

1465 Hz is above the ~50–100 Hz flicker fusion threshold — no visible flicker at any duty cycle. PWM runs entirely in hardware; CPU has no per-cycle obligation (no matrix scan interference).

**Pin routing via TCAROUTEA (PORTMUX):**

TCA0 WO outputs available per PORTMUX setting:

| TCAROUTEA[2:0] | Port  | WO0  | WO1  | WO2  | WO3  | WO4  | WO5  |
|----------------|-------|------|------|------|------|------|------|
| 0x0            | PORTA | PA0  | PA1  | PA2  | PA3  | **PA4** | **PA5** |
| 0x3            | PORTD | PD0  | PD1  | PD2  | PD3  | PD4  | PD5  |
| 0x5            | PORTF | PF0  | PF1  | PF2  | PF3  | PF4  | PF5  |

LED A → **PA4 (TCA0 WO4)**, LED B → **PA5 (TCA0 WO5)** with TCAROUTEA = 0x0 (PORTA). This avoids conflicts with matrix rows (PA0–PA3, PA6) and encoder (PA7).

### 9.4 LED Positions

Three LEDs arranged horizontally near top edge, offset to the right. Power LED leftmost, two indicator LEDs to its right.

---

## 10. UPDI Programming Header

- **Connector:** 3-pin through-hole header (2.54 mm pitch)
- **Pinout:** GND — UPDI — VCC
- **Position:** PCB top edge (preferred for easy access)
- **Programmer:** CH340-based SerialUPDI dongle (Adafruit UPDI friend)
- **Software:** avrdude with serialupdi programmer

**Critical rule: Never disable UPDI in fuses.** UPDI is the only recovery path if firmware is corrupted or the bootloader fails. Disabling it bricks the board and the only recovery path is a 12V UPDI programmer.

---

## 11. PCB Notes

| Parameter              | Value                         |
|------------------------|-------------------------------|
| Layers                 | 2                             |
| Dimensions             | 387 × 126 mm                  |
| Mounting holes         | M2.5, 7-hole layout           |
| Silkscreen             | Minimalist; logo + "Rev A — 2026.03" board marker |
| DRC rules              | Custom `.kicad_dru` with USB differential pair clearance and length-matching |
| USB connector          | Top edge, center of function row |
| Encoder                | Top-left                      |
| LEDs                   | Top edge, right of center     |
| RESET switch           | Top-right corner              |
| UPDI header            | Top edge                      |

### 11.1 Routing Notes

- USB D+/D− as matched-length differential pair; keep away from matrix column drive lines and PWM pins
- Polyfuse on VBUS between connector and bulk cap
- ESD protection IC (USBLC6-2) placed close to USB connector, before the MCU
- 4.7 µF bulk capacitor placed near USB connector, after polyfuse
- 100 nF decoupling caps placed at their respective MCU pins, not clustered
- Resistor for power LED placed near LED (not near MCU)
- Encoder RC filter components placed close to encoder terminals, not at MCU side

---

## 12. Case

| Parameter         | Value                                               |
|-------------------|-----------------------------------------------------|
| Type              | Sandwich mount                                      |
| Material          | 3D-printed PLA or PETG                              |
| Switch plate thickness | 2.2 mm (Kailh Choc V1 spec)                   |
| Parts             | Top plate, bottom case, encoder knob                |
| CAD source        | Fusion 360 (`.f3d`), STEP export (`.step`)          |
| STL exports       | `top_plate.stl`, `bottom_case.stl`, `encoder_knob.stl` |
| Mounting          | M2.5 screws through 7-hole pattern matching PCB    |
| Feet              | Rubber adhesive feet on bottom case                |

---

## 13. Bill of Materials

### 13.1 MCU

| Qty | Description                  | Value / Part #              | Package   |
|-----|------------------------------|-----------------------------|-----------|
| 1   | AVR64DU32                    | AVR64DU32-I/PT              | TQFP-32   |

### 13.2 Switches and Diodes

| Qty | Description                  | Value / Part #              | Package   |
|-----|------------------------------|-----------------------------|-----------|
| 90  | Kailh Choc V1 switches       | CPG135001D02 (or equiv.)    | Through-hole, MX-spacing |
| 90  | Switching diodes             | 1N4148W                     | SOD-123   |

### 13.3 Encoder and Reset

| Qty | Description                  | Value / Part #              | Package   |
|-----|------------------------------|-----------------------------|-----------|
| 1   | Rotary encoder               | Bourns PES12                | Through-hole |
| 1   | Encoder knob                 | To suit PES12 shaft         | —         |
| 1   | Reset tact switch            | 6 mm × 6 mm SPST NO        | Through-hole |

### 13.4 USB

| Qty | Description                  | Value / Part #              | Package   |
|-----|------------------------------|-----------------------------|-----------|
| 1   | USB Mini-B connector         | —                           | Through-hole |
| 1   | Polyfuse                     | Bourns PTC, 500 mA, 15 V    | 1812      |
| 1   | ESD protection IC            | USBLC6-2SC6                 | SOT-23-6  |

### 13.5 Capacitors

| Qty | Description                  | Value    | Voltage | Package | Notes                        |
|-----|------------------------------|----------|---------|---------|------------------------------|
| 1   | Bulk VBUS capacitor          | 4.7 µF   | 16 V    | 1206    | X7R or X5R ceramic, near USB connector |
| 5   | Decoupling capacitors        | 100 nF   | —       | 0805    | 2× VDD (MCU), 1× VUSB (MCU), 1× ESD IC, 1× RESET filter |

### 13.6 Capacitors (Encoder Filter and USB Shield)

| Qty | Description                  | Value   | Package | Notes                                   |
|-----|------------------------------|---------|---------|-----------------------------------------|
| 2   | Encoder RC filter caps       | 10 nF   | 0805    | One per encoder pin (A, B), pin to GND  |
| 1   | USB shield RC cap            | 4.7 nF  | 0805    | USB shield to GND, with 1 MΩ           |

### 13.7 Resistors

| Qty | Description                  | Value   | Package | Notes                                   |
|-----|------------------------------|---------|---------|-----------------------------------------|
| 1   | Power LED current limit      | 2.2 kΩ  | 0805    | ~1.4 mA at 5 V                          |
| 2   | PWM LED current limit        | 390 Ω   | 0805    | ~7.4 mA at 5 V, one per indicator LED   |
| 1   | RESET series resistor        | 330 Ω   | 0805    | ESD protection on RESET pin             |
| 2   | Encoder pull-up (per pin)    | 10 kΩ   | 0805    | To VDD, part of RC filter               |
| 4   | Encoder series (per pin)     | 10 kΩ   | 0805    | Between terminal and MCU GPIO (2 per encoder pin pair — but 1 pull-up + 1 series per pin = 2 pins × 2 = 4 total) |
| 1   | USB shield termination       | 1 MΩ    | 0805    | USB shield to GND (with 4.7 nF cap)     |

> **Encoder RC resistor count clarification:** 2 pins (A and B) × 2 resistors each (1 pull-up to VDD + 1 series to MCU GPIO) = **4 resistors of 10 kΩ** total for the encoder filter.



### 13.8 LEDs

| Qty | Description                  | Value / Part #              | Package       |
|-----|------------------------------|-----------------------------|---------------|
| 1   | Power LED                    | 3 mm amber diffused         | Through-hole  |
| 2   | Indicator LEDs               | 3 mm amber diffused         | Through-hole  |

Diffused preferred over clear — softer glow, 60–100° viewing angle, less eye-fatiguing at close range.

### 13.9 Programming Header

| Qty | Description                  | Value / Part #              | Package       |
|-----|------------------------------|-----------------------------|---------------|
| 1   | UPDI header                  | 3-pin 2.54 mm male header   | Through-hole  |

### 13.10 Hardware

| Qty | Description                  | Notes                                             |
|-----|------------------------------|---------------------------------------------------|
| 7   | M2.5 screws                  | Length to suit plate + PCB + case stack           |
| 7   | M2.5 nuts or standoffs       | Matching                                          |
| 4+  | Rubber adhesive feet         | Applied to bottom of case                         |
| 90  | Kailh Choc-compatible keycaps | MBK, CFX, or similar Choc-spacing profile        |

---

## 14. Assembly Notes

### 14.1 Soldering Notes

- Leaded solder preferred: 63/37, 290–315 °C
- SMD diodes (SOD-123): T18-C2 or T18-D12 tip, flux generously
- TQFP-32 MCU: drag solder with flux, wick bridges immediately
- ESD IC (SOT-23-6): small pads, flux and fine tip
- Through-hole components (switches, encoder, LEDs, reset switch, UPDI header): standard iron, 350 °C

### 14.2 Assembly Order (recommended)

1. SMD passives first (resistors, capacitors) — before through-hole, easier to access pads
2. ESD IC (USBLC6-2) and polyfuse
3. MCU (TQFP-32) — inspect under magnification, check for bridges before powering
4. USB connector
5. Reset tact switch and UPDI header
6. LEDs (check orientation — anode/cathode markings)
7. Encoder
8. Diodes (90×) — check cathode band direction (COL2ROW: cathode toward column pin)
9. Kailh Choc switches last (after all SMD and smaller TH components)

### 14.3 First Power-On Checklist

Before loading any firmware:
- [ ] Verify 5 V at VDD test point (no short to GND)
- [ ] Verify power LED illuminates
- [ ] Connect UPDI programmer, confirm avrdude detects AVR64DU32
- [ ] Read device signature to confirm correct MCU
- [ ] Flash test firmware (blink LED, confirm PWM pins respond)
- [ ] Test matrix continuity (short each switch, check scan output)
- [ ] Test encoder (confirm A/B pulses reach MCU pins)
- [ ] Test reset button (confirm system resets cleanly)

### 14.4 UPDI Recovery

If firmware is corrupted and USB is non-functional, UPDI is the only recovery path. Always keep the SerialUPDI programmer accessible.

---

## 15. Open Hardware Items

| Item | Status | Notes |
|------|--------|-------|
| Encoder knob selection | 🔧 Pending | Choose to suit PES12 shaft spec |
| 3D case design | 🔧 Pending | Switch plate 2.2 mm; Fusion 360 source to be created |
