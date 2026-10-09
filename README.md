# EEMBC CoreMark 1.0 Port for NXP S32K144W (Cortex-M4F)

[![License](https://img.shields.io/badge/License-Apache_2.0-blue.svg)](LICENSE)
[![Target](https://img.shields.io/badge/Target-NXP%20S32K144W-orange.svg)](#hardware-setup)
[![CoreMark](https://img.shields.io/badge/CoreMark-78.13%20%40%2048MHz-brightgreen.svg)](#benchmark-results)
[![EEMBC](https://img.shields.io/badge/EEMBC-Validated-success.svg)](#eembc-compliance)

This repository provides an official, hardware-validated **EEMBC CoreMark 1.0** port for the **NXP S32K144W** microcontroller (ARM Cortex-M4F with FPU). It includes:

- **Zero-overhead Hardware Timing**: Implemented via ARM Cortex-M4 **DWT Cycle Counter** (`DWT->CYCCNT`).
- **Physical UART Streaming**: Configured for S32K14WEVB OpenSDA Virtual COM port (`115200 8N1`).
- **Real-Time LED Status**: Green LED (benchmarking in progress) and Blue LED (validation passed).
- **AI-Ready Automation**: Supports automated building, flashing, and verification via the **S32 Design Studio AI Support / MCP Integration Plugin**.

---

## Benchmark Results

The benchmark was executed on physical **S32K14WEVB-Q064** hardware following official EEMBC CoreMark run and reporting rules:

| Metric | Result | Notes |
| :--- | :--- | :--- |
| **Target MCU** | **NXP S32K144W** | ARM Cortex-M4F, LQFP64 |
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

## Hardware & Software Specifications (Full Environment BOM)

### 1. Hardware Specifications (DUT & Test Bench)
| Component | Item Name / Designation | Exact Part Number / Version | Details & Characteristics |
| :--- | :--- | :--- | :--- |
| **Target MCU** | NXP S32K144W | FS32K144WFT0MLHT (NXP_S32K1xx_S32K144WF512M8) | 32-bit ARM Cortex-M4F, 512 KB Flash, 64 KB SRAM, FPv4-SP-D16 FPU |
| **Package** | LQFP64 | 64-pin Low Profile Quad Flat Package (10x10 mm, 0.5 mm pitch) | Pinout revision matches S32K144W_LQFP64 |
| **Core Clock** | RUN Mode Clock | 48.0 MHz | Driven by Fast Internal Reference Clock (FIRC 48MHz, 1% tolerance) |
| **Target Board** | NXP S32K144W EVB | XS32K14WEVB-Q064 | Schematic: SCH-46873 RevB, PCB: PCB 46873 RevA |
| **Debug Probe** | Onboard OpenSDA | P&E Microcomputer Systems OpenSDA | USB VID:PID = 1357:0089, Serial = SDA6C761E5B, Port = USB1 |
| **Virtual COM** | CDC Serial Port | OpenSDA - CDC Serial Port (COM5) | 115200 Baud, 8 Data Bits, 1 Stop Bit, No Parity |
| **UART Routing** | LPUART1 | PTC9 (TX) / PTC8 (RX) | Level-shifted through U2 to OpenSDA controller (J5 headers isolated) |
| **Status LEDs** | RGB LED Indicators | Red: PTE7, Green: PTE0, Blue: PTE3 | Active High (Green = Benchmarking, Blue = Validation Passed) |

### 2. Software, IDE & Toolchain Versions
| Layer | Software / Component | Exact Version / Build ID | Distribution / Source |
| :--- | :--- | :--- | :--- |
| **IDE Platform** | NXP S32 Design Studio | S32DS.3.6.11 (Build ID: 3.6.11.202609221336) | Product: com.nxp.s32ds.platform.ide.product |
| **AI MCP Plugin** | S32DS MCP Server Plugin | 1.0.3.202609171039 | com.nxp.s32ds.mcp.server (JSON-RPC server on TCP 8088) |
| **Agentic AI Suite** | S32DS Agentic AI Toolkit | 1.0.0 (s32ds-agentic-ai-installer-1.0.0.pyz) | FastMCP 3.3.1 + Python 3.11+ MCP Gateway |
| **RTD / SDK** | PlatformSDK S32K1/S32M24 | RTD 3.0.0 | Build: S32K1_RTD_3_0_0_D2503_ASR_REL_4_7_REV_0000_20250307 |
| **AUTOSAR Spec** | AUTOSAR Architecture | AUTOSAR 4.7.0 (ASR_REL_4_7_REV_0000) | Standard automotive abstraction headers |
| **Compiler** | S32DS GCC for ARM | GCC 10.2.0 (Build rev g5963bc8, BLD = 1728) | Binary: rm-none-eabi-gcc.exe |
| **C Runtime Library**| Newlib Nano | 
ewlib-nano (--specs=nano.specs) | Embedded lightweight stdio/stdlib implementation |
| **GDB Server** | P&E GDB Server Console | pegdbserver_console.exe Version 10.02.00.00 | Plugin: com.pemicro.debug.gdbjtag.pne_6.1.8.202603121731 |
| **P&E Core Library** | PEMicro Unit NGS DLL | unit_ngs_arm_internal.dll Version 1.3.5 | com.pemicro.core_1.3.5 |
| **Benchmark Suite**| EEMBC CoreMark | Version 1.0 | Official GitHub eembc/coremark (Apache-2.0) |

---

## Legal & Open Source Compliance Note

To comply with NXP Software Licensing Agreements (SLA), **proprietary NXP Real-Time Drivers (RTD) and SDK source code are intentionally NOT redistributed in this repository**.

Users generate or import the official NXP RTD drivers directly using their own installed copy of S32 Design Studio. This repository only distributes:
1. `port/core_portme.c` and `port/core_portme.h` (Apache-2.0).
2. Automation test scripts (Apache-2.0).
3. Benchmark documentation.

---

## Step-by-Step Reproduction Guide

### Prerequisites
1. Install [S32 Design Studio for S32 Platform (v3.6.11 or later)](https://www.nxp.com/s32ds).
2. Install **PlatformSDK_S32K1_S32M24** (RTD 3.0.0 or compatible) via *Help -> Extensions and Updates*.
3. (Optional but recommended) Enable the **S32 Design Studio MCP Integration Plugin** (listening on TCP port 8088).

---

### Option A: Automated Build & Flash via S32DS AI Support Plugin (Recommended)

If you have an AI agent (Claude Desktop, Cursor, VS Code, Antigravity) connected to the **S32DS MCP Server**:

1. Clone official CoreMark and this repository into your workspace:
   ```bash
   git clone https://github.com/eembc/coremark.git
   git clone https://github.com/Max97k/s32k1-coremark-port.git
   ```
2. Prompt your AI Agent:
   > *"Duplicate the working S32K144W RTD project into `CoreMark_S32K144W`, copy the CoreMark algorithm files and the `s32k1-coremark-port/port` files into it, build the project with S32DS, flash it to S32K144W EVB, and verify the CoreMark output on COM5."*
3. Or run the included Python test harness directly:
   ```bash
   python scripts/test_coremark_hardware.py
   ```
   The script communicates with S32DS on port 8088, launches the debugger, monitors `portable_fini`, captures COM5 UART streaming, and asserts CRC validation.

---

### Option B: Manual Integration in S32 Design Studio GUI

1. **Create/Duplicate Project**:
   - In S32DS, choose an existing working S32K144W RTD example (e.g. `Gpio_Dio_Ip_Example_S32K144W`) or create a new project with RTD Clock, Port, and Dio drivers.
   - Right-click the project -> **Copy**, then **Paste** as `CoreMark_S32K144W`.
2. **Add CoreMark Source Files**:
   - From official `coremark/` repo, copy into `src/`:
     - `core_main.c`
     - `core_list_join.c`
     - `core_matrix.c`
     - `core_state.c`
     - `core_util.c`
     - `ee_printf.c`
     - `coremark.h` (into `include/` or `src/`)
   - Rename existing `main.c` in `src/` to `main.c.bak` (CoreMark provides its own `main()`).
3. **Add S32K144W Port Files**:
   - Copy `port/core_portme.c` into `src/`.
   - Copy `port/core_portme.h` into `include/`.
4. **Build & Debug**:
   - Project -> **Build Project** (Ensure 0 Errors, 0 Warnings).
   - Open your favorite terminal (PuTTY / Tera Term) connected to the OpenSDA COM port at 115200 baud.
   - Right-click project -> **Debug As** -> **GDB PEMicro Interface Debugging**.
   - Resume execution in GDB. The Green LED will turn on during the ~15s run, followed by the Blue LED when complete. The terminal will display the full benchmark report.

---

## Directory Structure

```
s32k1-coremark-port/
├── port/
│   ├── core_portme.c           # S32K144W hardware porting (DWT, LPUART1, LEDs)
│   └── core_portme.h           # CoreMark configuration parameters (seeds, timing)
├── scripts/
│   └── test_coremark_hardware.py # Python automation script for S32DS RPC & COM5 capture
├── .gitignore
├── LICENSE                     # Apache 2.0 License
└── README.md                   # This documentation
```

---


---

## References & Upstream Projects

This project builds upon and integrates with the following official upstream open-source repositories:

| Resource / Project | Link / Badge | Description |
| :--- | :--- | :--- |
| **EEMBC CoreMark** | [![GitHub](https://img.shields.io/badge/GitHub-eembc%2Fcoremark-181717?logo=github)](https://github.com/eembc/coremark) | The official EEMBC CoreMark benchmark algorithm suite. |
| **NXP S32DS AI Support** | [![GitHub](https://img.shields.io/badge/GitHub-nxp--agentic--ai%2Fs32--ds--ai--support-181717?logo=github)](https://github.com/nxp-agentic-ai/s32-ds-ai-support) | NXP's official Model Context Protocol (MCP) server & agentic AI suite for S32 Design Studio. |
| **NXP S32 Design Studio (IDE)** | [S32 Design Studio for S32 Platform](https://www.nxp.com/design/design-center/software/automotive-software-and-tools/s32-design-studio-ide:S32-DESIGN-STUDIO-IDE) | Official IDE and tooling suite for NXP automotive and industrial microcontrollers. |
| **NXP S32K14WEVB Evaluation Board** | [S32K14WEVB-Q064 Product Page](https://www.nxp.com/design/design-center/development-boards/automotive-development-platforms/s32k-mcu-platforms/s32k14w-q064-evaluation-board-for-automotive-general-purpose:S32K14WEVB-Q064) | Official hardware documentation, schematics, and user guides for S32K14WEVB-Q064. |
| **EEMBC Official Website** | [https://www.eembc.org/coremark/](https://www.eembc.org/coremark/) | CoreMark benchmark documentation, scoring guidelines, and certification rules. |
| **NXP Community** | [Agentic AI Development Community](https://community.nxp.com/t5/Agentic-AI-Development/tkb-p/agentic-ai-development) | Official NXP discussion forum for AI-assisted embedded workflows. |

## License

The files in this repository are licensed under the [Apache License, Version 2.0](LICENSE).  
CoreMark is a registered trademark of the Embedded Microprocessor Benchmark Consortium (EEMBC).
