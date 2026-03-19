# LUFA USB Protocol Reference for AmberClick90

This document extracts key USB HID and vendor protocol information from the LUFA library that will be useful for implementing the AmberClick90 firmware on the AVR64DU32.

---

## 1. Overview

LUFA (Lightweight USB Framework for AVRs) provides excellent reference implementations for USB device classes. While AmberClick90 uses a bare-metal approach rather than the full LUFA framework, the LUFA codebase provides valuable patterns for USB descriptors, HID report structures, and protocol handling.

---

## 2. HID Class Constants and Definitions

### 2.1 Boot Protocol Values

From `LUFA/Drivers/USB/Class/Common/HIDClassCommon.h`:

```c
// HID Interface Protocol values
enum HID_ClassSubclassProtocol_t
{
    HID_CSCP_NonBootProtocol      = 0x00,  // Not a boot protocol device
    HID_CSCP_KeyboardBootProtocol = 0x01,  // Boot keyboard protocol
    HID_CSCP_MouseBootProtocol    = 0x02   // Boot mouse protocol
};
```

**Usage for AmberClick90:**
- Interface 0 (Keyboard): `bInterfaceProtocol = 0x01`
- Interface 1 (Mouse): `bInterfaceProtocol = 0x02`
- Interface 2 (Extended Controls): `bInterfaceProtocol = 0x00`

### 2.2 HID Class Requests

```c
enum HID_ClassRequests_t
{
    HID_REQ_GetReport   = 0x01,  // Get current HID report from device
    HID_REQ_GetIdle     = 0x02,  // Get current device idle count
    HID_REQ_GetProtocol = 0x03,  // Get current HID report protocol mode
    HID_REQ_SetReport   = 0x09,  // Set current HID report to device
    HID_REQ_SetIdle     = 0x0A,  // Set device's idle count
    HID_REQ_SetProtocol = 0x0B   // Set current HID report protocol mode
};
```

**Critical for AmberClick90:**
- Must handle `SetReport` for keyboard LED updates
- Must handle `SetIdle` for report rate control
- Must handle `SetProtocol` for boot/report protocol switching

### 2.3 HID Descriptor Types

```c
enum HID_DescriptorTypes_t
{
    HID_DTYPE_HID    = 0x21,  // HID class HID descriptor
    HID_DTYPE_Report = 0x22   // HID class report descriptor
};
```

---

## 3. Keyboard HID Report Structure

### 3.1 Standard Boot Keyboard Report (8 bytes)

From LUFA Keyboard demos:

```c
typedef struct
{
    uint8_t Modifier;      // Modifier keys bitfield
    uint8_t Reserved;      // Reserved (always 0x00)
    uint8_t KeyCode[6];    // Array of 6 keycodes (6KRO)
} USB_KeyboardReport_Data_t;
```

**Modifier Byte Bits:**
```c
#define HID_KEYBOARD_MODIFIER_LEFTCTRL   (1 << 0)
#define HID_KEYBOARD_MODIFIER_LEFTSHIFT  (1 << 1)
#define HID_KEYBOARD_MODIFIER_LEFTALT    (1 << 2)
#define HID_KEYBOARD_MODIFIER_LEFTGUI    (1 << 3)
#define HID_KEYBOARD_MODIFIER_RIGHTCTRL  (1 << 4)
#define HID_KEYBOARD_MODIFIER_RIGHTSHIFT (1 << 5)
#define HID_KEYBOARD_MODIFIER_RIGHTALT   (1 << 6)
#define HID_KEYBOARD_MODIFIER_RIGHTGUI   (1 << 7)
```

**Key Code Array:**
- 0x00 = No key pressed
- 0x01 = ErrorRollOver (too many keys pressed)
- 0x04-0xA4 = HID usage codes (see HID Usage Tables)
- Maximum 6 simultaneous non-modifier keys (6KRO)

### 3.2 Keyboard LED Output Report (1 byte)

```c
typedef struct
{
    unsigned NumLock    : 1;
    unsigned CapsLock   : 1;
    unsigned ScrollLock : 1;
    unsigned Compose    : 1;
    unsigned Kana       : 1;
    unsigned Reserved   : 3;
} USB_KeyboardLEDReport_Data_t;
```

