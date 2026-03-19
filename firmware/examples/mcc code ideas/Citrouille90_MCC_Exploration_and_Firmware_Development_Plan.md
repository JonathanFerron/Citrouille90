# Citrouille90 MCC Exploration and Firmware Development Plan

## Overview

This document outlines the systematic approach for exploring MCC-generated code, comparing it with other frameworks (DxCore, LUFA, TinyUSB, QMK, and a ClaudeAI-generated draft), and building a harmonious, minimalistic, bare-metal firmware for the AmberClick90 keyboard.

**Core Philosophy:**
- Bare metal, minimalistic approach
- Easily readable and maintainable code
- Internal harmony - no Frankenstein assemblies
- Learn from multiple sources, choose the best ideas intentionally

---

## MCC Code Generation Sequence

Generate MCC code in incremental complexity steps, each in its own folder for side-by-side comparison and diff analysis.

### Primary Sequence

1. **Bare metal blink**
   - Basic AVR64DU32 startup
   - Clock configuration (24 MHz)
   - Simple GPIO toggle
   - Goal: Understand initialization and basic GPIO

1b. **Matrix scanning only**
   - 10 rows × 9 columns GPIO setup
   - Matrix scan algorithm
   - UART debug output for key states
   - No USB - isolate matrix scanning code
   - Goal: Understand matrix scanning independently

1c. **Encoder only**
   - Quadrature decoder (EC12 encoder)
   - GPIO configuration with pull-ups
   - UART debug output for rotation events
   - No USB - isolate encoder state machine
   - Goal: Understand encoder implementation independently

2. **PWM LED pulse**
   - TCA timer configuration
   - PWM generation for 2 indicator LEDs (PA4, PA5)
   - Goal: Understand timer/PWM setup

3. **USB Vendor (Control EP0 only)**
   - USB peripheral initialization
   - Endpoint 0 control transfers
   - Vendor-defined class
   - Goal: Understand basic USB enumeration

4. **USB Vendor (Bulk transfers)**
   - Add bulk endpoints
   - Goal: Understand bulk transfer implementation

5. **USB HID Boot Keyboard**
   - Single interface, boot protocol
   - Standard 8-byte keyboard report
   - Goal: Understand HID keyboard basics

5b. **USB HID Boot Keyboard + Encoder**
   - Integrate encoder with HID keyboard
   - Encoder → HID report mapping
   - Goal: Understand peripheral integration with USB

6. **USB HID Report Keyboard**
   - Report protocol keyboard
   - Goal: Understand report vs boot protocol differences

7. **USB HID Boot Mouse + Report Keyboard**
   - Composite device (2 interfaces)
   - Boot mouse + report keyboard
   - Goal: Understand composite device descriptors

8. **USB HID Composite Full**
   - All 3 interfaces per AmberClick90 spec:
     - Interface 0: Boot Keyboard
     - Interface 1: Boot Mouse
     - Interface 2: Extended Controls (Consumer + System)
   - 3 IN endpoints
   - Goal: Understand complete multi-interface USB implementation

8b. **USB Composite + Matrix + Encoder (minimal)**
   - Full USB stack (3 interfaces)
   - Simplified 3×3 matrix instead of 10×9
   - Encoder integration
   - Goal: Understand full integration without complexity of full matrix

9. **Full feature set**
   - Complete AmberClick90 implementation
   - All 3 USB interfaces
   - Full 10×9 matrix
   - Encoder
   - 2 PWM LEDs
   - Goal: Complete reference implementation

### Future Exploration (Optional)

- **USB CDC** - Serial port for debugging (not in final product)
- **Interrupt vs Polling** - Compare code complexity and performance

---

## Folder Organization

```
mcc-exploration/
├── 01-bare-blink/
├── 01b-matrix-only/
├── 01c-encoder-only/
├── 02-pwm-pulse/
├── 03-usb-vendor-control/
├── 04-usb-vendor-bulk/
├── 05-usb-hid-boot-kbd/
├── 05b-usb-kbd-encoder/
├── 06-usb-hid-report-kbd/
├── 07-usb-composite-kbd-mouse/
├── 08-usb-composite-full/
├── 08b-usb-composite-minimal/
└── 09-full-feature/
```

