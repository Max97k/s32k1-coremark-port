/*
Copyright 2018 Embedded Microprocessor Benchmark Consortium (EEMBC)

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

Port for NXP S32K144W EVB (Cortex-M4F)
Using NXP Real Time Drivers (RTD) & Hardware DWT Timer / LPUART1
*/

#include "coremark.h"
#include "core_portme.h"

#include "StandardTypes.h"
#include "Port_Ci_Port_Ip.h"
#include "Port_Ci_Port_Ip_Cfg.h"
#include "Gpio_Dio_Ip.h"
#include "Clock_Ip.h"
#include "OsIf.h"
#include "DeviceDefinition.h"
#include "S32K144W_LPUART.h"
#include "S32K144W_PORT.h"
#include "S32K144W_PCC.h"

/* Volatile seeds for CoreMark algorithm */
#if VALIDATION_RUN
volatile ee_s32 seed1_volatile = 0x3415;
volatile ee_s32 seed2_volatile = 0x3415;
volatile ee_s32 seed3_volatile = 0x66;
#endif
#if PERFORMANCE_RUN
volatile ee_s32 seed1_volatile = 0x0;
volatile ee_s32 seed2_volatile = 0x0;
volatile ee_s32 seed3_volatile = 0x66;
#endif
#if PROFILE_RUN
volatile ee_s32 seed1_volatile = 0x8;
volatile ee_s32 seed2_volatile = 0x8;
volatile ee_s32 seed3_volatile = 0x8;
#endif
volatile ee_s32 seed4_volatile = ITERATIONS;
volatile ee_s32 seed5_volatile = 0;

ee_u32 default_num_contexts = 1;

/* In-memory buffer for capturing benchmark output */
char              g_coremark_output_buffer[4096];
volatile uint32_t g_coremark_buf_idx = 0;
volatile uint64_t g_coremark_total_cpu_cycles = 0;
volatile uint32_t g_coremark_completed = 0;

/* Hardware pin definitions for S32K144W EVB RGB LED */
#define LED_PORT_GPIO    IP_PTE
#define LED_PIN_GREEN    (0U)  /* PTE0 */
#define LED_PIN_BLUE     (3U)  /* PTE3 */
#define LED_PIN_RED      (7U)  /* PTE7 */

/* DWT (Data Watchpoint and Trace) cycle counter registers */
#define DWT_CTRL         (*(volatile uint32_t *)0xE0001000UL)
#define DWT_CYCCNT       (*(volatile uint32_t *)0xE0001004UL)
#define COREDEBUG_DEMCR  (*(volatile uint32_t *)0xE000EDFCUL)

#define CORE_CLOCK_FREQ_HZ (48000000UL)

static volatile uint32_t s_dwt_last_cyccnt = 0;
static volatile uint64_t s_dwt_accumulated_cycles = 0;

static CORETIMETYPE start_time_val, stop_time_val;

/* Initialize hardware DWT cycle counter */
static void DWT_Init(void)
{
    COREDEBUG_DEMCR |= (1UL << 24); /* Enable Trace (TRCENA) */
    DWT_CYCCNT = 0;                 /* Reset cycle counter */
    DWT_CTRL |= (1UL << 0);         /* Enable CYCCNT (CYCCNTENA) */
    s_dwt_last_cyccnt = DWT_CYCCNT;
    s_dwt_accumulated_cycles = 0;
}

/* Read 64-bit monotonically advancing CPU cycle counter */
static uint64_t DWT_GetCycles(void)
{
    uint32_t now = DWT_CYCCNT;
    uint32_t diff = now - s_dwt_last_cyccnt;
    s_dwt_accumulated_cycles += (uint64_t)diff;
    s_dwt_last_cyccnt = now;
    return s_dwt_accumulated_cycles;
}

/* Initialize LPUART1 on PTC8 (RX) / PTC9 (TX) for OpenSDA Virtual COM */
static void Lpuart1_Init(void)
{
    /* 1. Enable PORTC clock in PCC */
    IP_PCC->PCCn[PCC_PORTC_INDEX] |= PCC_PCCn_CGC_MASK;

    /* 2. Configure OpenSDA Virtual COM on S32K144W EVB:
          PTC8 = ALT2 (LPUART1_RX), PTC9 = ALT2 (LPUART1_TX) */
    IP_PORTC->PCR[8] = PORT_PCR_MUX(2);
    IP_PORTC->PCR[9] = PORT_PCR_MUX(2);

    /* Also configure PTC6 / PTC7 (Expansion header J5) */
    IP_PORTC->PCR[6] = PORT_PCR_MUX(2);
    IP_PORTC->PCR[7] = PORT_PCR_MUX(2);

    /* 3. Enable LPUART1 clock in PCC with FIRCDIV2 (48 MHz, PCS=3) */
    IP_PCC->PCCn[PCC_LPUART1_INDEX] &= ~PCC_PCCn_CGC_MASK;
    IP_PCC->PCCn[PCC_LPUART1_INDEX] = PCC_PCCn_PCS(3) | PCC_PCCn_CGC_MASK;

    /* 4. Disable transmitter and receiver during configuration */
    IP_LPUART1->CTRL = 0U;

    /* 5. Set Baud rate: 48 MHz / (26 * 16) = 115,384 baud (0.16% error from 115200)
          OSR = 25 (meaning 26x oversampling), SBR = 16 */
    IP_LPUART1->BAUD = LPUART_BAUD_OSR(25) | LPUART_BAUD_SBR(16);

    /* 6. Enable transmitter and receiver */
    IP_LPUART1->CTRL = LPUART_CTRL_TE_MASK | LPUART_CTRL_RE_MASK;
}