**LED Bit Definitions:**
```c
#define HID_KEYBOARD_LED_NUMLOCK    (1 << 0)
#define HID_KEYBOARD_LED_CAPSLOCK   (1 << 1)
#define HID_KEYBOARD_LED_SCROLLLOCK (1 << 2)
#define HID_KEYBOARD_LED_COMPOSE    (1 << 3)
#define HID_KEYBOARD_LED_KANA       (1 << 4)
```

---

## 4. Mouse HID Report Structure

### 4.1 Standard Boot Mouse Report (5 bytes)

```c
typedef struct
{
    uint8_t Button;    // Button bitfield
    int8_t  X;         // X movement delta
    int8_t  Y;         // Y movement delta
    int8_t  Wheel;     // Vertical wheel delta
    int8_t  HWheel;    // Horizontal wheel delta (optional)
} USB_MouseReport_Data_t;
```

**Button Byte Bits:**
```c
#define MOUSE_BUTTON_LEFT   (1 << 0)
#define MOUSE_BUTTON_RIGHT  (1 << 1)
#define MOUSE_BUTTON_MIDDLE (1 << 2)
```

**Movement Values:**
- Signed 8-bit integers (-127 to +127)
- Relative movement, not absolute position
- Wheel: positive = scroll up, negative = scroll down

---

## 5. HID Report Descriptors

### 5.1 Boot Keyboard Report Descriptor

From LUFA KeyboardMouse demo:

```c
const USB_Descriptor_HIDReport_Datatype_t PROGMEM KeyboardReport[] =
{
    HID_RI_USAGE_PAGE(8, 0x01),          // Generic Desktop
    HID_RI_USAGE(8, 0x06),               // Keyboard
    HID_RI_COLLECTION(8, 0x01),          // Application
    
        // Modifier byte
        HID_RI_USAGE_PAGE(8, 0x07),      // Keyboard/Keypad
        HID_RI_USAGE_MINIMUM(8, 0xE0),   // Left Control
        HID_RI_USAGE_MAXIMUM(8, 0xE7),   // Right GUI
        HID_RI_LOGICAL_MINIMUM(8, 0x00),
        HID_RI_LOGICAL_MAXIMUM(8, 0x01),
        HID_RI_REPORT_COUNT(8, 0x08),    // 8 bits
        HID_RI_REPORT_SIZE(8, 0x01),     // 1 bit each
        HID_RI_INPUT(8, HID_IOF_DATA | HID_IOF_VARIABLE | HID_IOF_ABSOLUTE),
        
        // Reserved byte
        HID_RI_REPORT_COUNT(8, 0x01),
        HID_RI_REPORT_SIZE(8, 0x08),
        HID_RI_INPUT(8, HID_IOF_CONSTANT),
        
        // LED output report (5 bits)
        HID_RI_USAGE_PAGE(8, 0x08),      // LEDs
        HID_RI_USAGE_MINIMUM(8, 0x01),   // Num Lock
        HID_RI_USAGE_MAXIMUM(8, 0x05),   // Kana
        HID_RI_REPORT_COUNT(8, 0x05),
        HID_RI_REPORT_SIZE(8, 0x01),
        HID_RI_OUTPUT(8, HID_IOF_DATA | HID_IOF_VARIABLE | HID_IOF_ABSOLUTE),
        
        // LED padding (3 bits)
        HID_RI_REPORT_COUNT(8, 0x01),
        HID_RI_REPORT_SIZE(8, 0x03),
        HID_RI_OUTPUT(8, HID_IOF_CONSTANT),
        
        // Key array (6 bytes)
        HID_RI_LOGICAL_MINIMUM(8, 0x00),
        HID_RI_LOGICAL_MAXIMUM(8, 0xA4),  // 164 (0xA4) = max HID keycode
        HID_RI_USAGE_PAGE(8, 0x07),       // Keyboard/Keypad
        HID_RI_USAGE_MINIMUM(8, 0x00),
        HID_RI_USAGE_MAXIMUM(8, 0xA4),
        HID_RI_REPORT_COUNT(8, 0x06),     // 6 keys
        HID_RI_REPORT_SIZE(8, 0x08),      // 8 bits each
        HID_RI_INPUT(8, HID_IOF_DATA | HID_IOF_ARRAY),
        
    HID_RI_END_COLLECTION(0),
};
```

### 5.2 Boot Mouse Report Descriptor