---

## Analysis Approach

For each MCC variation:

### 1. Generate with MCC
- Configure peripherals in MCC GUI
- Generate code
- Document MCC settings (screenshots/exports)

### 2. Build and Measure
- Compile the generated code
- Record flash usage (bytes and %)
- Record RAM usage (bytes and %)
- Note any warnings or issues

### 3. Code Review
Focus on:
- **Initialization sequence** - What happens at startup?
- **Peripheral configuration** - Which registers are set to what values?
- **Interrupt handlers vs polling** - What's the execution model?
- **USB descriptor structure** - How are descriptors organized?
- **State machines** - How is state tracked?
- **Buffer management** - How are buffers allocated and used?
- **Error handling** - How are errors detected and handled?

### 4. Create Comparison Notes
For each step, document:
- What new files appeared?
- What changed in existing files?
- What registers are being configured?
- What's the code size delta from previous step?
- What abstractions/wrappers were added?

### 5. Diff Analysis
Use diff tools to compare:
- Files with same name across different variations
- Identify what MCC adds for each feature
- Spot patterns in code generation

---

## Tracking Spreadsheet Structure

### Tab 1: Feature Comparison Matrix

**Purpose:** Compare how each framework implements each feature

**Columns:**
- Feature/Component
- MCC Notes
- DxCore Notes
- LUFA Notes
- TinyUSB Notes
- QMK Notes
- ClaudeAI Draft Notes
- My Chosen Approach
- Implementation Status (Not Started / In Progress / Done / Tested)
- File Location (where this lives in final project)

**Row Categories:**

#### Startup & Core
- Clock configuration (24 MHz)
- Fuse settings
- Startup code (.init sections)
- Main loop structure
- Watchdog timer

#### GPIO & Pins
- GPIO initialization pattern
- Port direction setting
- Pin read/write methods
- Pull-up configuration

#### Matrix Scanning
- Row/column GPIO setup
- Scan algorithm (row-driven vs column-driven)
- Debouncing approach
- Scan timing/frequency
- Key state storage

#### Encoder
- GPIO configuration (pull-ups)
- Quadrature decoder algorithm
- State machine implementation
- Debounce/filtering
- Integration with main loop

#### USB Core
- USB peripheral initialization
- Clock configuration for USB
- VBUS detection (if any)
- Endpoint buffer allocation
- Descriptor storage location

#### USB Descriptors
- Device descriptor
- Configuration descriptor
- Interface descriptors (×3)
- Endpoint descriptors
- HID descriptors
- String descriptors
- Descriptor management (hardcoded vs struct-based)

#### USB Enumeration
- Enumeration state machine
- Standard requests (GET_DESCRIPTOR, SET_ADDRESS, etc.)
- Configuration handling
- Suspend/Resume

#### USB HID Specific
- HID class requests (GET_REPORT, SET_IDLE, etc.)
- Boot protocol support
- Report protocol support
- Report descriptor structure
- Report sending mechanism

#### USB Endpoints
- EP0 control transfer handling
- Bulk endpoint implementation
- Interrupt endpoint implementation
- Endpoint callback/event model

#### HID Reports
- Boot keyboard report structure
- Boot mouse report structure
- Extended controls report structure
- Report composition/packing
- Report sending (when/how)

#### Timers
- TCA configuration (PWM)
- Timer overflow interrupt
- PWM duty cycle control
- Timing for scanning/debounce

#### LEDs
- PWM LED control (PA4, PA5)
- Brightness adjustment
- LED state tracking

#### Bootloader
- Entry mechanism (software reset)
- Vendor class implementation
- Flash write operations
- Application jump

---

### Tab 2: Code Size Tracking

**Purpose:** Track code size impact of each feature addition

