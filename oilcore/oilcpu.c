/* -*- coding: utf-8; -*- 
 * Distributed under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 */
#include "oilcpu.h"

/** 
 * SECTION:cpu_detection
 * @short_description: provide CPU info to oiltest
 * 
 * Provide the information about what features the current CPU has?
 */

char *oil_flags_descriptions[OIL_FLAGBIT_NR] = {
#if OIL_ARCH == mips
    "godson_2f",
#elif OIL_ARCH == i386
    "i386_mmx",
#endif
};

/**
 * oil_cpu_get_flags:
 *
 * query features of the current CPU
 *
 * Returns: the flags corresponding the current CPU
 */
unsigned int oil_cpu_get_flags ()
{
    unsigned int flags = 0;
    
#if OIL_ARCH == mips
    /* TODO: detect CPU supported features */
    flags |= OIL_FLAG (OIL_FLAGBIT_GS2F);
#elif OIL_ARCH == i386
    flags |= OIL_FLAG (OIL_FLAGBIT_i386_MMX);
#endif

    return flags;
}