```c
const USB_Descriptor_HIDReport_Datatype_t PROGMEM MouseReport[] =
{
    HID_RI_USAGE_PAGE(8, 0x01),          // Generic Desktop
    HID_RI_USAGE(8, 0x02),               // Mouse
    HID_RI_COLLECTION(8, 0x01),          // Application
    
        HID_RI_USAGE(8, 0x01),           // Pointer
        HID_RI_COLLECTION(8, 0x00),      // Physical
        
            // Button bits (3 buttons)
            HID_RI_USAGE_PAGE(8, 0x09),  // Button
            HID_RI_USAGE_MINIMUM(8, 0x01),
            HID_RI_USAGE_MAXIMUM(8, 0x03),
            HID_RI_LOGICAL_MINIMUM(8, 0x00),
            HID_RI_LOGICAL_MAXIMUM(8, 0x01),
            HID_RI_REPORT_COUNT(8, 0x03),
            HID_RI_REPORT_SIZE(8, 0x01),
            HID_RI_INPUT(8, HID_IOF_DATA | HID_IOF_VARIABLE | HID_IOF_ABSOLUTE),
            
            // Padding (5 bits)
            HID_RI_REPORT_COUNT(8, 0x01),
            HID_RI_REPORT_SIZE(8, 0x05),
            HID_RI_INPUT(8, HID_IOF_CONSTANT),
            
            // X and Y movement
            HID_RI_USAGE_PAGE(8, 0x01),      // Generic Desktop
            HID_RI_USAGE(8, 0x30),           // X
            HID_RI_USAGE(8, 0x31),           // Y
            HID_RI_LOGICAL_MINIMUM(8, -127),
            HID_RI_LOGICAL_MAXIMUM(8, 127),
            HID_RI_REPORT_COUNT(8, 0x02),
            HID_RI_REPORT_SIZE(8, 0x08),
            HID_RI_INPUT(8, HID_IOF_DATA | HID_IOF_VARIABLE | HID_IOF_RELATIVE),
            
            // Vertical wheel
            HID_RI_USAGE(8, 0x38),           // Wheel
            HID_RI_LOGICAL_MINIMUM(8, -127),
            HID_RI_LOGICAL_MAXIMUM(8, 127),
            HID_RI_REPORT_COUNT(8, 0x01),
            HID_RI_REPORT_SIZE(8, 0x08),
            HID_RI_INPUT(8, HID_IOF_DATA | HID_IOF_VARIABLE | HID_IOF_RELATIVE),
            
        HID_RI_END_COLLECTION(0),
    HID_RI_END_COLLECTION(0),
};
```

### 5.3 Consumer Control Report Descriptor (for Interface 2)

Pattern from LUFA discussions:

```c
const USB_Descriptor_HIDReport_Datatype_t PROGMEM ConsumerReport[] =
{
    // Consumer Control Collection
    HID_RI_USAGE_PAGE(8, 0x0C),          // Consumer
    HID_RI_USAGE(8, 0x01),               // Consumer Control
    HID_RI_COLLECTION(8, 0x01),          // Application
    
        HID_RI_REPORT_ID(8, 0x03),       // Report ID 3
        
        // 16-bit consumer code
        HID_RI_USAGE_PAGE(8, 0x0C),
        HID_RI_USAGE_MINIMUM(8, 0x00),
        HID_RI_USAGE_MAXIMUM(16, 0x029C), // Max consumer code
        HID_RI_LOGICAL_MINIMUM(8, 0x00),
        HID_RI_LOGICAL_MAXIMUM(16, 0x029C),
        HID_RI_REPORT_COUNT(8, 0x01),
        HID_RI_REPORT_SIZE(8, 0x10),      // 16 bits
        HID_RI_INPUT(8, HID_IOF_DATA | HID_IOF_ARRAY | HID_IOF_ABSOLUTE),
        
    HID_RI_END_COLLECTION(0),
    
    // System Control Collection (for sleep)
    HID_RI_USAGE_PAGE(8, 0x01),          // Generic Desktop
    HID_RI_USAGE(8, 0x80),               // System Control
    HID_RI_COLLECTION(8, 0x01),          // Application
    
        HID_RI_REPORT_ID(8, 0x03),       // Same report ID
        
        HID_RI_USAGE(8, 0x82),           // System Sleep
        HID_RI_LOGICAL_MINIMUM(8, 0x00),
        HID_RI_LOGICAL_MAXIMUM(8, 0x01),
        HID_RI_REPORT_COUNT(8, 0x01),
        HID_RI_REPORT_SIZE(8, 0x01),      // 1 bit
        HID_RI_INPUT(8, HID_IOF_DATA | HID_IOF_VARIABLE | HID_IOF_ABSOLUTE),
        
        // Padding
        HID_RI_REPORT_COUNT(8, 0x01),
        HID_RI_REPORT_SIZE(8, 0x07),
        HID_RI_INPUT(8, HID_IOF_CONSTANT),
        
    HID_RI_END_COLLECTION(0),
};
```

