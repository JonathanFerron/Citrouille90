# AmberClick90 Keyboard Firmware

Custom minimal firmware for the AmberClick90 90-key keyboard with AVR64DU32 MCU.

## Features

- **90-key matrix** (10 rows × 9 columns) with diodes
- **3 layers** with momentary layer switching
- **Boot keyboard protocol** (6-key rollover)
- **Media keys** (volume, play/pause, next/prev)
- **Macros** (Ctrl+C, Ctrl+V, etc.) with press-hold-release behavior
- **2 PWM-controlled LEDs** with layer indication
- **Rotary encoder** with Alt-Tab window switching
- **Debouncing** for reliable key detection

## Project Structure

```
amberclick90/
├── config.h          # Hardware and firmware configuration
├── keycode.h         # Keycode definitions and type checking
├── layer.c/h         # Layer management (only one active at a time)
├── keymap.c/h        # Keymap definitions with transparent key support
├── matrix.c/h        # Matrix scanning and debouncing
├── macro.c/h         # Macro execution (press/release)
├── led.c/h           # PWM LED control with layer indication
├── encoder.c/h       # Rotary encoder with Alt-Tab functionality
├── keyboard.c/h      # Main keyboard logic and key processing
├── usb_hid.h         # USB HID interface (needs implementation)
├── main.c            # Entry point and system initialization
├── Makefile          # Build configuration
└── README.md         # This file
```

## Keycode System

### Prefixes
- `KC_` - Standard HID keyboard codes (0x00-0xFF)
- `CC_` - Consumer control codes (0x0100-0x01FF)
- `LY_` - Layer switching codes (0x0200-0x02FF)
- `MC_` - Macro codes (0x0300-0x03FF)
- `LD_` - LED control codes (0x0400-0x04FF)
- `SYS_` - System/firmware codes (0x0500-0x05FF)

### Special Keycodes
- `KC_TRNS` (or `______`) - Transparent, falls through to lower layer
- `KC_NO` (or `XXXXXXX`) - No key, explicitly disabled
- `LY_MO1` - Hold for Layer 1
- `LY_MO2` - Hold for Layer 2

## Layer Behavior

- **Layer 0**: Base layer, both LEDs at base brightness (default: 64/255)
- **Layer 1**: Function layer, LED1 brighter (+64), LED2 at base
- **Layer 2**: System layer, both LEDs brighter (+64)

Only one layer is active at a time. If multiple layer keys are held, the highest layer takes priority.

## LED Configuration

- Base brightness: 64 (adjustable with LD_BRIU/LD_BRID on Layer 2)
- Brightness step: 16
- Layer delta: 64
- Both LEDs use the same PWM timer (TCA0 in split mode)

## Rotary Encoder

- **Clockwise**: Alt+Tab (switches to next window)
- **Counter-clockwise**: Alt+Shift+Tab (switches to previous window)
- **Timeout**: 1000ms after last rotation, Alt is released
- The encoder maintains Alt held state across multiple turns

## USB Implementation

The firmware structure is complete **except for the USB HID stack**. You need to implement `usb_hid.c` using one of these libraries:

### Option 1: LUFA (Lightweight USB Framework for AVRs)
- Most popular for AVR USB keyboards
- Well-documented with many examples
- Supports AVR64DU32
- https://github.com/abcminiuser/lufa

### Option 2: Microchip ASF (Advanced Software Framework)
- Official Microchip library
- Good support for AVR64DU32
- More complex but feature-rich

### Option 3: TinyUSB
- Modern, lightweight USB stack
- Growing AVR support
- Clean API

### USB Descriptor Requirements

Your USB descriptors should include:

1. **Device Descriptor**
   - Vendor ID: 0xFEED (change to your own)
   - Product ID: 0x0090 (change to your own)
   - Device class: 0 (defined at interface level)

2. **Configuration Descriptor**
   - Two interfaces: Keyboard + Consumer Control

3. **HID Report Descriptor**
   - Boot keyboard (6KRO): Standard boot protocol
   - Consumer control: For media keys

### Key Functions to Implement

```c
void usb_init(void);              // Initialize USB peripheral
void usb_task(void);              // USB event handling
bool usb_configured(void);        // Check if enumerated
void send_keyboard_report(void);  // Send keyboard HID report
void send_consumer_report(uint16_t keycode);  // Send media key
```

## Building

### Prerequisites
- AVR-GCC toolchain
- avr-libc
- AVRDUDE for programming
- USB library (LUFA/ASF/TinyUSB)

### Build Commands
```bash
make            # Build firmware
make size       # Show firmware size
make clean      # Clean build files
make flash      # Program device
```

### Configuration
Edit `Makefile` to match your programmer:
```makefile
AVRDUDE_PROGRAMMER = serialupdi
AVRDUDE_PORT = /dev/ttyUSB0  # or COM3 on Windows
```

## Pin Configuration

Edit `config.h` to match your PCB layout:

- **Rows**: PORTA0-7, PORTB0-1 (10 pins)
- **Columns**: PORTB2-5, PORTF0-4 (9 pins)
- **LEDs**: PORTC0-1 (2 pins)
- **Encoder**: PORTD0-1 (2 pins)

All pin assignments can be changed to match your hardware.

## Customization

### Editing the Keymap

Edit `keymap.c` to customize your layout:

```c
const uint16_t PROGMEM keymaps[NUM_LAYERS][MATRIX_ROWS][MATRIX_COLS] = {
    [0] = { /* Base layer */ },
    [1] = { /* Function layer */ },
    [2] = { /* System layer */ },
};
```

Use `______` for transparent keys and `XXXXXXX` for disabled keys.

### Adding Macros

1. Define keycode in `keycode.h`:
   ```c
   #define MC_MYNEW  (MC_BASE | 0x08)
   ```

2. Add implementation in `macro.c`:
   ```c
   case MC_MYNEW:
       keyboard_report.mods |= MOD_LCTL | MOD_LSFT;
       keyboard_report.keys[0] = KC_N;
       send_keyboard_report();
       break;
   ```

3. Add to keymap in `keymap.c`

### Adjusting Timing

In `config.h`:
- `DEBOUNCE_MS` - Key debounce time (default: 5ms)
- `ENCODER_ALT_TIMEOUT_MS` - Encoder Alt release timeout (default: 1000ms)
- `LED_BRIGHTNESS_*` - LED brightness settings

## Next Steps

1. **Implement USB stack** - Choose and integrate LUFA/ASF/TinyUSB
2. **Test matrix** - Verify all 90 keys scan correctly
3. **Calibrate debouncing** - Adjust if needed for your switches
4. **Test encoder** - Verify quadrature decoding works
5. **Tune LED brightness** - Adjust values to your preference
6. **Add bootloader** - Implement `SYS_BOOT` key functionality

## License

This firmware structure is provided as-is. Customize freely for your keyboard project.

## Hardware Requirements

- AVR64DU32 microcontroller
- 90 × mechanical switches
- 90 × 1N4148 diodes (one per key)
- 1 × rotary encoder (quadrature)
- 2 × LEDs with current-limiting resistors
- USB-C connector
- Support components (crystal, caps, etc.)

## Resources

- [QMK Firmware](https://qmk.fm/) - Reference for features
- [AVR64DU32 Datasheet](https://www.microchip.com/en-us/product/AVR64DU32)
- [LUFA Documentation](http://www.fourwalledcubicle.com/LUFA.php)
- [USB HID Usage Tables](https://usb.org/sites/default/files/hut1_3_0.pdf)