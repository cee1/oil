/*
 * oilfuncs-slots.h:
 *  Auto generate from oilfuncs.def
 *  It will be rewritten next time run oil-genclasses
 */

#ifndef __OILFUNCS_SLOTS_H__
#define __OILFUNCS_SLOTS_H__

#include <inttypes.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * oil_copy8x8_u8:
 *   oilfuncs.def:1 
 *   void oil_copy8x8_u8 (uint8_t *d_8x8, int ds, uint8_t *s_8x8, int ss);
 */
void (*oil_copy8x8_u8) (uint8_t * d_8x8, int ds, uint8_t * s_8x8, int ss);

#ifdef __cplusplus
}
#endif

#endif /* __OILFUNCS_SLOTS_H__ */