---

## 6. USB Configuration Descriptor Structure

### 6.1 Configuration Descriptor Pattern

From LUFA demos:

```c
typedef struct
{
    USB_Descriptor_Configuration_Header_t Config;
    
    // Interface 0: Keyboard
    USB_Descriptor_Interface_t            HID1_KeyboardInterface;
    USB_HID_Descriptor_HID_t              HID1_KeyboardHID;
    USB_Descriptor_Endpoint_t             HID1_ReportINEndpoint;
    
    // Interface 1: Mouse
    USB_Descriptor_Interface_t            HID2_MouseInterface;
    USB_HID_Descriptor_HID_t              HID2_MouseHID;
    USB_Descriptor_Endpoint_t             HID2_ReportINEndpoint;
    
    // Interface 2: Consumer
    USB_Descriptor_Interface_t            HID3_ConsumerInterface;
    USB_HID_Descriptor_HID_t              HID3_ConsumerHID;
    USB_Descriptor_Endpoint_t             HID3_ReportINEndpoint;
} USB_Descriptor_Configuration_t;
```

### 6.2 Configuration Header

```c
.Config =
{
    .Header                 = {.Size = sizeof(USB_Descriptor_Configuration_Header_t), 
                               .Type = DTYPE_Configuration},
    .TotalConfigurationSize = sizeof(USB_Descriptor_Configuration_t),
    .TotalInterfaces        = 3,  // Three HID interfaces
    .ConfigurationNumber    = 1,
    .ConfigurationStrIndex  = NO_DESCRIPTOR,
    .ConfigAttributes       = (USB_CONFIG_ATTR_RESERVED | USB_CONFIG_ATTR_SELFPOWERED),
    .MaxPowerConsumption    = USB_CONFIG_POWER_MA(100)  // 100mA
},
```

### 6.3 Interface Descriptor (Keyboard Example)

```c
.HID1_KeyboardInterface =
{
    .Header                 = {.Size = sizeof(USB_Descriptor_Interface_t), 
                               .Type = DTYPE_Interface},
    .InterfaceNumber        = INTERFACE_ID_Keyboard,  // 0
    .AlternateSetting       = 0x00,
    .TotalEndpoints         = 1,
    .Class                  = HID_CSCP_HIDClass,      // 0x03
    .SubClass               = HID_CSCP_BootSubclass,  // 0x01 (boot interface)
    .Protocol               = HID_CSCP_KeyboardBootProtocol,  // 0x01
    .InterfaceStrIndex      = NO_DESCRIPTOR
},
```

### 6.4 HID Class Descriptor

```c
.HID1_KeyboardHID =
{
    .Header                 = {.Size = sizeof(USB_HID_Descriptor_HID_t), 
                               .Type = HID_DTYPE_HID},  // 0x21
    .HIDSpec                = VERSION_BCD(1,11),  // HID v1.11
    .CountryCode            = 0x00,  // Not localized
    .TotalReportDescriptors = 1,
    .HIDReportType          = HID_DTYPE_Report,  // 0x22
    .HIDReportLength        = sizeof(KeyboardReport)
},
```

### 6.5 Endpoint Descriptor

```c
.HID1_ReportINEndpoint =
{
    .Header                 = {.Size = sizeof(USB_Descriptor_Endpoint_t), 
                               .Type = DTYPE_Endpoint},
    .EndpointAddress        = KEYBOARD_IN_EPADDR,  // e.g., 0x81 (EP1 IN)
    .Attributes             = (EP_TYPE_INTERRUPT | ENDPOINT_ATTR_NO_SYNC | ENDPOINT_USAGE_DATA),
    .EndpointSize           = HID_EPSIZE,  // Typically 8 bytes
    .PollingIntervalMS      = 0x05  // Poll every 5ms
},
```

---

## 7. USB Device Descriptor

