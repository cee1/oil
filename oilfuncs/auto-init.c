/* -*- coding: utf-8; -*- 
 * Distributed under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 */

#include <oilcore.h>
#include <oiltest.h>
#include "oilfuncs.h"

/**
 * SECTION:oilfuncs_init
 * @short_description: initialization-related functions for oilfuncs
 *
 * Before using functions in oilfuncs, call one of the following initialize functions:
 *   <itemizedlist>
 *     <listitem><para>oilfuncs_init_auto()</para><listitem>
 *     <listitem><para>oilfuncs_init_from_ini()</para><listitem>
 *   </itemizedlist>
 * Call oilclass_uninit() to un-initialize the whole oil libraries.
 */

/**
 * oilfuncs_init_auto:
 * Initialize oilfuncs, it selects implements by doing runtime test
 */
void oilfuncs_init_auto ()
{
    oil_class_init (oil_test_destroy_class_data);
    
    oilfuncs_register_classes ();
    oilfuncs_attach_classes ();
    oilfuncs_register_implements ();
    oil_test_optimize_class_all ();
}