**Columns:**
- Variation Name (1, 1b, 1c, 2, 3, etc.)
- Flash Used (bytes)
- Flash Used (%)
- RAM Used (bytes)
- RAM Used (%)
- Delta from Previous (Flash)
- Delta from Previous (RAM)
- Notes (what changed)

**Usage:** Understand the cost of each feature, but remember: if a feature needs to be there, it needs to be there. This is for understanding, not necessarily optimization.

---

### Tab 3: File Structure Comparison

**Purpose:** Map where different frameworks organize similar functionality

**Columns:**
- File Category
- MCC File(s)
- DxCore File(s)
- LUFA File(s)
- TinyUSB File(s)
- QMK File(s)
- ClaudeAI Draft File(s)
- My Project File(s)

**Categories:**
- USB Descriptors
- USB Stack Core
- HID Reports
- Matrix Scanning
- Encoder Handling
- Timer/PWM
- GPIO/HAL
- Initialization
- Main Loop
- Utilities

---

### Tab 4: Register Configuration Reference

**Purpose:** Document actual hardware register settings

**Columns:**
- Peripheral/Feature
- Register Name
- MCC Value/Setting
- DxCore Value/Setting
- LUFA Value/Setting
- TinyUSB Value/Setting
- QMK Value/Setting
- ClaudeAI Draft Value/Setting
- Datasheet Recommendation
- My Final Value
- Rationale/Notes

**Key Registers to Track:**
- USB.CTRLB, USB.CTRLA
- PORT direction registers
- TCA period/compare values
- Clock configuration registers
- Fuse settings
- etc.

---

### Tab 5: Implementation Checklist

**Purpose:** Simple task tracker

**Columns:**
- Task/Feature
- Priority (Must Have / Nice to Have / Future)
- Status
- Blockers/Notes
- Related Code Files

---

### Tab 6: HAL Design Philosophy

**Purpose:** Document naming conventions, abstraction patterns, and API design to maintain harmony across the codebase

This tab captures the "feel" and consistency of each approach to avoid creating a Frankenstein codebase when mixing ideas.

#### Section 1: Naming Conventions

**Columns:**
- Naming Element
- MCC Pattern
- DxCore Pattern
- LUFA Pattern
- TinyUSB Pattern
- QMK Pattern
- ClaudeAI Draft Pattern
- My Decision
- Rationale

**Rows:**
- Pin/Port naming (e.g., `LED_A_PIN` vs `INDICATOR_LED_1` vs `pin_led_indicator_a`)
- Function naming style (e.g., `led_set()` vs `LED_Set()` vs `SetLED()`)
- Macro naming (e.g., `LED_ON()` vs `led_on()` vs `SET_LED_STATE()`)
- Constant naming (e.g., `LED_BRIGHTNESS_MAX` vs `MAX_LED_BRIGHTNESS`)
- Enum naming (e.g., `enum led_state` vs `LED_STATE_T` vs `led_state_e`)
- Struct naming (e.g., `struct led_config` vs `led_config_t` vs `LedConfig`)
- File naming (e.g., `led_hal.c` vs `hal_led.c` vs `led.c`)
- Include guard style (e.g., `_LED_HAL_H_` vs `LED_HAL_H` vs `HAL_LED_H_`)

#### Section 2: Abstraction Philosophy

**Columns:**
- Abstraction Layer
- MCC Approach
- DxCore Approach
- LUFA Approach
- TinyUSB Approach
- QMK Approach
- ClaudeAI Draft Approach
- My Decision
- Why

**Rows:**
- Hardware abstraction depth (direct register vs wrapper functions)
- Pin definition style (port/bit vs pin number vs abstract name)
- Configuration approach (runtime structs vs compile-time macros)
- State management (global vars vs context structs vs static locals)
- Error handling (return codes vs asserts vs silent fail)
- Initialization pattern (explicit init calls vs auto-init)

#### Section 3: API Design Patterns

