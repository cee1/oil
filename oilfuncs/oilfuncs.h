/*
 * oilfuncs.h:
 *  Auto generate from oilfuncs.def
 *  It will be rewritten next time run oil-genclasses
 */

#ifndef __OILFUNCS_H__
#define __OILFUNCS_H__

#include <inttypes.h>
#include <oilfuncs-slots.h>

#ifdef __cplusplus
extern "C" {
#endif

extern void oilfuncs_register_classes ();
extern void oilfuncs_register_implements ();

/* for oilfunc-test */
extern void oilfuncs_attach_classes ();

#ifdef __cplusplus
}
#endif

#endif /* __OILFUNCS__ */

