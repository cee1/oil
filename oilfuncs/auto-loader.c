/* -*- coding: utf-8; -*- 
 * Distributed under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 */

#include <oilcore.h>
#include <oiltest.h>
#include "oilfuncs.h"

/**
 * oilfuncs_load_auto:
 * a loader for oilfuncs, which activate implement by doing runtime test
 */
void oilfuncs_load_auto ()
{
    oil_class_init (oil_test_destroy_class_data);
    
    oilfuncs_register_classes ();
    oilfuncs_attach_classes ();
    oilfuncs_register_implements ();
    oil_test_optimize_class_all ();
}