/* Transmit single character to serial port and log buffer */
void uart_send_char(char c)
{
    /* Record in memory buffer */
    if (g_coremark_buf_idx < sizeof(g_coremark_output_buffer) - 1U)
    {
        g_coremark_output_buffer[g_coremark_buf_idx++] = c;
        g_coremark_output_buffer[g_coremark_buf_idx] = '\0';
    }

    /* Output to hardware LPUART1 */
    if (IP_LPUART1->CTRL & LPUART_CTRL_TE_MASK)
    {
        if (c == '\n')
        {
            while (!(IP_LPUART1->STAT & LPUART_STAT_TDRE_MASK))
            {
                /* Spin wait */
            }
            IP_LPUART1->DATA = (uint32_t)'\r';
        }
        /* Wait until transmit data register is empty */
        while (!(IP_LPUART1->STAT & LPUART_STAT_TDRE_MASK))
        {
            /* Spin wait */
        }
        IP_LPUART1->DATA = (uint32_t)(uint8_t)c;
    }
}

/* Return elapsed milliseconds */
CORETIMETYPE barebones_clock(void)
{
    uint64_t cycles = DWT_GetCycles();
    uint32_t cycles_per_ms = CORE_CLOCK_FREQ_HZ / 1000UL;
    return (CORETIMETYPE)(cycles / cycles_per_ms);
}

void start_time(void)
{
    DWT_Init();
    GETMYTIME(&start_time_val);
}

void stop_time(void)
{
    GETMYTIME(&stop_time_val);
    g_coremark_total_cpu_cycles = DWT_GetCycles();
}

CORE_TICKS get_time(void)
{
    CORE_TICKS elapsed = (CORE_TICKS)MYTIMEDIFF(stop_time_val, start_time_val);
    return elapsed;
}

secs_ret time_in_secs(CORE_TICKS ticks)
{
    secs_ret retval = ((secs_ret)ticks) / (secs_ret)EE_TICKS_PER_SEC;
    return retval;
}

void portable_init(core_portable *p, int *argc, char *argv[])
{
    Clock_Ip_StatusType clockStatus;
    Port_Ci_Port_Ip_PortStatusType portStatus;

    (void)argc;
    (void)argv;

    /* 1. Initialize System Clocks via RTD Clock_Ip */
    clockStatus = Clock_Ip_Init(Clock_Ip_aClockConfig);
    (void)clockStatus;

#if defined (FEATURE_CLOCK_IP_HAS_SPLL_CLK)
    uint32 pllTimeout = 0x100000U;
    while ((CLOCK_IP_PLL_LOCKED != Clock_Ip_GetPllStatus()) && (pllTimeout > 0U))
    {
        pllTimeout--;
    }
    if (pllTimeout > 0U)
    {
        Clock_Ip_DistributePll();
    }
#endif

    /* 2. Initialize GPIO Pins via RTD Port_Ci_Port_Ip */
    portStatus = Port_Ci_Port_Ip_Init(NUM_OF_CONFIGURED_PINS_PortContainer_0_VS_0,
                                      g_pin_mux_InitConfigArr_PortContainer_0_VS_0);
    (void)portStatus;

    /* 3. Initialize RTD OsIf Driver */
    OsIf_Init(NULL_PTR);

    /* 4. Initialize Hardware DWT cycle counter */
    DWT_Init();

    /* 5. Initialize OpenSDA LPUART1 Serial Console */
    Lpuart1_Init();

    /* 6. Turn Green LED ON (benchmarking in progress) */
    Gpio_Dio_Ip_WritePin(LED_PORT_GPIO, LED_PIN_RED,   0U);
    Gpio_Dio_Ip_WritePin(LED_PORT_GPIO, LED_PIN_GREEN, 1U);
    Gpio_Dio_Ip_WritePin(LED_PORT_GPIO, LED_PIN_BLUE,  0U);

    /* 7. Print startup banner */
    ee_printf("\r\n");
    ee_printf("====================================================\r\n");
    ee_printf(" EEMBC CoreMark 1.0 - NXP S32K144W EVB Benchmark   \r\n");
    ee_printf(" Target: S32K144W Cortex-M4F @ %lu MHz             \r\n", CORE_CLOCK_FREQ_HZ / 1000000UL);
    ee_printf(" Toolchain: %s                                     \r\n", COMPILER_VERSION);
    ee_printf(" Iterations: %d                                    \r\n", ITERATIONS);
    ee_printf("====================================================\r\n");
    ee_printf("Benchmarking in progress (Green LED ON)...\r\n");

    /* Validation checks */
    if (sizeof(ee_ptr_int) != sizeof(ee_u8 *))
    {
        ee_printf("ERROR! Please define ee_ptr_int to a type that holds a pointer!\r\n");
    }
    if (sizeof(ee_u32) != 4)
    {
        ee_printf("ERROR! Please define ee_u32 to a 32b unsigned type!\r\n");
    }

    p->portable_id = 1;
}

void portable_fini(core_portable *p)
{
    /* Benchmark complete: Turn Green LED OFF, Turn Blue LED ON (Success) */
    Gpio_Dio_Ip_WritePin(LED_PORT_GPIO, LED_PIN_GREEN, 0U);
    Gpio_Dio_Ip_WritePin(LED_PORT_GPIO, LED_PIN_BLUE,  1U);

    ee_printf("\r\n");
    ee_printf("====================================================\r\n");
    ee_printf(" Benchmark Complete! Blue LED ON.                   \r\n");
    ee_printf(" Total CPU Cycles: %lu                              \r\n", (uint32_t)g_coremark_total_cpu_cycles);
    ee_printf("====================================================\r\n");

    g_coremark_completed = 1U;
    p->portable_id = 0;
}
