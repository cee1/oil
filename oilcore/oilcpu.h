/* -*- coding: utf-8; -*- */
#ifndef __OIL_CPU_H__
#define __OIL_CPU_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <oilconfig.h>
/**
 * OilCPUFlagBits:
 * 
 * FlagBit tells nth bit of the flags, it should be used with OIL_FLAG(), to generate the flag
 *
 * Note flagBit is per-architecture sense: the same flagBit has a different meaning in a different architecture
 */
typedef enum {
#if OIL_ARCH == mips
    OIL_FLAGBIT_GS2F,
#elif OIL_ARCH == i386
    OIL_FLAGBIT_i386_MMX,
#endif
    OIL_FLAGBIT_NR,
} OilCPUFlagBits;

#define OIL_FLAG(bit) (1 << (bit))

const char *oil_cpu_flag_to_string (unsigned int flag);
unsigned int oil_cpu_string_to_flag (const char *flag_string);

unsigned int oil_cpu_get_flags ();

#ifdef __cplusplus
}
#endif

#endif /* __OIL_CPU_H__ */
