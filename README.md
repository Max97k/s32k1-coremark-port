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

## Hardware Setup

- **Board**: NXP S32K14WEVB-Q064 (PCB 46873 RevB)
- **Debugger**: Onboard OpenSDA (P&E Micro PEMicro USB)
- **Serial Port**: OpenSDA CDC Virtual COM (`115200 baud, 8 data bits, 1 stop bit, no parity`)
  - **TX Pin**: `PTC9` (routed to OpenSDA transceiver via R680)
  - **RX Pin**: `PTC8` (routed to OpenSDA transceiver via R679)
- **Status LEDs**:
  - Green LED: `PTE0` (Benchmarking Active)
  - Blue LED: `PTE3` (Validation Passed)

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

## License

The files in this repository are licensed under the [Apache License, Version 2.0](LICENSE).  
CoreMark is a registered trademark of the Embedded Microprocessor Benchmark Consortium (EEMBC).
