# EEMBC CoreMark 1.0 移植版（適用於 NXP S32K144W Cortex-M4F）

[![License: Apache 2.0](https://img.shields.io/badge/License-Apache_2.0-blue.svg)](LICENSE)
[![Target: NXP S32K144W](https://img.shields.io/badge/Target-NXP%20S32K144W-orange.svg)](#2-硬體與軟體環境清單)
[![CoreMark: 78.13 @ 48MHz](https://img.shields.io/badge/CoreMark-78.13%20%40%2048MHz-brightgreen.svg)](#1-基準測試實測成績)
[![EEMBC: Validated](https://img.shields.io/badge/EEMBC-Validated-success.svg)](#1-基準測試實測成績)

[English](README.md) | 繁體中文

---

本專案提供針對 **恩智浦半導體 (NXP Semiconductors®) S32K144W** 車規微控制器（Arm® Cortex®-M4F @ 48 MHz 具備硬體 FPU）官方且通過實體硬體驗證的 **EEMBC® CoreMark® 1.0** 基準測試移植。

本專案特色：
- **零額外負擔硬體精確計時**：透過 ARM Cortex-M4 內部 **DWT 週期計數器** (`DWT->CYCCNT`) 實作。
- **實體 UART 串列資料流**：配置於 S32K14WEVB OpenSDA 虛擬串列埠 (`115200 8N1`)。
- **即時 LED 狀態指示**：綠燈（測試執行中）與藍燈（驗證通過）。
- **自動化測試支援**：支援一鍵編譯、燒錄與硬體自動化斷言驗證。

---

## 目錄

- [1. 基準測試實測成績](#1-基準測試實測成績)
- [2. 硬體與軟體環境清單](#2-硬體與軟體環境清單)
- [3. 法規與開源遵循說明](#3-法規與開源遵循說明)
- [4. 專案目錄結構](#4-專案目錄結構)
- [5. 快速上手與複現指南](#5-快速上手與複現指南)
  - [5.1 環境需求](#51-環境需求)
  - [5.2 方法 A：透過 Python 自動化腳本建置與燒錄](#52-方法-a透過-python-自動化腳本建置與燒錄)
  - [5.3 方法 B：透過 S32 Design Studio GUI 手動整合](#53-方法-b透過-s32-design-studio-gui-手動整合)
- [6. 上游專案與參考連結](#6-上游專案與參考連結)
- [7. 商標與版權許可](#7-商標與版權許可)

---

## 1. 基準測試實測成績

本基準測試嚴格遵循 EEMBC CoreMark 官方運行與回報規範於實體 **S32K14WEVB-Q064** 硬體上執行：

| 評測指標 | 測試結果 | 備註說明 |
| :--- | :--- | :--- |
| **目標 MCU** | **NXP S32K144W** | Arm Cortex-M4F, LQFP64 |
| **時脈頻率** | **48 MHz** (RUN 模式) | FIRC 內部參考時脈源 |
| **CoreMark 分數** | **78.13** | 78.130087 Iterations/sec |
| **CoreMark / MHz** | **1.63 CoreMark/MHz** | 符合 Cortex-M4 標準效能基準線 |
| **執行時間** | **15.359 秒** | 符合 EEMBC 規範要求（$\ge 10$ 秒） |
| **疊代次數** | **1,200 次** | 使用確定性 `SEED_VOLATILE` |
| **編譯器** | **GCC 10.2.0** (S32DS 隨附) | `-Os -mcpu=cortex-m4 -mthumb -mfloat-abi=hard` |
| **CRC5 驗證** | **通過 (PASS)** | `seedcrc: 0xe9f5`, `crcfinal: 0x988c` |

### 實體串列埠輸出記錄 (COM5 @ 115200 Baud)

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

## 2. 硬體與軟體環境清單

### 硬體測試環境 (DUT & Test Bench)
| 元件分類 | 項目名稱 | 確切料號 / 規格 | 規格特性 |
| :--- | :--- | :--- | :--- |
| **目標 MCU** | NXP S32K144W | `FS32K144WFT0MLHT` | 32-bit Arm Cortex-M4F, 512 KB Flash, 64 KB SRAM, FPv4-SP FPU |
| **封裝形式** | LQFP64 | 64-pin Low Profile Quad Flat Package | 10x10 mm, 0.5 mm 腳距 |
| **核心時脈** | RUN 模式時脈 | 48.0 MHz | 由快速內部參考時脈 (FIRC 48MHz, 1% 容差) 驅動 |
| **評估板** | NXP S32K144W EVB | `XS32K14WEVB-Q064` | 電路圖: SCH-46873 RevB, PCB: PCB 46873 RevA |
| **除錯器** | 板載 OpenSDA | P&E Microcomputer Systems OpenSDA | USB VID:PID = `1357:0089`, 介面 = OpenSDA SWD |
| **虛擬串列埠**| CDC 串列埠 | OpenSDA CDC Serial Port (COM5) | 115200 Baud, 8 Data Bits, 1 Stop Bit, No Parity |
| **UART 線路** | LPUART1 | PTC9 (TX) / PTC8 (RX) | 經由 U2 電平轉換連接至 OpenSDA 控制器 |
| **狀態指示燈**| 板載 RGB LED | 紅: PTE7, 綠: PTE0, 藍: PTE3 | 正邏輯 High-True（綠燈 = 測試中，藍燈 = 驗證通過） |

### 軟體與工具鏈版本
| 層級分類 | 軟體元件名稱 | 版本 / 組建代號 | 來源管道 |
| :--- | :--- | :--- | :--- |
| **IDE 平台** | NXP S32 Design Studio | S32DS.3.6.11 (Build 240725) | 官方 NXP 安裝包 |
| **RTD / SDK**| PlatformSDK S32K1/S32M24 | RTD 3.0.0 | AUTOSAR 4.7 規範相容驅動 |
| **編譯器** | S32DS GCC for Arm | GCC 10.2.0 (`arm-none-eabi-gcc`) | S32DS 隨附工具鏈 |
| **C 函式庫** | Newlib Nano | `--specs=nano.specs` | 輕量化嵌入式 C 執行庫 |
| **除錯伺服器**| P&E GDB Server Console | `pegdbserver_console.exe` | S32DS PEMicro 外掛套件 |
| **基準套件** | EEMBC CoreMark | Version 1.0 | 官方 `eembc/coremark` 開源庫 |

---

## 3. 法規與開源遵循說明

為嚴格遵循 NXP 軟體授權協議 (Software License Agreement, SLA)，**本儲存庫中未包含任何專有的 NXP Real-Time Drivers (RTD) 或 SDK 原始碼**。

使用者應透過本機安裝之 S32 Design Studio 官方環境直接生成或匯入驅動程式碼。本儲存庫僅發布：
1. `port/core_portme.c` 與 `port/core_portme.h`（Apache-2.0 授權）。
2. 自動化測試腳本（Apache-2.0 授權）。
3. 移植手冊與基準測試文件。

---

## 4. 專案目錄結構

```
s32k1-coremark-port/
├── port/
│   ├── core_portme.c              # S32K144W 硬體底層移植 (DWT, LPUART1, LED)
│   └── core_portme.h              # CoreMark 移植參數配置 (種子碼, 計時掛鉤)
├── scripts/
│   └── test_coremark_hardware.py  # S32DS RPC 控制與 COM5 串列埠自動化測試腳本
├── .gitignore
├── LICENSE                        # Apache 2.0 授權條款
├── README.md                      # 英文說明文件
└── README_zh.md                   # 繁體中文說明文件
```

---

## 5. 快速上手與複現指南

### 5.1 環境需求
1. 安裝 [NXP S32 Design Studio for S32 Platform (v3.6.11 或更新版本)](https://www.nxp.com/s32ds)。
2. 透過 *Help -> Extensions and Updates* 安裝 **PlatformSDK_S32K1_S32M24**（RTD 3.0.0 或相容版本）。
3. 安裝 PEMicro OpenSDA USB 驅動程式。

---

### 5.2 方法 A：透過 Python 自動化腳本建置與燒錄

1. 將官方 CoreMark 演算法倉庫與本專案複製至工作空間：
   ```bash
   git clone https://github.com/eembc/coremark.git
   git clone https://github.com/Max97k/s32k1-coremark-port.git
   ```
2. 直接執行內附之 Python 硬體自動化驗證腳本：
   ```bash
   python scripts/test_coremark_hardware.py
   ```
   腳本將連線至本機除錯環境，啟動除錯器，監控 `portable_fini` 斷點，接收 COM5 串列埠輸出並斷言驗證 CRC5 校驗碼。

---

### 5.3 方法 B：透過 S32 Design Studio GUI 手動整合

1. **建立或複製專案**：
   - 在 S32DS 中複製一個現有的 S32K144W RTD 專案（如 `Gpio_Dio_Ip_Example_S32K144W`）並命名為 `CoreMark_S32K144W`。
2. **加入 CoreMark 核心原始檔**：
   - 從官方 `coremark/` 複製以下檔案至專案 `src/`：
     - `core_main.c`、`core_list_join.c`、`core_matrix.c`、`core_state.c`、`core_util.c`、`ee_printf.c`
     - 複製 `coremark.h` 至 `include/`。
   - 將 `src/` 內原有的 `main.c` 更名為 `main.c.bak`。
3. **加入 S32K144W 移植檔**：
   - 將 `port/core_portme.c` 複製至 `src/`。
   - 將 `port/core_portme.h` 複製至 `include/`。
4. **編譯與除錯**：
   - 點擊 **Project $\to$ Build Project**（確認 0 Errors, 0 Warnings）。
   - 開啟串列終端機軟體（如 PuTTY / Tera Term）連線至 OpenSDA COM 埠（115200 波特率）。
   - 在專案上按右鍵 $\to$ **Debug As** $\to$ **GDB PEMicro Interface Debugging**。
   - 在 GDB 中點擊 Resume 執行，板上綠燈將亮起約 15 秒，測試完成後藍燈亮起，串列埠將輸出完整報告。

---

## 6. 上游專案與參考連結

* [EEMBC CoreMark 官方儲存庫](https://github.com/eembc/coremark) — 官方基準測試演算法套件。
* [NXP S32 Design Studio 官方網頁](https://www.nxp.com/s32ds) — 恩智浦車用微控制器官方 IDE。
* [NXP S32K14WEVB 評估板產品頁面](https://www.nxp.com/design/design-center/development-boards/automotive-development-platforms/s32k-mcu-platforms/s32k14w-q064-evaluation-board-for-automotive-general-purpose:S32K14WEVB-Q064) — 官方硬體規格書與電路圖。
* [EEMBC CoreMark 官方網站](https://www.eembc.org/coremark/) — 評分規範與官方登錄手冊。

---

## 7. 商標與版權許可

- 本專案採用 [Apache License, Version 2.0](LICENSE) 條款授權。
- **EEMBC®** 與 **CoreMark®** 為 Embedded Microprocessor Benchmark Consortium 之註冊商標。
- **NXP®** 與 **S32 Design Studio®** 為 NXP Semiconductors N.V. 之註冊商標。
- **Arm®** 與 **Cortex®-M4F** 為 Arm Limited 之註冊商標。
- **PEMicro®** 與 **OpenSDA** 為 P&E Microcomputer Systems, Inc. 之註冊商標。
- 所有其他商標均為其各自合法擁有者之財產。
