#include <inttypes.h>

void oil_copy8x8_u8_mmx (uint8_t *dest, int dstr, uint8_t *src, int sstr)
{
    __asm__ __volatile__ (
        "lea         (%2, %2, 2), %%edi     \n\t"

        "movq        (%1), %%mm0            \n\t"
        "movq        (%1, %2), %%mm1        \n\t"
        "movq        (%1, %2, 2), %%mm2     \n\t"
        "movq        (%1, %%edi), %%mm3     \n\t"

        "lea         (%1, %2, 4), %1        \n\t" 

        "movq        %%mm0, (%0)            \n\t"
        "movq        %%mm1, (%0, %2)        \n\t"
        "movq        %%mm2, (%0, %2, 2)     \n\t"
        "movq        %%mm3, (%0, %%edi)     \n\t"

        "lea         (%0, %2, 4), %0        \n\t" 

        "movq        (%1), %%mm0            \n\t"
        "movq        (%1, %2), %%mm1        \n\t"
        "movq        (%1, %2, 2), %%mm2     \n\t"
        "movq        (%1, %%edi), %%mm3     \n\t"

        "movq        %%mm0, (%0)            \n\t"
        "movq        %%mm1, (%0, %2)        \n\t"
        "movq        %%mm2, (%0, %2, 2)     \n\t"
        "movq        %%mm3, (%0, %%edi)     \n\t"
        "emms                               \n\t"
        :   "+a" (dest)
        :   "c" (src),
            "r" (sstr),
            "r" (dstr)
        :   "memory", "edi"
    ); 
}
