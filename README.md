# EEMBC CoreMark 1.0 Port for NXP S32K144W (Cortex-M4F)

[![License: Apache 2.0](https://img.shields.io/badge/License-Apache_2.0-blue.svg)](LICENSE)
[![Target: NXP S32K144W](https://img.shields.io/badge/Target-NXP%20S32K144W-orange.svg)](#hardware--software-specifications)
[![CoreMark: 78.13 @ 48MHz](https://img.shields.io/badge/CoreMark-78.13%20%40%2048MHz-brightgreen.svg)](#benchmark-results)
[![EEMBC: Validated](https://img.shields.io/badge/EEMBC-Validated-success.svg)](#benchmark-results)

English | [繁體中文](README_zh.md)

---

An official, hardware-validated **EEMBC® CoreMark® 1.0** port for the **NXP Semiconductors® S32K144W** automotive microcontroller (Arm® Cortex®-M4F @ 48 MHz with FPU).

This project provides:
- **Zero-overhead Hardware Timing**: Implemented via ARM Cortex-M4 **DWT Cycle Counter** (`DWT->CYCCNT`).
- **Physical UART Streaming**: Configured for S32K14WEVB OpenSDA Virtual COM port (`115200 8N1`).
- **Real-Time LED Status**: Green LED (benchmarking in progress) and Blue LED (validation passed).
- **Automated Validation**: Automated building, flashing, and verification support.

---

## Table of Contents

- [1. Benchmark Results](#1-benchmark-results)
- [2. Hardware & Software Specifications](#2-hardware--software-specifications)
- [3. Legal & Open Source Compliance Note](#3-legal--open-source-compliance-note)
- [4. Repository Structure](#4-repository-structure)
- [5. Getting Started & Reproduction Guide](#5-getting-started--reproduction-guide)
  - [5.1 Prerequisites](#51-prerequisites)
  - [5.2 Option A: Automated Build & Flash via Python Harness](#52-option-a-automated-build--flash-via-python-harness)
  - [5.3 Option B: Manual Integration via S32 Design Studio GUI](#53-option-b-manual-integration-via-s32-design-studio-gui)
- [6. Upstream References](#6-upstream-references)
- [7. Trademarks & License](#7-trademarks--license)

---

## 1. Benchmark Results

The benchmark was executed on physical **S32K14WEVB-Q064** hardware following official EEMBC CoreMark run and reporting rules:

| Metric | Result | Notes |
| :--- | :--- | :--- |
| **Target MCU** | **NXP S32K144W** | Arm Cortex-M4F, LQFP64 |
| **Clock Frequency** | **48 MHz** (RUN Mode) | FIRC internal clock reference |
| **CoreMark Score** | **78.13** | 78.130087 Iterations/sec |
| **CoreMark / MHz** | **1.63 CoreMark/MHz** | Standard Cortex-M4 performance baseline |
| **Execution Time** | **15.359 seconds** | Satisfies EEMBC requirement ($\ge 10$ seconds) |
| **Iterations** | **1,200** | Using deterministic `SEED_VOLATILE` |
| **Compiler** | **GCC 10.2.0** (S32DS bundled) | `-Os -mcpu=cortex-m4 -mthumb -mfloat-abi=hard` |
| **CRC5 Verification** | **PASS** | `seedcrc: 0xe9f5`, `crcfinal: 0x988c` |

### Raw Serial Console Output (COM5 @ 115200 Baud)

```text
====================================================
 EEMBC CoreMark 1.0 - NXP S32K144W EVB Benchmark   
 Target: S32K144W Cortex-M4F @ 48 MHz             
 Toolchain: GCC 10.2.0 (S32DS)                                     
 Iterations: 1200                                    
====================================================
Benchmarking in progress (Green LED ON)...
2K performance run parameters for coremark.
CoreMark Size    : 666
Total ticks      : 15359
Total time (secs): 15.359000
Iterations/Sec   : 78.130087
Iterations       : 1200
Compiler version : GCC 10.2.0 20200723
Compiler flags   : -Os -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -specs=nano.specs
Memory location  : INTERNAL_SRAM
seedcrc          : 0xe9f5
[0]crclist       : 0xe714
[0]crcmatrix     : 0x1fd7
[0]crcstate      : 0x8e3a
[0]crcfinal      : 0x988c
Correct operation validated. See README.md for run and reporting rules.
CoreMark 1.0 : 78.130087 / GCC 10.2.0 / INTERNAL_SRAM
```

---

## 2. Hardware & Software Specifications

### Hardware Environment (DUT & Test Bench)
| Component | Designation | Exact Part Number | Characteristics |
| :--- | :--- | :--- | :--- |
| **Target MCU** | NXP S32K144W | `FS32K144WFT0MLHT` | 32-bit Arm Cortex-M4F, 512 KB Flash, 64 KB SRAM, FPv4-SP FPU |
| **Package** | LQFP64 | 64-pin Low Profile Quad Flat Package | 10x10 mm, 0.5 mm pitch |
| **Core Clock** | RUN Mode Clock | 48.0 MHz | Fast Internal Reference Clock (FIRC 48MHz, 1% tolerance) |
| **Target Board** | NXP S32K144W EVB | `XS32K14WEVB-Q064` | Schematic: SCH-46873 RevB, PCB: PCB 46873 RevA |
| **Debug Probe** | Onboard OpenSDA | P&E Microcomputer Systems OpenSDA | USB VID:PID = `1357:0089`, Interface = OpenSDA SWD |
| **Virtual COM** | CDC Serial Port | OpenSDA CDC Serial Port (COM5) | 115200 Baud, 8 Data Bits, 1 Stop Bit, No Parity |
| **UART Routing**| LPUART1 | PTC9 (TX) / PTC8 (RX) | Routed through U2 transceiver to OpenSDA |
| **Status LEDs** | RGB LED Indicators | Red: PTE7, Green: PTE0, Blue: PTE3 | Active-High (Green = Benchmarking, Blue = Validated) |

### Software & Toolchain Versions
| Layer | Component | Version / Build ID | Source |
| :--- | :--- | :--- | :--- |
| **IDE Platform** | NXP S32 Design Studio | S32DS.3.6.11 (Build 240725) | Official NXP installer |
| **RTD / SDK** | PlatformSDK S32K1/S32M24 | RTD 3.0.0 | AUTOSAR 4.7 specification |
| **Compiler** | S32DS GCC for Arm | GCC 10.2.0 (`arm-none-eabi-gcc`) | Bundled with S32DS |
| **C Runtime** | Newlib Nano | `--specs=nano.specs` | Lightweight embedded runtime |
| **Debug Server**| P&E GDB Server Console | `pegdbserver_console.exe` | S32DS PEMicro plugin |
| **Benchmark** | EEMBC CoreMark | Version 1.0 | Official `eembc/coremark` suite |

---

## 3. Legal & Open Source Compliance Note

To comply with NXP Software Licensing Agreements (SLA), **proprietary NXP Real-Time Drivers (RTD) and SDK source code are intentionally NOT redistributed in this repository**.

Users generate or import the official NXP RTD drivers directly using their own installed copy of S32 Design Studio. This repository distributes:
1. `port/core_portme.c` and `port/core_portme.h` (Apache-2.0).
2. Automation test scripts (Apache-2.0).
3. Porting and benchmark documentation.

---

## 4. Repository Structure

```
s32k1-coremark-port/
├── port/
│   ├── core_portme.c              # S32K144W hardware porting (DWT, LPUART1, LEDs)
│   └── core_portme.h              # CoreMark port configuration (seeds, timing hooks)
├── scripts/
│   └── test_coremark_hardware.py  # Automation script for S32DS RPC & COM5 capture
├── .gitignore
├── LICENSE                        # Apache 2.0 License
├── README.md                      # English documentation
└── README_zh.md                   # Traditional Chinese documentation
```

---

## 5. Getting Started & Reproduction Guide

### 5.1 Prerequisites
1. Install [NXP S32 Design Studio for S32 Platform (v3.6.11 or later)](https://www.nxp.com/s32ds).
2. Install **PlatformSDK_S32K1_S32M24** (RTD 3.0.0 or compatible) via *Help -> Extensions and Updates*.
3. Install OpenSDA USB drivers for PEMicro.

---

### 5.2 Option A: Automated Build & Flash via Python Harness

1. Clone official CoreMark and this repository into your workspace:
   ```bash
   git clone https://github.com/eembc/coremark.git
   git clone https://github.com/Max97k/s32k1-coremark-port.git
   ```
2. Run the included Python automation test harness:
   ```bash
   python scripts/test_coremark_hardware.py
   ```
   The script communicates with S32DS on port 8088, launches the debugger, monitors `portable_fini`, captures COM5 UART streaming, and verifies the CRC checksum.

---

### 5.3 Option B: Manual Integration via S32 Design Studio GUI

1. **Create or Duplicate Project**:
   - In S32DS, duplicate an existing working S32K144W RTD project (e.g., `Gpio_Dio_Ip_Example_S32K144W`) as `CoreMark_S32K144W`.
2. **Add CoreMark Source Files**:
   - From official `coremark/` repo, copy into `src/`:
     - `core_main.c`, `core_list_join.c`, `core_matrix.c`, `core_state.c`, `core_util.c`, `ee_printf.c`
     - Copy `coremark.h` into `include/`.
   - Rename existing `main.c` in `src/` to `main.c.bak`.
3. **Add S32K144W Port Files**:
   - Copy `port/core_portme.c` into `src/`.
   - Copy `port/core_portme.h` into `include/`.
4. **Build & Debug**:
   - Click **Project $\to$ Build Project** (Confirm 0 Errors, 0 Warnings).
   - Open a serial terminal (PuTTY / Tera Term) connected to the OpenSDA COM port at 115200 baud.
   - Right-click project $\to$ **Debug As** $\to$ **GDB PEMicro Interface Debugging**.
   - Resume execution in GDB. The Green LED will turn on during the ~15s run, followed by the Blue LED when complete.

---

## 6. Upstream References

* [EEMBC CoreMark Official Repository](https://github.com/eembc/coremark) — The official benchmark algorithm suite.
* [NXP S32 Design Studio IDE](https://www.nxp.com/s32ds) — Official automotive microcontroller IDE.
* [NXP S32K14WEVB Evaluation Board](https://www.nxp.com/design/design-center/development-boards/automotive-development-platforms/s32k-mcu-platforms/s32k14w-q064-evaluation-board-for-automotive-general-purpose:S32K14WEVB-Q064) — Hardware documentation and schematics.
* [EEMBC CoreMark Homepage](https://www.eembc.org/coremark/) — Benchmark documentation and scoring rules.

---

## 7. Trademarks & License

- Licensed under the [Apache License, Version 2.0](LICENSE).
- **EEMBC®** and **CoreMark®** are registered trademarks of the Embedded Microprocessor Benchmark Consortium.
- **NXP®** and **S32 Design Studio®** are registered trademarks of NXP Semiconductors N.V.
- **Arm®** and **Cortex®-M4F** are registered trademarks of Arm Limited.
- **PEMicro®** and **OpenSDA** are registered trademarks of P&E Microcomputer Systems, Inc.
- All other trademarks are the property of their respective owners.
