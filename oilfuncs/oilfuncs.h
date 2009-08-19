/* -*- coding: utf-8; -*- 
 * Distributed under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 */

#ifndef __OILFUNCS_H__
#define __OILFUNCS_H__

#include <inttypes.h>
#include <oilfuncs-slots.h>
#include <oilcore.h>

#ifdef __cplusplus
extern "C" {
#endif

extern void oilfuncs_register_classes ();
extern void oilfuncs_register_implements ();

/* for oilfunc-test */
extern void oilfuncs_attach_classes ();

void oilfuncs_init_from_ini ();
void oil_load_ini (const char *ini_string);

void oilfuncs_init_auto ();

#ifdef __cplusplus
}
#endif

#endif /* __OILFUNCS__ */