**Columns:**
- API Pattern
- MCC Example
- DxCore Example
- LUFA Example
- TinyUSB Example
- QMK Example
- ClaudeAI Draft Example
- My Choice
- Notes

**Rows:**
- Setter pattern (`led_set_brightness(LED_A, 50)` vs `LED_A_brightness = 50`)
- Getter pattern (`led_get_brightness(LED_A)` vs `LED_A.brightness`)
- Toggle/action pattern (`led_toggle(LED_A)` vs `LED_TOGGLE(A)`)
- Multi-parameter functions vs config structs
- Use of enums vs #defines for constants
- Callback/event model (if applicable)
- Initialization parameter passing

#### Section 4: Code Organization

**Columns:**
- Organization Aspect
- MCC
- DxCore
- LUFA
- TinyUSB
- QMK
- ClaudeAI Draft
- My Decision

**Rows:**
- Header/source file split (what goes in .h vs .c)
- Public vs private function separation
- Inline vs separate function approach
- Use of static inline helpers
- Macro placement (header vs source)
- Configuration file location/style
- Hardware-specific vs portable code separation

#### Section 5: Documentation Style

**Columns:**
- Documentation Element
- MCC Style
- DxCore Style
- LUFA Style
- TinyUSB Style
- QMK Style
- ClaudeAI Draft Style
- My Style

**Rows:**
- Function comment format (Doxygen vs plain vs none)
- Register/bit explanations in comments
- Example usage in comments
- File header content
- Section separators/organization markers
- Inline comments verbosity
- Magic number explanation

#### Section 6: Specific HAL Examples

Work through concrete examples to test conventions.

**LED HAL Example:**

Pin Definition Strategy:
```c
MCC:        #define LED_INDICATOR_A_PIN  PIN4_bm
            #define LED_INDICATOR_A_PORT PORTA

DxCore:     #define PIN_LED_A  PIN_PA4

LUFA:       #define LED_A_PORT PORTA
            #define LED_A_PIN  4

My choice:  [TBD - which feels most readable/maintainable?]
```

Basic Operations:
```c
MCC:        LED_SetHigh(LED_INDICATOR_A);
            LED_SetLow(LED_INDICATOR_A);

DxCore:     digitalWrite(PIN_LED_A, HIGH);
            digitalWrite(PIN_LED_A, LOW);

LUFA:       LED_A_PORT.OUTSET = LED_A_PIN;
            LED_A_PORT.OUTCLR = LED_A_PIN;

My choice:  [TBD]
```

PWM Control:
```c
MCC:        PWM_SetDutyCycle(PWM_LED_A, 127);

DxCore:     analogWrite(PIN_LED_A, 127);

TinyUSB:    tca_set_pwm(TCA_WO4, 127);

My choice:  [TBD]
```

Initialization:
```c
MCC:        LED_Initialize();
            PWM_Initialize();

DxCore:     pinMode(PIN_LED_A, OUTPUT);
            analogWrite(PIN_LED_A, 0);

My choice:  [TBD]
```

#### Section 7: Consistency Rules

Checklist to ensure harmony across codebase:

- [ ] All pin names follow same pattern (port+number vs abstract)
- [ ] All function names use same case convention
- [ ] All initialization follows same pattern
- [ ] All error handling follows same approach
- [ ] All state management uses consistent storage
- [ ] All configuration uses same mechanism
- [ ] All documentation uses same style
- [ ] All file organization follows same structure

#### Section 8: Mix-and-Match Matrix

When combining ideas from different sources, document compatibility:

**Columns:**
- Feature Combination
- Potential Conflict
- Resolution Strategy
- Test Status

**Example Rows:**
- "MCC USB stack + DxCore pin abstraction" → "Naming style clash" → "Wrap MCC USB in my naming" → "Tested OK"
- "LUFA HID reports + QMK matrix scan" → "Different state storage" → "Unified state struct" → "In progress"

---

### Tab 7: KeyCode System Design

**Purpose:** Define the vocabulary and organization of keycodes - this is the user-facing API of the firmware