```c
const USB_Descriptor_Device_t PROGMEM DeviceDescriptor =
{
    .Header                 = {.Size = sizeof(USB_Descriptor_Device_t), 
                               .Type = DTYPE_Device},
    .USBSpecification       = VERSION_BCD(2,0,0),  // USB 2.0
    .Class                  = USB_CSCP_NoDeviceClass,  // 0x00 (interface-defined)
    .SubClass               = USB_CSCP_NoDeviceSubclass,
    .Protocol               = USB_CSCP_NoDeviceProtocol,
    .Endpoint0Size          = FIXED_CONTROL_ENDPOINT_SIZE,  // 8, 16, 32, or 64
    .VendorID               = 0x03EB,  // Atmel VID (change for production!)
    .ProductID              = 0x2042,  // Choose unique PID
    .ReleaseNumber          = VERSION_BCD(1,0,0),
    .ManufacturerStrIndex   = STRING_ID_Manufacturer,
    .ProductStrIndex        = STRING_ID_Product,
    .SerialNumStrIndex      = USE_INTERNAL_SERIAL,  // Or NO_DESCRIPTOR
    .NumberOfConfigurations = FIXED_NUM_CONFIGURATIONS
};
```

---

## 8. String Descriptors

### 8.1 Language Descriptor

```c
const USB_Descriptor_String_t PROGMEM LanguageString = 
    USB_STRING_DESCRIPTOR_ARRAY(LANGUAGE_ID_ENG);  // 0x0409 = English (US)
```

### 8.2 Product String Example

```c
const USB_Descriptor_String_t PROGMEM ProductString = 
    USB_STRING_DESCRIPTOR(L"AmberClick90");
```

### 8.3 Manufacturer String Example

```c
const USB_Descriptor_String_t PROGMEM ManufacturerString = 
    USB_STRING_DESCRIPTOR(L"YourName");
```

---

## 9. USB Class Request Handlers

### 9.1 HID Class Request Handler Pattern

From LUFA keyboard demos:

```c
void EVENT_USB_Device_ControlRequest(void)
{
    uint8_t* ReportData = NULL;
    uint8_t  ReportSize = 0;
    
    // Handle HID class requests
    switch (USB_ControlRequest.bRequest)
    {
        case HID_REQ_GetReport:
            if (USB_ControlRequest.bmRequestType == (REQDIR_DEVICETOHOST | REQTYPE_CLASS | REQREC_INTERFACE))
            {
                // Determine which interface
                uint8_t InterfaceNumber = (USB_ControlRequest.wIndex & 0xFF);
                
                // Get appropriate report
                if (InterfaceNumber == INTERFACE_ID_Keyboard)
                {
                    ReportData = (uint8_t*)&KeyboardReportData;
                    ReportSize = sizeof(KeyboardReportData);
                }
                else if (InterfaceNumber == INTERFACE_ID_Mouse)
                {
                    ReportData = (uint8_t*)&MouseReportData;
                    ReportSize = sizeof(MouseReportData);
                }
                
                Endpoint_ClearSETUP();
                Endpoint_Write_Control_Stream_LE(ReportData, ReportSize);
                Endpoint_ClearOUT();
            }
            break;
            
        case HID_REQ_SetReport:
            if (USB_ControlRequest.bmRequestType == (REQDIR_HOSTTODEVICE | REQTYPE_CLASS | REQREC_INTERFACE))
            {
                Endpoint_ClearSETUP();
                
                // Read LED report (keyboard only)
                uint8_t LEDReport;
                Endpoint_Read_Control_Stream_LE(&LEDReport, sizeof(LEDReport));
                Endpoint_ClearIN();
                
                // Process LED states
                ProcessKeyboardLEDs(LEDReport);
            }
            break;
            
        case HID_REQ_GetProtocol:
            if (USB_ControlRequest.bmRequestType == (REQDIR_DEVICETOHOST | REQTYPE_CLASS | REQREC_INTERFACE))
            {
                Endpoint_ClearSETUP();
                Endpoint_Write_8(UsingReportProtocol);
                Endpoint_ClearIN();
                Endpoint_ClearStatusStage();
            }
            break;
            
        case HID_REQ_SetProtocol:
            if (USB_ControlRequest.bmRequestType == (REQDIR_HOSTTODEVICE | REQTYPE_CLASS | REQREC_INTERFACE))
            {
                Endpoint_ClearSETUP();
                Endpoint_ClearStatusStage();
                
                UsingReportProtocol = (USB_ControlRequest.wValue != 0);
            }
            break;
            
        case HID_REQ_SetIdle:
            if (USB_ControlRequest.bmRequestType == (REQDIR_HOSTTODEVICE | REQTYPE_CLASS | REQREC_INTERFACE))
            {
                Endpoint_ClearSETUP();
                Endpoint_ClearStatusStage();
                
                IdleCount = ((USB_ControlRequest.wValue & 0xFF00) >> 8);
            }
            break;
            
        case HID_REQ_GetIdle:
            if (USB_ControlRequest.bmRequestType == (REQDIR_DEVICETOHOST | REQTYPE_CLASS | REQREC_INTERFACE))
            {
                Endpoint_ClearSETUP();
                Endpoint_Write_8(IdleCount);
                Endpoint_ClearIN();
                Endpoint_ClearStatusStage();
            }
            break;
    }
}
```

