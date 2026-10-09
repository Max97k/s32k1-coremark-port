# EEMBC CoreMark 1.0 Port for NXP S32K144W (Cortex-M4F)

[![License: Apache 2.0](https://img.shields.io/badge/License-Apache_2.0-blue.svg)](LICENSE)
[![Target: NXP S32K144W](https://img.shields.io/badge/Target-NXP%20S32K144W-orange.svg)](#2-hardware--software-specifications)
[![CoreMark: 78.13 @ 48MHz](https://img.shields.io/badge/CoreMark-78.13%20%40%2048MHz-brightgreen.svg)](#1-benchmark-results)
[![EEMBC: Validated](https://img.shields.io/badge/EEMBC-Validated-success.svg)](#1-benchmark-results)
[![Toolchain: GCC 10.2](https://img.shields.io/badge/Toolchain-GCC%2010.2%20(0%20errors)-brightgreen.svg)](https://gcc.gnu.org)

English | [繁體中文](README_zh.md)

---

An official, hardware-validated **EEMBC® CoreMark® 1.0** port and ready-to-import **S32 Design Studio** project for the **NXP Semiconductors® S32K144W** automotive microcontroller (Arm® Cortex®-M4F @ 48 MHz with FPU).

This project provides:
- **Direct Workspace Import**: Complete Eclipse/S32DS project structure (`.project`, `.cproject`, `.mex`) ready to build out-of-the-box.
- **Zero-Overhead Hardware Timing**: Implemented via Arm Cortex-M4 **DWT Cycle Counter** (`DWT->CYCCNT`).
- **Physical UART Streaming**: Configured for S32K14WEVB OpenSDA Virtual COM port (`115200 8N1`).
- **Real-Time LED Status**: Green LED (benchmarking in progress) and Blue LED (validation passed).
- **Dual Build Workflows**: Supports both S32DS GUI build and headless command-line (CLI) build and flash.

---

## Table of Contents

- [1. Benchmark Results](#1-benchmark-results)
- [2. Hardware & Software Specifications](#2-hardware--software-specifications)
- [3. Repository Structure](#3-repository-structure)
- [4. Getting Started & Build Workflows](#4-getting-started--build-workflows)
  - [4.1 Prerequisites](#41-prerequisites)
  - [4.2 Option A: Build and Debug via S32 Design Studio (IDE)](#42-option-a-build-and-debug-via-s32-design-studio-ide)
  - [4.3 Option B: Headless Build and Flash via Command Line (CLI)](#43-option-b-headless-build-and-flash-via-command-line-cli)
- [5. Upstream References](#5-upstream-references)
- [6. Trademarks & License](#6-trademarks--license)

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

## 3. Repository Structure

```
s32k1-coremark-port/
├── board/                         # Pin and clock initialization configs (MEX generated)
│   ├── Port_Ci_Port_Ip_Cfg.c
│   └── Port_Ci_Port_Ip_Cfg.h
├── include/                       # CoreMark & Porting header files
│   ├── check_example.h
│   ├── coremark.h                 # EEMBC CoreMark 1.0 algorithm definitions
│   └── core_portme.h              # S32K144W port definitions & timing hooks
├── Project_Settings/
│   ├── Debugger/                  # PEMicro GDB launch configurations
│   ├── Linker_Files/              # S32K144W Flash & RAM linker scripts (.ld)
│   └── Startup_Code/              # CMSIS startup, vector table, NVIC configuration
├── src/                           # Source files
│   ├── core_list_join.c           # CoreMark list processing benchmark
│   ├── core_main.c                # CoreMark benchmark entrypoint
│   ├── core_matrix.c              # CoreMark matrix manipulation benchmark
│   ├── core_portme.c              # S32K144W hardware porting (DWT, LPUART1, LEDs)
│   ├── core_state.c               # CoreMark state machine benchmark
│   ├── core_util.c                # CoreMark CRC calculation utilities
│   └── ee_printf.c                # Embedded lightweight formatting print
├── tools/
│   └── flash_target.py            # Automated OpenSDA PEMicro flash & reset tool
├── Gpio_Dio_Ip_Example.mex        # S32 Configuration Tools project file
├── .cproject                      # Eclipse CDT build configuration
├── .project                       # Eclipse project descriptor
├── .gitignore
├── LICENSE                        # Apache 2.0 License
├── README.md                      # English documentation
└── README_zh.md                   # Traditional Chinese documentation
```

---

## 4. Getting Started & Build Workflows

### 4.1 Prerequisites
1. Install [NXP S32 Design Studio for S32 Platform (v3.6.11 or later)](https://www.nxp.com/s32ds).
2. Install **PlatformSDK_S32K1_S32M24** (RTD 3.0.0 or compatible) via *Help -> Extensions and Updates*.
3. Install OpenSDA USB drivers for PEMicro.

---

### 4.2 Option A: Build and Debug via S32 Design Studio (IDE)

1. Launch S32 Design Studio.
2. Select **File $\to$ Import... $\to$ General $\to$ Existing Projects into Workspace**.
3. Browse to this cloned repository root and click **Finish**.
4. In the Project Explorer, right-click the project and select **S32 Configuration Tool $\to$ Update Code** to generate local RTD driver bindings.
5. Click **Project $\to$ Build Project** (or press `Ctrl+B`). Verify build output reports `0 errors, 0 warnings`.
6. Connect the board via USB (`J7`). Open your serial terminal (PuTTY / Tera Term) connected to the OpenSDA COM port at 115200 baud.
7. Open **Run $\to$ Debug Configurations...**, select `CoreMark_S32K144W_Debug_FLASH_PNE`, and click **Debug**.
8. Resume execution in GDB. The Green LED will illuminate during the ~15s run, followed by the Blue LED when complete. The terminal will display the full benchmark report.

---

### 4.3 Option B: Headless Build and Flash via Command Line (CLI)

The project can be built and flashed headlessly without launching the Eclipse GUI:

```powershell
# 1. Add S32DS build tools and compiler to PATH
$env:PATH = "C:\NXP\S32DS.3.6.11\S32DS\build_tools\msys32\usr\bin;C:\NXP\S32DS.3.6.11\S32DS\build_tools\gcc_v10.2\gcc-10.2-arm32-eabi\bin;" + $env:PATH

# 2. Invoke GNU Make in the build directory
cd Debug_FLASH
make -j8 all

# 3. Flash to target board using the headless PEMicro script
python ../tools/flash_target.py
```

Expected build size output:
```
   text    data     bss     dec     hex filename
  43868     272   10520   54660    d584 CoreMark_S32K144W.elf
```

---

## 5. Upstream References

* [EEMBC CoreMark Official Repository](https://github.com/eembc/coremark) — The official benchmark algorithm suite.
* [NXP S32 Design Studio IDE](https://www.nxp.com/s32ds) — Official automotive microcontroller IDE.
* [NXP S32K14WEVB Evaluation Board](https://www.nxp.com/design/design-center/development-boards/automotive-development-platforms/s32k-mcu-platforms/s32k14w-q064-evaluation-board-for-automotive-general-purpose:S32K14WEVB-Q064) — Hardware documentation and schematics.
* [EEMBC CoreMark Homepage](https://www.eembc.org/coremark/) — Benchmark documentation and scoring rules.

---

## 6. Trademarks & License

- Licensed under the [Apache License, Version 2.0](LICENSE).
- **EEMBC®** and **CoreMark®** are registered trademarks of the Embedded Microprocessor Benchmark Consortium.
- **NXP®** and **S32 Design Studio®** are registered trademarks of NXP Semiconductors N.V.
- **Arm®** and **Cortex®-M4F** are registered trademarks of Arm Limited.
- **PEMicro®** and **OpenSDA** are registered trademarks of P&E Microcomputer Systems, Inc.
- All other trademarks are the property of their respective owners.
