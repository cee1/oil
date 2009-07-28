/*
 * oilfuncs.h:
 *  Auto generate from oilfuncs.def
 *  It will be rewritten next time run oil-genclasses
 */

#ifndef __OILFUNCS_H__
#define __OILFUNCS_H__

#include <inttypes.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * oil_copy8x8_u8:
 *   oilfuncs.def:1 
 *   void oil_copy8x8_u8 (uint8_t *d_8x8, int ds, uint8_t *s_8x8, int ss);
 */
extern void (*oil_copy8x8_u8) (uint8_t * d_8x8, int ds, uint8_t * s_8x8, int ss);

extern void oilfuncs_register_classes ();
extern void oilfuncs_register_implements ();

/* for oilfunc-test */
extern void oilfuncs_attach_classes ();

#ifdef __cplusplus
}
#endif

#endif /* __OILFUNCS__ */