---

## 10. Vendor Class Bootloader Pattern

### 10.1 Bootloader Device Descriptor

For a vendor-class bootloader (non-DFU):

```c
const USB_Descriptor_Device_t PROGMEM DeviceDescriptor =
{
    .Header                 = {.Size = sizeof(USB_Descriptor_Device_t), 
                               .Type = DTYPE_Device},
    .USBSpecification       = VERSION_BCD(2,0,0),
    .Class                  = USB_CSCP_VendorSpecificClass,  // 0xFF
    .SubClass               = 0x00,  // Vendor-defined
    .Protocol               = 0x00,  // Vendor-defined
    .Endpoint0Size          = 64,
    .VendorID               = 0x03EB,  // Same as application
    .ProductID              = 0x2FFF,  // Different from application!
    .ReleaseNumber          = VERSION_BCD(1,0,0),
    .ManufacturerStrIndex   = STRING_ID_Manufacturer,
    .ProductStrIndex        = STRING_ID_Product,
    .SerialNumStrIndex      = NO_DESCRIPTOR,
    .NumberOfConfigurations = 1
};
```

### 10.2 Vendor Interface Descriptor

```c
.VendorInterface =
{
    .Header                 = {.Size = sizeof(USB_Descriptor_Interface_t), 
                               .Type = DTYPE_Interface},
    .InterfaceNumber        = 0,
    .AlternateSetting       = 0,
    .TotalEndpoints         = 2,  // IN and OUT
    .Class                  = USB_CSCP_VendorSpecificClass,
    .SubClass               = 0x00,
    .Protocol               = 0x00,
    .InterfaceStrIndex      = NO_DESCRIPTOR
},
```

### 10.3 Vendor Endpoint Descriptors

```c
.DataInEndpoint =
{
    .Header                 = {.Size = sizeof(USB_Descriptor_Endpoint_t), 
                               .Type = DTYPE_Endpoint},
    .EndpointAddress        = VENDOR_IN_EPADDR,   // e.g., 0x81
    .Attributes             = EP_TYPE_BULK,
    .EndpointSize           = 64,
    .PollingIntervalMS      = 0x00
},

.DataOutEndpoint =
{
    .Header                 = {.Size = sizeof(USB_Descriptor_Endpoint_t), 
                               .Type = DTYPE_Endpoint},
    .EndpointAddress        = VENDOR_OUT_EPADDR,  // e.g., 0x01
    .Attributes             = EP_TYPE_BULK,
    .EndpointSize           = 64,
    .PollingIntervalMS      = 0x00
},
```

---

## 11. Key Implementation Notes for Citrouille90

### 11.1 Descriptor Callback Function

Must implement this function to handle descriptor requests:

