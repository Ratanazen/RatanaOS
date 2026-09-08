# RatanaOS 64-bit (macOS Desktop Edition)

![Year](https://img.shields.io/badge/Edition-macOS_Desktop_Sequoia-blueviolet)
![Architecture](https://img.shields.io/badge/Architecture-x86__64_%7C_AMD64-blue)
![Graphics](https://img.shields.io/badge/GUI-VBE_32--bit_Framebuffer-brightgreen)
![Language](https://img.shields.io/badge/Language-C23_%2F_NASM_64-orange)
![Build](https://img.shields.io/badge/Build-Passing-brightgreen)
![License](https://img.shields.io/badge/License-MIT-green)

**RatanaOS 64-bit (macOS Edition)** is a native **64-bit x86_64 Long Mode operating system** featuring an authentic **macOS Sequoia-style graphical desktop environment**, top Menu Bar with  Apple menu, bottom floating translucent Dock with colorful rich icons, traffic light window controls (🔴 🟡 🟢), and a complete **About This Mac** system report.

---

## macOS GUI Desktop Architecture

```
+-----------------------------------------------------------------------------------------+
| []  Finder  File  Edit  View  Go  Window  Help             [WiFi] [RAM: 24KB] [10:35 PM] | <- Top Menu Bar
+-----------------------------------------------------------------------------------------+
|                                                                                         |
|  [📁 RatanaOS HD]                                                                       |
|                                                                                         |
|      +-------------------------------------------------------------------+              |
|      | (•)(•)(•)                   About This Mac                        |              |
|      | +---------------------------------------------------------------+ |              |
|      | |       RatanaOS Sequoia (2026 64-bit Edition)                 | |              |
|      | |        Version 15.4 (Build 2026.09)                           | |              |
|      | |        Model:   MacBook Pro (x86_64 Long Mode)                | |              |
|      | |        Chip:    x86_64 Processor                              | |              |
|      | |        Memory:  256 MB Unified (16 MB Dynamic Heap)           | |              |
|      | |        Graphics: VBE 32-bit Linear Framebuffer (1024x768)     | |              |
|      | +---------------------------------------------------------------+ |              |
|      +-------------------------------------------------------------------+              |
|                                                                                         |
|                                                                                         |
|             +-------------------------------------------------------------+             |
|             |  [Finder] [Terminal] [SysMon] [Calc] [Paint] [About] [Trash]| <- macOS Dock
|             +-------------------------------------------------------------+             |
+-----------------------------------------------------------------------------------------+
```

---

## Key macOS Subsystems & Features

### 1. Top Menu Bar (24px)
- ** Apple Menu**: Dropdown with *About This Mac*, *System Settings*, *Force Quit*, and *Exit to CLI*.
- **Active App Title & Standard Menus**: `File`, `Edit`, `View`, `Window`, `Help`.
- **Menu Extras (Status Icons)**:
  - 📶 Wi-Fi status symbol.
  - 🔋 Battery indicator.
  - 🧠 Live RAM & Heap monitor (`KB`).
  - 🕒 CMOS Real-Time Clock (`MM/DD HH:MM`).
  - 🔍 Spotlight Search & Control Center emblems.

### 2. Floating Bottom Dock (Translucent Glass)
- Rounded glass pill container with rich 32x32 vector/bitmap application icons:
  1. **Finder**: 2-tone blue/cyan smiling face icon.
  2. **Terminal.app**: Dark icon with `>_` green prompt.
  3. **Activity Monitor.app**: Activity pulse wave & CPU monitor.
  4. **Calculator.app**: Orange grid icon with math operators (`+`, `-`, `*`, `=`).
  5. **Paint Studio.app**: Artist palette with color swatches and brush.
  6. **About This Mac**:  Apple emblem icon.
  7. **Trash**: Metal mesh wastebasket icon (clicking clears drawing canvas).
- **Active App Indicators**: Glowing dot below open windows.

### 3. macOS Window Styling & Traffic Lights
- 🔴 **Red Button** (`0x00FF5F56`): Close window.
- 🟡 **Yellow Button** (`0x00FFBD2E`): Minimize to Dock.
- 🟢 **Green Button** (`0x0027C93F`): Zoom / Expand window.
- Centered typography and dark-mode window shadows.

### 4. "About This Mac" Experience
- Authentic Apple-style system specifications card:
  - Big  Logo emblem.
  - Model: `MacBook Pro (x86_64 Long Mode)`.
  - Chip: Processor brand extracted via 64-bit CPUID.
  - Memory: `256 MB Unified Memory (16 MB Dynamic Heap)`.
  - Startup Disk: `RatanaOS HD (1GB)`.
  - Graphics: `VBE 32-bit Linear Framebuffer 1024x768`.
  - Serial: `C02RTN2026X86`.

---

## Quickstart & Testing

### 1. Build Kernel
```bash
make clean && make
```

### 2. Run Automated Test Suite
```bash
make test
```

### 3. Launch in QEMU
```bash
make run
```
*Tip: In the shell prompt, type `gui` to launch the macOS desktop. Use your mouse to interact with windows, click the Apple menu, launch Dock apps, and press `ESC` or choose `Exit to CLI` to return to the shell.*
