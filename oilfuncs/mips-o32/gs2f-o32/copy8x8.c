#include <inttypes.h>

void oil_copy8x8_u8_gs2f (uint8_t *dest, int dstr, uint8_t *src, int sstr)
{
    __asm__ __volatile__ (
        ".set       mips3                       \n\t"
        "andi      $24, %1, 0x7                 \n\t"
        "bne        $24, $0, ld_unaligned       \n\t"
        "andi      $25, %0, 0x7                 \n\t"
        "ld_aligned:                            \n\t"
        "ld         $8, 0(%1)                   \n\t"
        "addu       %1, %1, %2                  \n\t"
        "ld         $9, 0(%1)                   \n\t"
        "addu       %1, %1, %2                  \n\t"
        "ld         $10, 0(%1)                  \n\t"
        "addu       %1, %1, %2                  \n\t"
        "ld         $11, 0(%1)                  \n\t"
        "addu       %1, %1, %2                  \n\t"
        "ld         $12, 0(%1)                  \n\t"
        "addu       %1, %1, %2                  \n\t"
        "ld         $13, 0(%1)                  \n\t"
        "addu       %1, %1, %2                  \n\t"
        "ld         $14, 0(%1)                  \n\t"
        "addu       %1, %1, %2                  \n\t"
        "ld         $15, 0(%1)                  \n\t"
        "bne        $25, $0, sd_unaligned       \n\t"
        "nop                                    \n\t"
        "sd_aligned:                            \n\t"
        "sd         $8, 0(%0)                   \n\t"
        "addu       %0, %0, %3                  \n\t"
        "sd         $9, 0(%0)                   \n\t"
        "addu       %0, %0, %3                  \n\t"
        "sd         $10, 0(%0)                  \n\t"
        "addu       %0, %0, %3                  \n\t"
        "sd         $11, 0(%0)                  \n\t"
        "addu       %0, %0, %3                  \n\t"
        "sd         $12, 0(%0)                  \n\t"
        "addu       %0, %0, %3                  \n\t"
        "sd         $13, 0(%0)                  \n\t"
        "addu       %0, %0, %3                  \n\t"
        "sd         $14, 0(%0)                  \n\t"
        "addu       %0, %0, %3                  \n\t"
        "sd         $15, 0(%0)                  \n\t"
        "j          finished                    \n\t"
        "nop                                    \n\t"
        "ld_unaligned:                          \n\t"
        "ldr        $8, 0(%1)                   \n\t"
        "ldl        $8, 7(%1)                   \n\t"
        "addu       %1, %1, %2                  \n\t"
        "ldr        $9, 0(%1)                   \n\t"
        "ldl        $9, 7(%1)                   \n\t"
        "addu       %1, %1, %2                  \n\t"
        "ldr        $10, 0(%1)                  \n\t"
        "ldl        $10, 7(%1)                  \n\t"
        "addu       %1, %1, %2                  \n\t"
        "ldr        $11, 0(%1)                  \n\t"
        "ldl        $11, 7(%1)                  \n\t"
        "addu       %1, %1, %2                  \n\t"
        "ldr        $12, 0(%1)                  \n\t"
        "ldl        $12, 7(%1)                  \n\t"
        "addu       %1, %1, %2                  \n\t"
        "ldr        $13, 0(%1)                  \n\t"
        "ldl        $13, 7(%1)                  \n\t"
        "addu       %1, %1, %2                  \n\t"
        "ldr        $14, 0(%1)                  \n\t"
        "ldl        $14, 7(%1)                  \n\t"
        "addu       %1, %1, %2                  \n\t"
        "ldr        $15, 0(%1)                  \n\t"
        "ldl        $15, 7(%1)                  \n\t"
        "beq        $25, $0, sd_aligned         \n\t"
        "nop                                    \n\t"
        "sd_unaligned:                          \n\t"
        "sdr        $8, 0(%0)                   \n\t"
        "sdl        $8, 7(%0)                   \n\t"
        "addu       %0, %0, %3                  \n\t"
        "sdr        $9, 0(%0)                   \n\t"
        "sdl        $9, 7(%0)                   \n\t"
        "addu       %0, %0, %3                  \n\t"
        "sdr        $10, 0(%0)                  \n\t"
        "sdl        $10, 7(%0)                  \n\t"
        "addu       %0, %0, %3                  \n\t"
        "sdr        $11, 0(%0)                  \n\t"
        "sdl        $11, 7(%0)                  \n\t"
        "addu       %0, %0, %3                  \n\t"
        "sdr        $12, 0(%0)                  \n\t"
        "sdl        $12, 7(%0)                  \n\t"
        "addu       %0, %0, %3                  \n\t"
        "sdr        $13, 0(%0)                  \n\t"
        "sdl        $13, 7(%0)                  \n\t"
        "addu       %0, %0, %3                  \n\t"
        "sdr        $14, 0(%0)                  \n\t"
        "sdl        $14, 7(%0)                  \n\t"
        "addu       %0, %0, %3                  \n\t"
        "sdr        $15, 0(%0)                  \n\t"
        "sdl        $15, 7(%0)                  \n\t"
        "addu       %0, %0, %3                  \n\t"
        "finished:                              \n\t"
        :   "+r" (dest)
        :   "r" (src),
            "r" (sstr),
            "r" (dstr)
        :   "memory", "$8", "$9", "$10", "$11", "$12", "$13", "$14", "$15", "$24", "$25"
    ); 
}
