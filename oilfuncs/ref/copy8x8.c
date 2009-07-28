#include <inttypes.h>

void oil_copy8x8_u8_ref (uint8_t *d1, int ds, uint8_t *s1, int ss)
{
    int i, j;
    for (i=0; i<8; i++) {
        for (j=0; j<8; j++) {
            d1[j] = s1[j];
        }

        d1 += ds;
        s1 += ss;
    }
}