This is a comprehensive single tab covering all aspects of keycode design.

#### Section 1: KeyCode Namespace & Ranges

**Columns:**
- Aspect
- MCC Approach
- DxCore Approach
- LUFA Approach
- TinyUSB Approach
- QMK Approach
- ClaudeAI Draft Approach
- My Decision
- Rationale

**Rows:**
- Base type (enum vs #define vs const)
- Value range allocation strategy
- HID usage code representation
- Special keycode range start
- Layer key range
- Macro key range
- Modifier representation
- Mouse key range (if applicable)
- Consumer control range
- Reserved/invalid code handling

**Example Documentation:**
```
QMK:
  - Uses #define with range-based allocation
  - 0x00-0xFF: Basic HID codes
  - 0x5C-0x5F: Layer tap
  - 0x7E00-0x7E3F: Layer momentary
  - QK_KB_0 through QK_KB_31 for user/keyboard specific

LUFA:
  - Direct HID usage codes (simpler)
  - Separate enums for different HID pages
  - No built-in layer system

TinyUSB:
  - HID_KEY_* constants matching HID spec exactly
  - No special keycodes (library level only)

My decision: [TBD - do I want QMK-style range encoding or separate types?]
```

#### Section 2: KeyCode Naming Conventions

**Columns:**
- Naming Pattern
- MCC Example
- DxCore Example
- LUFA Example
- TinyUSB Example
- QMK Example
- ClaudeAI Draft Example
- My Choice
- Notes

**Rows:**

*Basic Keys:*
- Alphabetic (A-Z)
- Numbers (0-9, keypad)
- Function keys (F1-F24)
- Modifiers (Ctrl, Shift, Alt, GUI)
- Navigation (arrows, Home, End, etc.)
- Editing (Insert, Delete, Backspace)
- Punctuation/symbols

*Special Functions:*
- Layer switching
- Transparent key
- No-op key
- Bootloader entry
- Reset
- Macro triggers
- Mouse actions
- Media keys
- System keys (sleep, power)

**Example Documentation:**
```
QMK:
  KC_A, KC_B, KC_1, KC_2
  KC_LCTL, KC_LSFT
  KC_F1, KC_F2
  MO(layer), TG(layer), LT(layer, kc)
  KC_TRNS (transparent)
  KC_NO (no-op)
  QK_BOOT (bootloader)
  M(macro_id) or MACRO_1
  KC_MS_UP, KC_MS_BTN1
  KC_MUTE, KC_VOLU

LUFA:
  HID_KEYBOARD_SC_A, HID_KEYBOARD_SC_B
  HID_KEYBOARD_SC_LEFT_CONTROL
  (No layer system - library level only)

TinyUSB:
  HID_KEY_A, HID_KEY_B
  HID_KEY_CONTROL_LEFT
  HID_KEY_F1
  (No special functions - library level only)

My decision: [Which prefix? KC_ vs KEY_ vs K_? Or no prefix?]
```

#### Section 3: KeyCode Organization Strategy

**Columns:**
- Organizational Aspect
- MCC
- DxCore
- LUFA
- TinyUSB
- QMK
- ClaudeAI Draft
- My Approach

**Rows:**
- File structure (where keycodes are defined)
- Header dependencies
- User keymap location
- Default keymap location
- Macro definitions location
- Layer definitions location
- Enum vs #define choice
- Grouping/sectioning style
- Comments/documentation density

**Example:**
```
QMK:
  quantum/quantum_keycodes.h - all QMK keycodes
  keyboards/[keyboard]/keymaps/[user]/ - user keymaps
  Heavily documented with ranges
  Uses both enum and #define

LUFA:
  LUFA/Drivers/USB/Class/Common/HIDClassCommon.h
  Direct HID usage codes
  Minimal comments (assumes HID spec knowledge)
  Pure #defines

My decision:
  src/keycodes.h - all keycodes centralized?
  Or src/keycodes/hid.h, src/keycodes/special.h split?
```

#### Section 4: Layer System Integration

**Columns:**
- Layer Concept
- MCC Implementation
- DxCore Implementation
- LUFA Implementation
- TinyUSB Implementation
- QMK Implementation
- ClaudeAI Draft Implementation
- My Design

**Rows:**
- Layer activation mechanism
- Layer keycode encoding
- Momentary layer (hold for layer)
- Toggle layer (tap to switch)
- Layer tap (tap=key, hold=layer)
- Default layer setting
- Maximum layer count
- Layer state storage
- Transparent key behavior
- Layer priority/fallthrough

**Example:**
```
QMK:
  MO(n) - momentary layer n while held
  TG(n) - toggle layer n on/off
  LT(n, kc) - tap=kc, hold=layer n
  Encoded in keycode value itself (upper bits)
  32 layers supported
  uint32_t layer_state

LUFA:
  No built-in layer system
  Would implement in application code

My decision:
  Do I encode layer info in keycode (QMK style)?
  Or separate layer_action_t type?
  How many layers do I need? (spec mentions Layer 0-3)
```

#### Section 5: Special KeyCode Types

Document each special keycode category in detail.

**Macro Keys:**
```
QMK:
  M(0), M(1), M(2)... or MACRO_0, MACRO_1
  Macro IDs stored in keycode
  Macro definitions in separate array

My decision:
  MACRO(id) function-like?
  Or MC_VOL_UP discrete codes?
  How many macro slots?
  Where defined - keymap or separate file?
```

**Layer Keys:**
```
QMK:
  Multiple types: MO, TG, TT, LM, LT, OSL, etc.
  Very flexible but complex

My decision:
  Do I need all these variants?
  Just MO and TG for now?
  Keep it simple initially?
```

**Mouse Keys:**
```
QMK:
  KC_MS_UP, KC_MS_DOWN, KC_MS_LEFT, KC_MS_RIGHT
  KC_MS_BTN1, KC_MS_BTN2, KC_MS_BTN3
  KC_MS_WH_UP, KC_MS_WH_DOWN

My decision:
  Mouse keycodes for boot mouse interface?
  Encoder handling separate?
```

**System/Special:**
```
QMK:
  QK_BOOT - enter bootloader
  QK_REBOOT - reset keyboard
  QK_DEBUG_TOGGLE - toggle debug
  KC_TRANSPARENT - pass through
  KC_NO - do nothing

My decision:
  BOOT, RESET naming?
  TRNS vs ___ symbol?
  NO vs XXX symbol?
```

#### Section 6: Keymap Definition Syntax

What users actually write in their keymaps.

**Columns:**
- Syntax Element
- MCC Style
- DxCore Style
- LUFA Style
- TinyUSB Style
- QMK Style
- ClaudeAI Draft Style
- My Choice
- Example

**Rows:**

*Layer array structure:*
```
QMK:
  const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [0] = LAYOUT_90(
      KC_ESC, KC_1, KC_2, ...
    ),
  };

Alternative:
  const keycode_t keymaps[NUM_LAYERS][MATRIX_ROWS][MATRIX_COLS] = {
    [LAYER_BASE] = {
      {KC_ESC, KC_1, KC_2, ...},
      ...
    },
  };
```

*Visual helpers:*
```
QMK:
  Uses LAYOUT_* macro for visual representation
  #define LAYOUT_90(...) { formatted layout }

My decision:
  Do I want visual ASCII layout in keymap?
  Or just array syntax?
```

*Readability aliases:*
```
QMK:
  #define _______ KC_TRNS
  #define XXXXXXX KC_NO
  Makes keymaps more scannable

My decision:
  Use symbols or words?
  ___ or TRNS?
```

#### Section 7: HID Report Mapping

How keycodes become USB reports.

**Columns:**
- Mapping Aspect
- MCC
- DxCore
- LUFA
- TinyUSB
- QMK
- ClaudeAI Draft
- My Implementation

**Rows:**
- Basic HID keycode translation
- Modifier handling
- Consumer control translation
- System control translation
- Mouse button mapping
- Multi-interface report routing
- Report buffer management

**Example:**
```
QMK:
  Has keycode_to_system() helper
  Has keycode_to_consumer() helper
  Checks keycode ranges to route to correct interface

LUFA:
  Direct mapping - app code handles translation

My decision:
  Do I need translation helpers?
  Or direct HID usage codes in keymap?

  Option A: User keymaps use HID codes directly
    Pro: Simple, no translation
    Con: User needs to know HID spec

  Option B: User keymaps use KC_* names
    Pro: Readable, portable
    Con: Need translation layer
```

#### Section 8: Consistency & Harmony Rules

Checklist for keycode system:

- [ ] All keycodes follow same naming pattern
- [ ] Layer keys have consistent prefix
- [ ] Macro keys have consistent numbering
- [ ] Special keys clearly distinguished from HID keys
- [ ] Modifiers follow consistent naming (L/R prefix?)
- [ ] Navigation keys grouped logically
- [ ] Media keys follow same pattern as QMK or diverge intentionally
- [ ] Mouse keys (if any) follow clear naming
- [ ] All three HID interfaces covered in keycode set
- [ ] No naming collisions between ranges
- [ ] Reserved ranges clearly documented
- [ ] User-extensible ranges defined

#### Section 9: Concrete Examples

Work through real scenarios to test the design.

**Example 1: Volume Up Key**
```
Keymap entry:  KC_VOLU  (what user types)
Internal code: 0xE9     (internal representation)
USB interface: Interface 2 (Extended Controls)
HID page:      0x0C (Consumer)
HID usage:     0xE9 (Volume Up)
Report byte:   Send 0x00E9 in bytes 1-2 of Interface 2 report
```

**Example 2: Layer-Tap Key**
```
Keymap entry:  LT(1, KC_SPC)  (tap=space, hold=layer 1)
Internal code: 0x7E01 | (KC_SPC << 8)  (or however encoded)
Processing:    Key event handler checks hold duration
               <threshold: send KC_SPC to Interface 0
               >=threshold: activate layer 1, don't send key
```

**Example 3: Macro Key**
```
Keymap entry:  MACRO_VOL_UP  (custom encoder macro)
Internal code: 0x8000 | MACRO_ID_VOL_UP
Processing:    Macro engine executes sequence
               Sends KC_VOLU to Interface 2
```

#### Section 10: Migration & Translation Guide

Mechanical rules for harmonizing borrowed code.

**QMK → My Code:**
```
Find/Replace patterns:
  KC_LCTL → MY_LCTL (or keep KC_ prefix?)
  MO(0) → LAYER_MO(0) (or keep MO?)

Structural changes:
  Remove quantum/ dependencies
  Simplify layer state to my needs
  Extract only needed keycodes
```

**LUFA → My Code:**
```
Translation:
  HID_KEYBOARD_SC_A → KC_A
  Add layer/macro support (LUFA doesn't have)

Integration:
  Keep HID usage values
  Add my keycode wrapper
```

---

## Key Comparisons to Focus On

### When Analyzing MCC Output vs Other Frameworks

**Bloat Sources:**
- Unnecessary abstraction layers
- Configuration complexity (how many files to change one setting?)
- Generated code that's never used
- Wrapper functions with no added value

**Readability:**
- Can you follow the code flow?
- Are register operations clear?
- Is the USB state machine understandable?
- Are magic numbers explained?

**Portability vs Simplicity:**
- How tightly coupled to MCC framework?
- Can pieces be extracted easily?
- How much overhead does abstraction add?

**USB Specific:**
- Where are descriptors stored?
- How are endpoint buffers managed?
- What's the event handling model?
- How does enumeration work?

**Timing:**
- Delays, timeouts, polling intervals
- Matrix scan frequency
- Debouncing implementation
- USB timing requirements

**Memory:**
- Stack, heap, buffer allocation
- Where do descriptors live? (Flash vs RAM)
- State storage approach

---

## Questions to Answer Per Variation

Document these for each MCC variation:

1. **Where does MCC put USB descriptors?**
   - Flash or RAM?
   - Single file or multiple?
   - Generated or hand-coded structure?

2. **How does MCC handle endpoint buffers?**
   - Static allocation or dynamic?
   - Buffer sizes
   - Double buffering?

3. **What's the USB event handling model?**
   - Interrupt-driven or polled?
   - Callback functions?
   - State machine structure?

4. **How much overhead does the MCC abstraction add?**
   - Code size
   - Call depth
   - Understandability

5. **Can you easily extract just the register configurations?**
   - Are raw register writes visible?
   - Or buried in abstraction?

6. **How does MCC implement debouncing (if at all)?**
   - Time-based?
   - State machine?
   - None (left to user)?

7. **What's the quadrature decoder approach?**
   - State table?
   - Bitwise operations?
   - Interrupt or polling?

---

## ClaudeAI Draft - Special Notes

The ClaudeAI-generated draft serves as a unique reference point:

**Advantages:**
- Generated as cohesive whole - internal consistency by design
- Tailored to AmberClick90 specs specifically
- Represents one complete harmonious solution
- Shows how different pieces can work together

**How to Use:**
- Use as consistency baseline when mixing ideas
- Check if it validates approaches seen in other frameworks
- May serve as actual starting point for implementation
- Reference when evaluating "does this feel harmonious?"

**Document:**
- What constraints/assumptions was it working under?
- What did it do particularly well?
- What are known limitations?
- Which parts feel "right" vs need refinement?

---

## Practical Workflow

### Phase 1: MCC Exploration (Weeks 1-2)

1. Generate all MCC variations (1 through 9)
2. Build each, record metrics in Tab 2
3. For each variation, fill in "MCC Notes" column in Tab 1
4. Document file structure in Tab 3
5. Capture register settings in Tab 4

### Phase 2: Framework Comparison (Weeks 3-4)

1. Study DxCore examples for AVR-DU
2. Study LUFA HID examples
3. Study TinyUSB AVR examples
4. Study QMK keyboard implementation
5. Analyze ClaudeAI draft
6. Fill in respective columns in all tabs

### Phase 3: Decision Phase (Week 5)

1. Complete "My Decision" columns in Tab 1
2. Fill in Tab 6 (HAL Design Philosophy)
3. Fill in Tab 7 (KeyCode System Design)
4. Create consistency checklist
5. Document translation/harmonization rules

### Phase 4: Implementation (Weeks 6+)

1. Work through Tab 5 (Implementation Checklist)
2. Build feature by feature using "My Chosen Approach" column
3. Test each feature as it's completed
4. Update "Implementation Status" as you go
5. Refactor for harmony if needed

---

## Success Criteria

You'll know you've succeeded when:

- [ ] You can explain what every line of your code does
- [ ] The codebase feels harmonious and intentional, not patched together
- [ ] Naming is consistent across all subsystems
- [ ] You can easily modify keymaps without touching firmware core
- [ ] USB enumeration works reliably
- [ ] All three interfaces work in UEFI and OS
- [ ] Matrix scanning is solid with proper debouncing
- [ ] Encoder behavior is smooth and predictable
- [ ] Code is minimal - no bloat, only what's needed
- [ ] A new developer could read and understand the code
- [ ] You're proud of the implementation

---

## Notes

- **Power LED:** Hardwired to VBUS+GND (with current limiting resistor), not MCU-controlled
- **Indicator LEDs:** PA4, PA5 with PWM via TCA
- **Code Size:** If a feature needs to be there, it needs to be there - focus on understanding, not premature optimization
- **Spreadsheet Tool:** Use freeze panes to keep headers visible
- **Tab Growth:** Start with single comprehensive tabs; split only if they become unwieldy (>100 rows)

---

**End of Development Plan**
