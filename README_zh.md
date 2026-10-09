# EEMBC CoreMark 1.0 移植版（適用於 NXP S32K144W Cortex-M4F）

[![License: Apache 2.0](https://img.shields.io/badge/License-Apache_2.0-blue.svg)](LICENSE)
[![Target: NXP S32K144W](https://img.shields.io/badge/Target-NXP%20S32K144W-orange.svg)](#2-硬體與軟體環境清單)
[![CoreMark: 78.13 @ 48MHz](https://img.shields.io/badge/CoreMark-78.13%20%40%2048MHz-brightgreen.svg)](#1-基準測試實測成績)
[![EEMBC: Validated](https://img.shields.io/badge/EEMBC-Validated-success.svg)](#1-基準測試實測成績)
[![Toolchain: GCC 10.2](https://img.shields.io/badge/Toolchain-GCC%2010.2%20(0%20errors)-brightgreen.svg)](https://gcc.gnu.org)

[English](README.md) | 繁體中文

---

本專案提供針對 **恩智浦半導體 (NXP Semiconductors®) S32K144W** 車規微控制器（Arm® Cortex®-M4F @ 48 MHz 具備硬體 FPU）官方且通過實體硬體驗證的 **EEMBC® CoreMark® 1.0** 基準測試移植，並提供**可直接匯入 S32 Design Studio 建立之完整專案發布目錄**。

本專案特色：
- **隨插即用直接匯入**：提供標準 Eclipse / S32DS 專案結構（`.project`, `.cproject`, `.mex`），使用者 clone 即可直接匯入編譯。
- **零額外負擔硬體精確計時**：透過 ARM Cortex-M4 內部 **DWT 週期計數器** (`DWT->CYCCNT`) 實作。
- **實體 UART 串列資料流**：配置於 S32K14WEVB OpenSDA 虛擬串列埠 (`115200 8N1`)。
- **即時 LED 狀態指示**：綠燈（測試執行中）與藍燈（驗證通過）。
- **雙軌建置流程**：同時支援 S32DS IDE 圖形化操作與純命令列（CLI）自動化建置與燒錄。

---

## 目錄

- [1. 基準測試實測成績](#1-基準測試實測成績)
- [2. 硬體與軟體環境清單](#2-硬體與軟體環境清單)
- [3. 專案目錄結構](#3-專案目錄結構)
- [4. 快速上手與建置指南](#4-快速上手與建置指南)
  - [4.1 環境需求](#41-環境需求)
  - [4.2 方法 A：透過 S32 Design Studio (IDE) 匯入與除錯](#42-方法-a透過-s32-design-studio-ide-匯入與除錯)
  - [4.3 方法 B：透過純命令列 (CLI) 高速編譯與燒錄](#43-方法-b透過純命令列-cli-高速編譯與燒錄)
- [5. 上游專案與參考連結](#5-上游專案與參考連結)
- [6. 商標與版權許可](#6-商標與版權許可)

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

## 3. 專案目錄結構

```
s32k1-coremark-port/
├── board/                         # 腳位與時脈初始配置結構體 (MEX 工具生成)
│   ├── Port_Ci_Port_Ip_Cfg.c
│   └── Port_Ci_Port_Ip_Cfg.h
├── include/                       # 標頭檔目錄
│   ├── check_example.h
│   ├── coremark.h                 # EEMBC CoreMark 1.0 演算法定義
│   └── core_portme.h              # S32K144W 移植定義與計時掛鉤
├── Project_Settings/
│   ├── Debugger/                  # PEMicro GDB 啟動配置檔
│   ├── Linker_Files/              # S32K144W Flash 與 RAM 鏈結腳本 (.ld)
│   └── Startup_Code/              # CMSIS 啟動代碼、中斷向量表、NVIC 配置
├── src/                           # 原始碼目錄
│   ├── core_list_join.c           # 鏈結串列基準測試模組
│   ├── core_main.c                # CoreMark 演算法入口點
│   ├── core_matrix.c              # 矩陣乘法運算模組
│   ├── core_portme.c              # S32K144W 硬體底層移植 (DWT, LPUART1, LED)
│   ├── core_state.c               # 狀態機切換模組
│   ├── core_util.c                # CRC 驗證工具常式
│   └── ee_printf.c                # 輕量化嵌入式格式化字串輸出
├── tools/
│   └── flash_target.py            # 命令列自動化 PEMicro 燒錄工具腳本
├── Gpio_Dio_Ip_Example.mex        # S32 Configuration Tools 視覺化配置工程檔
├── .cproject                      # Eclipse CDT 編譯設定檔
├── .project                       # Eclipse 專案描述檔
├── .gitignore
├── LICENSE                        # Apache 2.0 授權條款
├── README.md                      # 英文說明文件
└── README_zh.md                   # 繁體中文說明文件
```

---

## 4. 快速上手與建置指南

### 4.1 環境需求
1. 安裝 [NXP S32 Design Studio for S32 Platform (v3.6.11 或更新版本)](https://www.nxp.com/s32ds)。
2. 透過 *Help -> Extensions and Updates* 安裝 **PlatformSDK_S32K1_S32M24**（RTD 3.0.0 或相容版本）。
3. 安裝 PEMicro OpenSDA USB 驅動程式。

---

### 4.2 方法 A：透過 S32 Design Studio (IDE) 匯入與除錯

1. 開啟 S32 Design Studio。
2. 點選 **File $\to$ Import... $\to$ General $\to$ Existing Projects into Workspace**。
3. 瀏覽並選取本儲存庫目錄，點擊 **Finish** 匯入。
4. 在 Project Explorer 專案上按滑鼠右鍵，選擇 **S32 Configuration Tool $\to$ Update Code** 自動生成本機 RTD 驅動綁定。
5. 點擊功能表 **Project $\to$ Build Project**（或按 `Ctrl+B`），確認編譯結果為 `0 errors, 0 warnings`。
6. 以 USB 線連接評估板（`J7` 介面），開啟串列終端機軟體（如 PuTTY / Tera Term）連線至 OpenSDA COM 埠（115200 波特率）。
7. 開啟 **Run $\to$ Debug Configurations...**，選擇 `CoreMark_S32K144W_Debug_FLASH_PNE` 並點擊 **Debug**。
8. 在 GDB 中點擊 Resume 執行，板上綠燈將亮起約 15 秒，測試完成後藍燈亮起，串列埠將輸出完整報告。

---

### 4.3 方法 B：透過純命令列 (CLI) 高速編譯與燒錄

本專案支援無介面命令列（Headless CLI）高速編譯與自動燒錄：

```powershell
# 1. 將 S32DS 隨附之建置工具鏈與 GCC 加入環境變數
$env:PATH = "C:\NXP\S32DS.3.6.11\S32DS\build_tools\msys32\usr\bin;C:\NXP\S32DS.3.6.11\S32DS\build_tools\gcc_v10.2\gcc-10.2-arm32-eabi\bin;" + $env:PATH

# 2. 於 Debug_FLASH 目錄下執行 GNU Make 高速編譯
cd Debug_FLASH
make -j8 all

# 3. 執行自動化燒錄腳本透過 OpenSDA 將 ELF 寫入晶片
python ../tools/flash_target.py
```

預期編譯容量報告：
```
   text    data     bss     dec     hex filename
  43868     272   10520   54660    d584 CoreMark_S32K144W.elf
```

---

## 5. 上游專案與參考連結

* [EEMBC CoreMark 官方儲存庫](https://github.com/eembc/coremark) — 官方基準測試演算法套件。
* [NXP S32 Design Studio 官方網頁](https://www.nxp.com/s32ds) — 恩智浦車用微控制器官方 IDE。
* [NXP S32K14WEVB 評估板產品頁面](https://www.nxp.com/design/design-center/development-boards/automotive-development-platforms/s32k-mcu-platforms/s32k14w-q064-evaluation-board-for-automotive-general-purpose:S32K14WEVB-Q064) — 官方硬體規格書與電路圖。
* [EEMBC CoreMark 官方網站](https://www.eembc.org/coremark/) — 評分規範與官方登錄手冊。

---

## 6. 商標與版權許可

- 本專案採用 [Apache License, Version 2.0](LICENSE) 條款授權。
- **EEMBC®** 與 **CoreMark®** 為 Embedded Microprocessor Benchmark Consortium 之註冊商標。
- **NXP®** 與 **S32 Design Studio®** 為 NXP Semiconductors N.V. 之註冊商標。
- **Arm®** 與 **Cortex®-M4F** 為 Arm Limited 之註冊商標。
- **PEMicro®** 與 **OpenSDA** 為 P&E Microcomputer Systems, Inc. 之註冊商標。
- 所有其他商標均為其各自合法擁有者之財產。