```c
uint16_t CALLBACK_USB_GetDescriptor(const uint16_t wValue,
                                    const uint16_t wIndex,
                                    const void** const DescriptorAddress)
{
    const uint8_t  DescriptorType   = (wValue >> 8);
    const uint8_t  DescriptorNumber = (wValue & 0xFF);
    
    const void* Address = NULL;
    uint16_t    Size    = NO_DESCRIPTOR;
    
    switch (DescriptorType)
    {
        case DTYPE_Device:
            Address = &DeviceDescriptor;
            Size    = sizeof(USB_Descriptor_Device_t);
            break;
            
        case DTYPE_Configuration:
            Address = &ConfigurationDescriptor;
            Size    = sizeof(USB_Descriptor_Configuration_t);
            break;
            
        case DTYPE_String:
            switch (DescriptorNumber)
            {
                case STRING_ID_Language:
                    Address = &LanguageString;
                    Size    = pgm_read_byte(&LanguageString.Header.Size);
                    break;
                case STRING_ID_Manufacturer:
                    Address = &ManufacturerString;
                    Size    = pgm_read_byte(&ManufacturerString.Header.Size);
                    break;
                case STRING_ID_Product:
                    Address = &ProductString;
                    Size    = pgm_read_byte(&ProductString.Header.Size);
                    break;
            }
            break;
            
        case HID_DTYPE_HID:
            // Return appropriate HID descriptor based on wIndex (interface number)
            switch (wIndex)
            {
                case 0:  // Keyboard
                    Address = &ConfigurationDescriptor.HID1_KeyboardHID;
                    Size    = sizeof(USB_HID_Descriptor_HID_t);
                    break;
                case 1:  // Mouse
                    Address = &ConfigurationDescriptor.HID2_MouseHID;
                    Size    = sizeof(USB_HID_Descriptor_HID_t);
                    break;
                case 2:  // Consumer
                    Address = &ConfigurationDescriptor.HID3_ConsumerHID;
                    Size    = sizeof(USB_HID_Descriptor_HID_t);
                    break;
            }
            break;
            
        case HID_DTYPE_Report:
            // Return appropriate report descriptor based on wIndex
            switch (wIndex)
            {
                case 0:  // Keyboard
                    Address = &KeyboardReport;
                    Size    = sizeof(KeyboardReport);
                    break;
                case 1:  // Mouse
                    Address = &MouseReport;
                    Size    = sizeof(MouseReport);
                    break;
                case 2:  // Consumer
                    Address = &ConsumerReport;
                    Size    = sizeof(ConsumerReport);
                    break;
            }
            break;
    }
    
    *DescriptorAddress = Address;
    return Size;
}
```

### 11.2 Critical Timing Considerations

From LUFA best practices:

- **Endpoint polling intervals:**
  - Keyboard: 5-10ms (0x05-0x0A in descriptor)
  - Mouse: 5ms or less for responsiveness
  - Consumer: 10ms acceptable
  
- **Idle rate handling:**
  - Default idle rate: infinite (0 = only send on change)
  - BIOS may set finite idle rate (4ms typical)
  - Must respect SetIdle requests
  
- **Boot protocol:**
  - UEFI/BIOS will request boot protocol (SetProtocol 0)
  - Must work correctly in both boot and report protocol modes

### 11.3 Report Sending Pattern

```c
void SendNextReport(void)
{
    static USB_KeyboardReport_Data_t PrevKeyboardReport;
    USB_KeyboardReport_Data_t        KeyboardReport;
    bool                             SendReport = false;
    
    // Create keyboard report from matrix
    CreateKeyboardReport(&KeyboardReport);
    
    // Send if changed OR if idle timeout expired
    if ((memcmp(&PrevKeyboardReport, &KeyboardReport, sizeof(USB_KeyboardReport_Data_t)) != 0) ||
        IdleTimeoutExpired)
    {
        SendReport = true;
    }
    
    if (SendReport)
    {
        // Select endpoint
        Endpoint_SelectEndpoint(KEYBOARD_IN_EPADDR);
        
        // Check if endpoint is ready
        if (Endpoint_IsINReady())
        {
            // Write report data
            Endpoint_Write_Stream_LE(&KeyboardReport, sizeof(KeyboardReport), NULL);
            
            // Finalize and send
            Endpoint_ClearIN();
            
            // Save for comparison
            PrevKeyboardReport = KeyboardReport;
        }
    }
}
```

### 11.4 Consumer Code Examples

Common consumer codes for Interface 2:

```c
#define CONSUMER_VOLUME_INCREMENT   0x00E9
#define CONSUMER_VOLUME_DECREMENT   0x00EA
#define CONSUMER_MUTE               0x00E2
#define CONSUMER_PLAY_PAUSE         0x00CD
#define CONSUMER_SCAN_NEXT_TRACK    0x00B5
#define CONSUMER_SCAN_PREV_TRACK    0x00B6
#define CONSUMER_STOP               0x00B7
#define CONSUMER_BROWSER_HOME       0x0223
#define CONSUMER_BROWSER_BACK       0x0224
#define CONSUMER_BROWSER_FORWARD    0x0225
#define CONSUMER_BROWSER_REFRESH    0x0227
#define CONSUMER_BROWSER_SEARCH     0x0221
#define CONSUMER_BROWSER_FAVORITES  0x022A
#define CONSUMER_EMAIL              0x018A
#define CONSUMER_CALCULATOR         0x0192
#define CONSUMER_MY_COMPUTER        0x0194
#define CONSUMER_MEDIA_SELECT       0x0183
```

