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

Target: NXP S32K144W EVB (ARM Cortex-M4F)
S32 Design Studio 3.6 / NXP RTD
*/

#ifndef CORE_PORTME_H
#define CORE_PORTME_H

#include <stddef.h>
#include <stdint.h>

/************************/
/* Data types and settings */
/************************/
#ifndef HAS_FLOAT
#define HAS_FLOAT 1
#endif

#ifndef HAS_TIME_H
#define HAS_TIME_H 0
#endif

#ifndef USE_CLOCK
#define USE_CLOCK 0
#endif

#ifndef HAS_STDIO
#define HAS_STDIO 1
#endif

#ifndef HAS_PRINTF
#define HAS_PRINTF 0
#endif

#ifndef MAIN_HAS_NOARGC
#define MAIN_HAS_NOARGC 1
#endif

#ifndef MAIN_HAS_NORETURN
#define MAIN_HAS_NORETURN 0
#endif

#ifndef MEM_METHOD
#define MEM_METHOD 0 /* MEM_STATIC = 0 */
#endif

#ifndef MULTITHREAD
#define MULTITHREAD 1
#define USE_PTHREAD 0
#define USE_FORK    0
#define USE_SOCKET  0
#endif

#ifndef ITERATIONS
#define ITERATIONS 1200
#endif

#ifndef CORE_DEBUG
#define CORE_DEBUG 0
#endif

#ifndef SEED_METHOD
#define SEED_METHOD SEED_VOLATILE
#endif

#ifndef COMPILER_REQUIRES_SORT_RETURN
#define COMPILER_REQUIRES_SORT_RETURN 0
#endif

#ifndef VALIDATION_RUN
#define VALIDATION_RUN 0
#endif

#ifndef PROFILE_RUN
#define PROFILE_RUN 0
#endif

#ifndef PERFORMANCE_RUN
#define PERFORMANCE_RUN 1
#endif

#ifndef COMPILER_VERSION
#ifdef __GNUC__
#define COMPILER_VERSION "GCC " __VERSION__ " (S32DS)"
#else
#define COMPILER_VERSION "GCC (S32DS)"
#endif
#endif

#ifndef COMPILER_FLAGS
#define COMPILER_FLAGS "-Os -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -specs=nano.specs"
#endif

#ifndef MEM_LOCATION
#define MEM_LOCATION "INTERNAL_SRAM"
#endif

/* Data Types */
typedef signed short   ee_s16;
typedef unsigned short ee_u16;
typedef signed int     ee_s32;
typedef double         ee_f32;
typedef unsigned char  ee_u8;
typedef unsigned int   ee_u32;
typedef ee_u32         ee_ptr_int;
typedef size_t         ee_size_t;

#ifndef NULL
#define NULL ((void *)0)
#endif

#define align_mem(x) (void *)(4 + (((ee_ptr_int)(x)-1) & ~3))

/* Timing parameters (Millisecond Resolution) */
typedef ee_u32 CORETIMETYPE;
typedef ee_u32 CORE_TICKS;

#define CLOCKS_PER_SEC             1000U
#define TIMER_RES_DIVIDER          1U
#define SAMPLE_TIME_IMPLEMENTATION 1
#define EE_TICKS_PER_SEC           (CLOCKS_PER_SEC / TIMER_RES_DIVIDER)

#define GETMYTIME(_t)              (*_t = barebones_clock())
#define MYTIMEDIFF(fin, ini)       ((fin) - (ini))

/* Forward declarations for timing functions */
CORETIMETYPE barebones_clock(void);
void         start_time(void);
void         stop_time(void);
CORE_TICKS   get_time(void);

typedef struct CORE_PORTABLE_S
{
    ee_u8 portable_id;
} core_portable;

void portable_init(core_portable *p, int *argc, char *argv[]);
void portable_fini(core_portable *p);

extern ee_u32 default_num_contexts;

/* Printf and serial IO */
int  ee_printf(const char *fmt, ...);
void uart_send_char(char c);

/* Buffer access for test verification */
extern char              g_coremark_output_buffer[4096];
extern volatile uint32_t g_coremark_buf_idx;
extern volatile uint64_t g_coremark_total_cpu_cycles;
extern volatile uint32_t g_coremark_completed;

#endif /* CORE_PORTME_H */
