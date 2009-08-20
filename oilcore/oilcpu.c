/* -*- coding: utf-8; -*- 
 * Distributed under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 */
#include "oilcpu.h"
#include <string.h>
#include <stdlib.h>

/*
 * SECTION:cpu_detection
 * @short_description: inspect platform information
 * 
 * Provide information of current platform, like what features the current CPU has?
 */

const static char *oil_flags_descriptions[OIL_FLAGBIT_NR] = {
#if OIL_ARCH == mips
    "godson_2f",
#elif OIL_ARCH == i386
    "i386_mmx",
#endif
};

/**
 * oil_cpu_get_flags:
 *
 * Query features of the current CPU.
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

/**
 * oil_cpu_flag_to_string:
 * @flag: a flag, corresponding to a feature of cpu
 *
 * Get the string description of the @flag.
 *
 * Returns: a string describing the @flag or NULL, if @flag is invalid
 */
const char *oil_cpu_flag_to_string (unsigned int flag)
{
    unsigned int i;
    for (i = 0; i < OIL_FLAGBIT_NR; i++) {
        if (flag == OIL_FLAG(i))
            return oil_flags_descriptions[i];
    }
    
    return NULL;
}

/**
 * oil_cpu_string_to_flag:
 * @flag_string: a string describing a flag
 *
 * Give a string description of a flag, return the corresponding flag.
 *
 * Returns: a flag corresponding to @flag_string, or 0, which means @flag_string is invalid
 */
unsigned int oil_cpu_string_to_flag (const char *flag_string)
{
    if (flag_string) {
        unsigned int i;
        
        for (i = 0; i < OIL_FLAGBIT_NR; i++) {
            if (!strcmp (oil_flags_descriptions[i], flag_string))
                return OIL_FLAG(i);
        }
    }
    
    return 0;
}