---

## 12. AVR-DU Specific Adaptations

While LUFA was written for older AVR USB parts, the concepts transfer to AVR-DU with these adaptations:

### 12.1 USB Peripheral Differences

**AVR-DU Changes from LUFA targets:**
- Different register names (consult AVR64DU32 datasheet)
- Different USB peripheral architecture
- Native USB Full-Speed (similar to AT90USB series)
- Integrated USB PAD with internal resistors

### 12.2 Endpoint Configuration

AVR-DU endpoint configuration will differ from LUFA's older AVR USB parts. Key concepts remain:

- Endpoint 0: Control (bidirectional, 8/16/32/64 bytes)
- Endpoints 1-3: IN endpoints for HID reports
- Each endpoint needs buffer allocation in SRAM
- Endpoint sizes must match descriptor declarations

### 12.3 Clock Configuration

AVR-DU uses different clock setup:
- 24 MHz main clock (vs LUFA's varied clock configs)
- USB Full-Speed requires proper PLL configuration
- Consult AVR64DU32 datasheet Section 11 (CLKCTRL) and Section 32 (USB)

---

## 13. Testing Checklist

When implementing USB stack for AmberClick90:

**Basic Enumeration:**
- [ ] Device descriptor recognized
- [ ] Configuration descriptor accepted
- [ ] String descriptors readable
- [ ] All three interfaces enumerate

**Keyboard Interface (Interface 0):**
- [ ] Boot protocol works in UEFI/BIOS
- [ ] Report protocol works in OS
- [ ] SetProtocol switching works
- [ ] Modifier keys work correctly
- [ ] 6KRO detected correctly
- [ ] LED output reports received and processed
- [ ] SetIdle handled correctly

**Mouse Interface (Interface 1):**
- [ ] Boot protocol works in UEFI/BIOS
- [ ] Movement deltas work correctly
- [ ] Button presses detected
- [ ] Scroll wheel functional
- [ ] Encoder controls mouse in UEFI

**Consumer Interface (Interface 2):**
- [ ] Media keys functional in OS
- [ ] Volume control works
- [ ] Browser keys functional
- [ ] System sleep works
- [ ] Not enumerated in UEFI (expected)

**General:**
- [ ] No USB enumeration errors
- [ ] Device doesn't disconnect unexpectedly
- [ ] Timing requirements met
- [ ] Report rate acceptable (<10ms latency)

---

## 14. Reference Documents

**LUFA Sources to Study:**
- `LUFA/Drivers/USB/Class/Common/HIDClassCommon.h` - HID constants
- `Demos/Device/LowLevel/KeyboardMouse/` - Full keyboard+mouse example
- `Demos/Device/ClassDriver/KeyboardMouseMultiReport/` - Multi-interface pattern
- `LUFA/Drivers/USB/Core/` - USB core concepts (adapt for AVR-DU)

**External References:**
- USB HID v1.11 Specification
- USB HID Usage Tables v1.3
- AVR64DU32 Datasheet (Section 32: USB)
- USB 2.0 Specification (Chapter 9: Device Framework)

---

## 15. Summary for AmberClick90

**What to borrow from LUFA:**
- HID report descriptor structures (copy patterns directly)
- USB descriptor organization (device, config, interface, endpoint, HID, string)
- Class request handler patterns (GetReport, SetReport, SetProtocol, SetIdle)
- Report data structures (keyboard, mouse, consumer)
- Timing and protocol behaviors

**What NOT to use directly:**
- LUFA's hardware abstraction layer (AVR-DU is different)
- LUFA's event system (build your own for bare-metal)
- LUFA's endpoint management macros (AVR-DU has different registers)
- LUFA's build system (you're using avr-gcc directly)

**Implementation strategy:**
1. Start with descriptors (copy LUFA patterns, adapt for three interfaces)
2. Implement USB enumeration (AVR-DU specific registers)
3. Add control endpoint handler (handle standard and HID class requests)
4. Implement interrupt endpoint sending (for reports)
5. Test enumeration before adding application logic
6. Add keyboard matrix scanning
7. Add report generation
8. Test thoroughly in both UEFI and OS

This approach gives you the proven USB protocol patterns from LUFA while letting you build a lean, AVR-DU-optimized bare-metal implementation.

---

**End of LUFA USB Protocol Reference**</parameter>